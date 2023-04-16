cmake_minimum_required(VERSION 3.16)
project(CppDecimal VERSION 0.1.0)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)


#add_compile_definitions(DECSUBSET=1)
#set(CMAKE_BUILD_TYPE Debug)

add_subdirectory(src)

# We will use the network to fetch Google Test sources
# Make it possible to disable unit tests when not on network
option(ENABLE_UNIT_TESTS "Enable unit tests" ON)
message(STATUS "Enable testing: ${ENABLE_UNIT_TESTS}")

if(ENABLE_UNIT_TESTS)

Include(FetchContent)
FetchContent_Declare(
  googletest
  GIT_REPOSITORY https://github.com/google/googletest.git
  GIT_TAG release-1.12.1
)
# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

FetchContent_Declare(
  glog
  GIT_REPOSITORY https://github.com/google/glog.git
  GIT_TAG v0.6.0
)
FetchContent_MakeAvailable(glog)

enable_testing()
add_subdirectory(tst)

endif()




