# Distributed under the OSI-approved BSD 3-Clause License.  See accompanying
# file Copyright.txt or https://cmake.org/licensing for details.

set(_ANDROID_TOOL_C_COMPILER "")
set(_ANDROID_TOOL_CXX_COMPILER "")
set(_ANDROID_TOOL_PREFIX "")
file(GLOB _gcc "${CMAKE_ANDROID_STANDALONE_TOOLCHAIN}/bin/*-gcc${_ANDROID_HOST_EXT}")
foreach(gcc IN LISTS _gcc)
  if("${gcc}" MATCHES "/bin/([^/]*)gcc${_ANDROID_HOST_EXT}$")
    set(_ANDROID_TOOL_PREFIX "${CMAKE_MATCH_1}")
    break()
  endif()
endforeach()

if(NOT _ANDROID_TOOL_PREFIX)
  message(FATAL_ERROR
    "Android: No '*-gcc' compiler found in CMAKE_ANDROID_STANDALONE_TOOLCHAIN:\n"
    "  ${CMAKE_ANDROID_STANDALONE_TOOLCHAIN}"
    )
endif()

# Help CMakeFindBinUtils locate things.
set(_CMAKE_TOOLCHAIN_PREFIX "${_ANDROID_TOOL_PREFIX}")

# _ANDROID_TOOL_PREFIX should now match `gcc -dumpmachine`.
string(REGEX REPLACE "-$" "" _ANDROID_TOOL_C_TOOLCHAIN_MACHINE "${_ANDROID_TOOL_PREFIX}")

execute_process(
  COMMAND "${CMAKE_ANDROID_STANDALONE_TOOLCHAIN}/bin/${_ANDROID_TOOL_PREFIX}gcc${_ANDROID_HOST_EXT}" -dumpversion
  OUTPUT_VARIABLE _gcc_version
  ERROR_VARIABLE _gcc_error
  OUTPUT_STRIP_TRAILING_WHITESPACE
  )
if(_gcc_version MATCHES "^([0-9]+\\.[0-9]+)")
  set(_ANDROID_TOOL_C_TOOLCHAIN_VERSION "${CMAKE_MATCH_1}")
else()
  message(FATAL_ERROR
    "Android: Failed to extract the standalone toolchain version.  The command:\n"
    "  '${CMAKE_ANDROID_STANDALONE_TOOLCHAIN}/bin/${_ANDROID_TOOL_PREFIX}gcc${_ANDROID_HOST_EXT}' '-dumpversion'\n"
    "produced output:\n"
    "  ${_gcc_version}\n"
    )
endif()

set(_ANDROID_TOOL_C_TOOLCHAIN_PREFIX "${CMAKE_ANDROID_STANDALONE_TOOLCHAIN}/bin/${_ANDROID_TOOL_PREFIX}")
set(_ANDROID_TOOL_C_TOOLCHAIN_SUFFIX "${_ANDROID_HOST_EXT}")

set(_ANDROID_TOOL_CXX_TOOLCHAIN_MACHINE "${_ANDROID_TOOL_C_TOOLCHAIN_MACHINE}")
set(_ANDROID_TOOL_CXX_TOOLCHAIN_VERSION "${_ANDROID_TOOL_C_TOOLCHAIN_VERSION}")
set(_ANDROID_TOOL_CXX_TOOLCHAIN_PREFIX "${_ANDROID_TOOL_C_TOOLCHAIN_PREFIX}")
set(_ANDROID_TOOL_CXX_TOOLCHAIN_SUFFIX "${_ANDROID_TOOL_C_TOOLCHAIN_SUFFIX}")

if(EXISTS "${CMAKE_ANDROID_STANDALONE_TOOLCHAIN}/bin/clang${_ANDROID_HOST_EXT}")
  set(_ANDROID_TOOL_C_COMPILER "${CMAKE_ANDROID_STANDALONE_TOOLCHAIN}/bin/clang${_ANDROID_HOST_EXT}")
  set(_ANDROID_TOOL_C_COMPILER_EXTERNAL_TOOLCHAIN "${CMAKE_ANDROID_STANDALONE_TOOLCHAIN}")
  set(_ANDROID_TOOL_CXX_COMPILER "${CMAKE_ANDROID_STANDALONE_TOOLCHAIN}/bin/clang++${_ANDROID_HOST_EXT}")
  set(_ANDROID_TOOL_CXX_COMPILER_EXTERNAL_TOOLCHAIN "${CMAKE_ANDROID_STANDALONE_TOOLCHAIN}")
else()
  set(_ANDROID_TOOL_C_COMPILER "${_ANDROID_TOOL_C_TOOLCHAIN_PREFIX}gcc${_ANDROID_TOOL_C_TOOLCHAIN_SUFFIX}")
  set(_ANDROID_TOOL_C_COMPILER_EXTERNAL_TOOLCHAIN "")
  set(_ANDROID_TOOL_CXX_COMPILER "${_ANDROID_TOOL_CXX_TOOLCHAIN_PREFIX}g++${_ANDROID_TOOL_CXX_TOOLCHAIN_SUFFIX}")
  set(_ANDROID_TOOL_CXX_COMPILER_EXTERNAL_TOOLCHAIN "")
endif()

set(_ANDROID_TOOL_NDK_TOOLCHAIN_HOST_TAG "")
set(_ANDROID_TOOL_NDK_TOOLCHAIN_VERSION "")
