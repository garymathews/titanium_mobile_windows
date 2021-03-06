#!/usr/bin/env bash

# TitaniumKit
#
# Copyright (c) 2014 by Appcelerator, Inc. All Rights Reserved.
# Licensed under the terms of the Apache Public License.
# Please see the LICENSE included with this distribution for details.

if ! test -d "${GTEST_ROOT}"; then
    echo "GTEST_ROOT must point to your Google Test installation."
    exit 1
fi

declare -rx VERBOSE=1

declare -r TitaniumKit_DISABLE_TESTS="OFF"

cmd+="cmake"
cmd+=" -DTitaniumKit_DISABLE_TESTS=${TitaniumKit_DISABLE_TESTS}"
cmd+=" -DHAL_DISABLE_TESTS=ON"

cmake -P cmake/IsWin32.cmake 2>&1 | grep -q -e 1
declare -r CMAKE_HOST_WIN32=${PIPESTATUS[1]}

if [[ ${CMAKE_HOST_WIN32} == 0 ]]; then
    declare -r project_name=$(grep -E '^\s*project[(][^)]+[)]\s*$' CMakeLists.txt | awk 'BEGIN {FS="[()]"} {printf "%s", $2}')
    declare -r solution_file_name="${project_name}.sln"
    declare -r MSBUILD_PATH="c:/Program Files (x86)/MSBuild/12.0/Bin/MSBuild.exe"
    declare -r BUILD_DIR="build"
    cmd+=" ../"
    cmd+=" && \"${MSBUILD_PATH}\" ${solution_file_name}"
else
    declare -r CMAKE_BUILD_TYPE=Debug
    declare -r BUILD_DIR=build.$(echo ${CMAKE_BUILD_TYPE} | tr '[:upper:]' '[:lower:]')
    cmd+=" -DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}"
    cmd+=" ../"
    cmd+=" && make -j 4"
fi

function echo_and_eval {
    local -r cmd="${1:?}"
    echo "${cmd}" && eval "${cmd}"
}

echo_and_eval "rm -rf \"${BUILD_DIR}\""
echo_and_eval "mkdir -p \"${BUILD_DIR}\""
echo_and_eval "pushd \"${BUILD_DIR}\""

# detect use of HAL types for interface (search for virtual, non-final, non-override methods which uses HAL types)
echo_and_eval "find -E ../include -type f -and -name \"*.hpp\" -exec grep -E -H \"JS\w+\" {} \; | grep virtual | grep -v final | grep -v override > DETECT_HAL_TYPES.log"
#
# uncomment following block if you want to exit(1) on use of HAL types for interface
#

#if [ -s DETECT_HAL_TYPES.log ]; then
#  echo "[ERROR] Found virtual and non-final methods which uses HAL types"
#  cat ./DETECT_HAL_TYPES.log
#  exit 1
#fi

echo_and_eval "${cmd} 2>error.log | tee ./build.log"

#
# uncomment following block if you want to exit(1) on compiler warnings
#

if [ -s error.log ]; then
  echo "[ERROR] stderr output found on build."
  cat ./error.log
  exit 1
fi

if [[ ${CMAKE_HOST_WIN32} == 0 ]]; then
    # On Windows we need to copy the DLL to the location of the
    # executables.
    declare -r dll_paths=$(find . -type f -name "*.dll" | sort | uniq | awk '{printf "\"%s\" ", $1}')
    declare -r exe_directories=$(find ./test ./examples -type f -name "*.exe" -exec dirname {} \; | sort | uniq | awk '{printf "\"%s\" ", $0}')
    for exe_directory in ${exe_directories}; do
        for dll_path in ${dll_paths}; do
            echo_and_eval "cp -p ${dll_path} ${exe_directory}"
        done
    done
fi

if [[ "${TitaniumKit_DISABLE_TESTS}" != "ON" ]]; then
    echo_and_eval "ctest -VV --output-on-failure"
fi

echo_and_eval "popd"
