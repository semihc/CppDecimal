#include <decimal>
#include <gtest/gtest.h>

using namespace dec;


TEST(DecimalTests, Inclusion) {
  ASSERT_TRUE(sizeof(DecContext)==sizeof(decContext));
  ASSERT_TRUE(sizeof(DecNumber)==sizeof(decNumber));
  ASSERT_TRUE(sizeof(DecSingle)==sizeof(decSingle));
  ASSERT_TRUE(sizeof(DecDouble)==sizeof(decDouble));
  ASSERT_TRUE(sizeof(DecQuad)==sizeof(decQuad));


  decn one{1};
  decn two{2};
  decn three{3.0};
  decn four = 4;

  ASSERT_TRUE(one + two == three);
  ASSERT_TRUE(one + two != decn{"3.1"});
  ASSERT_TRUE(one + three == four);
}


TEST(DecimalTests, BasicUse) {
  decd one{1}; 
  decd two = 2.0;
  decd three = one + two;  
  EXPECT_TRUE(decn{3} == three);

  decn e = 2.71828;       // Euler's number
  decp pi{22.0/7.0};      // The Pi constant

  ASSERT_TRUE(decn(3.14) != pi);
}