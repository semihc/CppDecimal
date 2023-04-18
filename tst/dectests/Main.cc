#include <gtest/gtest.h>
#include "absl/flags/parse.h"

int main(int argc, char **argv) 
{
  auto remvec = absl::ParseCommandLine(argc, argv);
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
