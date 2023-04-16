# Google Test Framework
add_subdirectory(googletest)
# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)

# Google Flags Library
add_subdirectory(gflags)

# Google Logging Library
add_subdirectory(glog)


