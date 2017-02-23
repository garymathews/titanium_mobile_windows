set(CMAKE_CUDA_VERBOSE_FLAG "-v")

if(NOT "x${CMAKE_CUDA_SIMULATE_ID}" STREQUAL "xMSVC")
  set(CMAKE_CUDA_COMPILE_OPTIONS_PIE -Xcompiler=-fPIE)
  set(CMAKE_CUDA_COMPILE_OPTIONS_PIC -Xcompiler=-fPIC)
  set(CMAKE_CUDA_COMPILE_OPTIONS_VISIBILITY -Xcompiler=-fvisibility=)
  # CMAKE_SHARED_LIBRARY_CUDA_FLAGS is sent to the host linker so we
  # don't need to forward it through nvcc.
  set(CMAKE_SHARED_LIBRARY_CUDA_FLAGS -fPIC)
  string(APPEND CMAKE_CUDA_FLAGS_INIT " ")
  string(APPEND CMAKE_CUDA_FLAGS_DEBUG_INIT " -g")
  string(APPEND CMAKE_CUDA_FLAGS_RELEASE_INIT " -O3 -DNDEBUG")
  string(APPEND CMAKE_CUDA_FLAGS_MINSIZEREL_INIT " -O1 -DNDEBUG")
  string(APPEND CMAKE_CUDA_FLAGS_RELWITHDEBINFO_INIT " -O2 -g -DNDEBUG")
endif()
set(CMAKE_SHARED_LIBRARY_CREATE_CUDA_FLAGS -shared)
set(CMAKE_INCLUDE_SYSTEM_FLAG_CUDA -isystem=)

if("x${CMAKE_CUDA_SIMULATE_ID}" STREQUAL "xMSVC")
  set(CMAKE_CUDA_STANDARD_DEFAULT "")
else()
  set(CMAKE_CUDA_STANDARD_DEFAULT 98)
  set(CMAKE_CUDA98_STANDARD_COMPILE_OPTION "")
  set(CMAKE_CUDA98_EXTENSION_COMPILE_OPTION "")
  set(CMAKE_CUDA11_STANDARD_COMPILE_OPTION "-std=c++11")
  set(CMAKE_CUDA11_EXTENSION_COMPILE_OPTION "-std=c++11")
endif()
