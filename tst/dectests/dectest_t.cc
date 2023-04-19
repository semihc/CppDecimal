#include <decimal>
#include <cstdio>
#include <string>
#include "gtest/gtest.h"
#include "absl/flags/flag.h"
#include "absl/flags/declare.h"

using namespace std;

int testVisitDirectory();
int testProcessDecTestFile();

ABSL_DECLARE_FLAG(std::string, dectests_dir);


TEST(decTests, Directory) {
 
  int rv = 0;
  if( ! absl::GetFlag(FLAGS_dectests_dir).empty() ) {
    // dectests_dir was set on the command line, thus visit and test
    rv = testVisitDirectory();
  }
  ASSERT_TRUE(0 == rv);
}


TEST(decTests, Basic) {
 
  int rv = testProcessDecTestFile();
  ASSERT_TRUE(0 == rv);

}
