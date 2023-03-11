#include <decimal>
#include <gtest/gtest.h>
#include <cstdio>
#include <string>

using namespace std;

int testVisitDirectory();
int testProcessDecTestFile();

TEST(decTests, Basic) {
 
  int rv = testVisitDirectory();
  ASSERT_TRUE(0 == rv);

  rv = testProcessDecTestFile();
  ASSERT_TRUE(0 == rv);

}
