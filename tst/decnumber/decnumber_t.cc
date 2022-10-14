#include <decimal>
#include <gtest/gtest.h>
#include <cstdio>
#include <string>

using namespace std;



TEST(decNumberTests, SimpleAddition) {
 
  decNumber a, b; // working numbers 
  decContext set; // working context 
  char str[DECNUMDIGITS+14]; // conversion buffer 
  decContextDefault(&set, DEC_INIT_BASE); // initialize 
  set.traps=0; // no traps, thank you
  set.digits=DECNUMDIGITS; // set precision
  
  char arg1[] = {"1"};
  char arg2[] = {"2"};

  decNumberFromString(&a, arg1, &set);
  decNumberFromString(&b, arg2, &set);
  decNumberAdd(&a, &a, &b, &set); // a=a+b 
  decNumberToString(&a, str);
  printf("%s + %s => %s\n", arg1, arg2, str); 

  ASSERT_TRUE(3==atoi(str));
}


TEST(decNumberTests, CompoundInterest)
{
  decNumber one, mtwo, hundred; // constants
  decNumber start, rate, years; // parameters
  decNumber total; // result
  decContext set; // working context
  char str[DECNUMDIGITS+14]; // conversion buffer
 
  decContextDefault(&set, DEC_INIT_BASE); // initialize
  set.traps=0; // no traps
  set.digits=25; // precision 25
  decNumberFromString(&one, "1", &set); // set constants
  decNumberFromString(&mtwo, "-2", &set);
  decNumberFromString(&hundred, "100", &set);

  ASSERT_TRUE(0 == set.status);
  
  decNumberFromString(&start, "100000", &set); // parameter words
  decNumberFromString(&rate, "6.5", &set);
  decNumberFromString(&years, "20", &set);
  
  decNumberDivide(&rate, &rate, &hundred, &set); // rate=rate/100
  decNumberAdd(&rate, &rate, &one, &set); // rate=rate+1
  decNumberPower(&rate, &rate, &years, &set); // rate=rate**years
  decNumberMultiply(&total, &rate, &start, &set); // total=rate*start
  decNumberRescale(&total, &total, &mtwo, &set); // two digits please

  EXPECT_TRUE(0 == (set.status & DEC_Errors));

  decNumberToString(&total, str);

  ASSERT_TRUE(0==strcmp(str,"352364.51"));
}


TEST(decNumberTests, CompressedFormats)
{
  decimal64 a; // working decimal64 number
  decNumber d; // working number
  decContext set; // working context
  char str[DECIMAL64_String]; // number>string buffer
  char hexes[25]; // decimal64>hex buffer
  int i; // counter
   
  decContextDefault(&set, DEC_INIT_DECIMAL64); // initialize
  
  char arg1[] = {"79"};
  decimal64FromString(&a, arg1, &set);
  // lay out the decimal64 as eight hexadecimal pairs
  for (i=0; i<8; i++) {
    sprintf(&hexes[i*3], "%02x ", a.bytes[i]);
  }
  decimal64ToNumber(&a, &d);
  decNumberToString(&d, str);

  EXPECT_TRUE(0==strcmp(str, arg1));
  // Little endian:
  EXPECT_TRUE(0==strcmp(hexes, "79 00 00 00 00 00 38 22 "));
  // Big endian:
  //EXPECT_TRUE(0==strcmp(hexes, "22 38 00 00 00 00 00 79 "));
  printf("%s => %s=> %s\n", arg1, hexes, str);
}


TEST(decNumberTests, PackedDecimals)
{
  uint8_t startpack[]={0x01, 0x00, 0x00, 0x0C}; // investment=100000
  int32_t startscale=0;
  uint8_t ratepack[]={0x06, 0x5C}; // rate=6.5%
  int32_t ratescale=1;
  uint8_t yearspack[]={0x02, 0x0C}; // years=20
  int32_t yearsscale=0;
  uint8_t respack[16]; // result, packed
  int32_t resscale; // ..
  char hexes[49]; // for packed>hex
  int i; // counter

  decNumber one, mtwo, hundred; // constants
  decNumber start, rate, years; // parameters
  decNumber total; // result
  decContext set; // working context
 
  decContextDefault(&set, DEC_INIT_BASE); // initialize
  set.traps=0; // no traps
  set.digits=25; // precision 25
  decNumberFromString(&one, "1", &set); // set constants
  decNumberFromString(&mtwo, "-2", &set);
  decNumberFromString(&hundred, "100", &set);

  EXPECT_TRUE(0 == set.status);

  decPackedToNumber(startpack, sizeof(startpack), &startscale, &start);
  decPackedToNumber(ratepack, sizeof(ratepack), &ratescale, &rate);
  decPackedToNumber(yearspack, sizeof(yearspack), &yearsscale, &years);
  
  decNumberDivide(&rate, &rate, &hundred, &set); // rate=rate/100
  decNumberAdd(&rate, &rate, &one, &set); // rate=rate+1
  decNumberPower(&rate, &rate, &years, &set); // rate=rate**years
  decNumberMultiply(&total, &rate, &start, &set); // total=rate*start
  decNumberRescale(&total, &total, &mtwo, &set); // two digits please
  
  decPackedFromNumber(respack, sizeof(respack), &resscale, &total);
  
  // lay out the total as sixteen hexadecimal pairs
  for (i=0; i<16; i++) {
    sprintf(&hexes[i*3], "%02x ", respack[i]);
  }


  EXPECT_TRUE(resscale == 2);
  ASSERT_TRUE(0==strcmp(hexes, "00 00 00 00 00 00 00 00 00 00 00 03 52 36 45 1c "));
  printf("Result: %s (scale=%ld)\n", hexes, (long int)resscale);

}


TEST(decNumberTests, QuadTest)
{
  decQuad a, b; // working decQuads
  decContext set; // working context
  char str[DECQUAD_String]; // number>string buffer

  decContextDefault(&set, DEC_INIT_DECQUAD); // initialize

  char arg1[] = {"1.123456"};
  char arg2[] = {"2.111111"}; 

  decQuadFromString(&a, arg1, &set);
  decQuadFromString(&b, arg2, &set);
  decQuadAdd(&a, &a, &b, &set); // a=a+b
  decQuadToString(&a, str);

  ASSERT_TRUE(0==strcmp(str, "3.234567"));
  printf("%s + %s => %s\n", arg1, arg2, str);
}


TEST(decNumberTests, QuadWithNumber)
{
  decQuad a; // working decQuad
  decNumber numa, numb; // working decNumbers
  decContext set; // working context
  char str[DECQUAD_String]; // number>string buffer

  decContextDefault(&set, DEC_INIT_DECQUAD); // initialize

  char arg1[] = {"1.0"};
  char arg2[] = {"2.0"}; 

  decQuadFromString(&a, arg1, &set); // get a
  decQuadAdd(&a, &a, &a, &set); // double a
  decQuadToNumber(&a, &numa); // convert to decNumber
  decNumberFromString(&numb, arg2, &set);
  decNumberPower(&numa, &numa, &numb, &set); // numa=numa**numb
  decQuadFromNumber(&a, &numa, &set); // back via a Quad
  decQuadToString(&a, str); // ..

  ASSERT_TRUE(0==strcmp(str, "4.00"));
  printf("power(2*%s, %s) => %s\n", arg1, arg2, str);
}
