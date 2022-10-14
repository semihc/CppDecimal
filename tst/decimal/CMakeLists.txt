add_executable(decimal_test
  decimal_t.cc 
  Main.cc
)

add_test(NAME decimal_test COMMAND decimal_test)

target_include_directories(decimal_test
  PUBLIC
    ${CMAKE_CURRENT_LIST_DIR}
    ${PROJECT_SOURCE_DIR}/src
)

target_link_libraries(decimal_test PRIVATE gtest decimal)