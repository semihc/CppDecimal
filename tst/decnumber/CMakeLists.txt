add_executable(decnumber_test
  decnumber_t.cc 
  Main.cc
)

add_test(NAME decnumber_test COMMAND decnumber_test)

target_include_directories(decnumber_test
  PUBLIC
    ${CMAKE_CURRENT_LIST_DIR}
    ${PROJECT_SOURCE_DIR}/src
)

target_link_libraries(decnumber_test PRIVATE gtest decimal)