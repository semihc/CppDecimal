add_library(decimal  
 STATIC
  decContext.c 
  decimal32.c
  decimal64.c
  decimal128.c  
  decNumber.c 
  decPacked.c 
  decSingle.c 
  decDouble.c
  decQuad.c  
  DecContext.cc
  DecNumber.cc
  DecSingle.cc
  DecDouble.cc
  DecQuad.cc
  DecPacked.cc
)

#target_include_directories(decnumber 
# INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}
#)
target_include_directories(decimal PUBLIC .)




# we will use the network to fetch Google Test sources
# make it possible to disable unit tests when not on network
option(ENABLE_UNIT_TESTS "Enable unit tests" ON)
message(STATUS "Enable testing: ${ENABLE_UNIT_TESTS}")

if(ENABLE_UNIT_TESTS)
  # the following code to fetch googletest
  # is inspired by and adapted after:
  #   - https://cmake.org/cmake/help/v3.11/module/FetchContent.html
  include(FetchContent)

  FetchContent_Declare(
    googletest
    GIT_REPOSITORY https://github.com/google/googletest.git
    GIT_TAG        release-1.12.1
  )

  FetchContent_GetProperties(googletest)

  if(NOT googletest_POPULATED)
    FetchContent_Populate(googletest)

    # Prevent GoogleTest from overriding our compiler/linker options
    # when building with Visual Studio
    set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
    # Prevent GoogleTest from using PThreads
    set(gtest_disable_pthreads ON CACHE BOOL "" FORCE)

    # adds the targers: gtest, gtest_main, gmock, gmock_main
    add_subdirectory(
      ${googletest_SOURCE_DIR}
      ${googletest_BINARY_DIR}
      )
    
  endif()


  #[[
  add_executable(cpp_test "")

  target_sources(cpp_test
    PRIVATE
      test.cpp
    )

  target_link_libraries(cpp_test
    PRIVATE
      sum_integers
      gtest_main
    )

  enable_testing()

  add_test(
    NAME google_test
    COMMAND $<TARGET_FILE:cpp_test>
    )
  ]]
endif()
