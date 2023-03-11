
add_executable(dectest_runner
  DTRunner.cc 
  dectest_t.cc
  Main.cc
)

add_test(NAME dectest_runner COMMAND dectest_runner)

target_include_directories(dectest_runner
  PUBLIC
    ${CMAKE_CURRENT_LIST_DIR}
    ${PROJECT_SOURCE_DIR}/src
)

target_link_libraries(dectest_runner PRIVATE gtest decimal)