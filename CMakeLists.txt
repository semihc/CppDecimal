cmake_minimum_required(VERSION 3.16)
project(CppDecimal VERSION 0.1.0)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED OFF)

if (WIN32)
  # Concurrent compile support (maybe /FS also?)
  add_compile_options(/MP)
endif()

# External dependancies/subprojects
add_subdirectory(ext)

add_subdirectory(src)

# We will use the network to fetch Google Test sources
# Make it possible to disable unit tests when not on network
option(ENABLE_UNIT_TESTS "Enable unit tests" ON)
message(STATUS "Enable testing: ${ENABLE_UNIT_TESTS}")

if(ENABLE_UNIT_TESTS)

enable_testing()
add_subdirectory(tst)

endif()




