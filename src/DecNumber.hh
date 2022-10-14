#ifndef DECNUMBER_HH
#define DECNUMBER_HH
#pragma once

/** \file DecNumber.hh 
 * Declarations for the class DecNumber.
 *
 * (C) Copyright by Semih Cemiloglu
 * All rights reserved, see COPYRIGHT file for details.
 *
 *
 */

#include "DecFwd.hh"
extern "C" {
 #include "decNumber.h"
}
#include "DecContext.hh"
#include <cstdint>
#include <string>
#include <iosfwd>

namespace dec {

/*!
  \class DecNumber
  DecNumber encapsulates decNumber and reimplements global functions
  that operates upon decNumber as member functions with the same name.
  decNumber module uses an arbitrary-precision decimal number representation
  designed for efficient computation in software and implements the
  arithmetic and logical operations, together with a number of conversions
  and utilities. Once a number is held as a decNumber, no further conversions
  are necessary to carry out arithmetic.
  The decNumber representation is variable-length and machine-dependent
  (for example, it contains integers which may be big-endian or little-endian).
 */
class DecNumber : public decNumber
{
  // STATIC MEMBERS
  static thread_local DecContext s_DecContext;
  
public:
  // TYPES
  typedef decNumber* decNumberPtr_t;
  enum {
    MaxStrSize = DECMAXSTRSIZE
  };
    
  // CREATORS
  //! Default constructor
  DecNumber()  { decNumberZero(this); }

  // Constructors using decNumber structure
  DecNumber(const decNumber& d) : decNumber(d) {}
  DecNumber(const decNumber* p) : decNumber(*p) {}

  // Conversion constructors using simple types
  DecNumber(const char* str) { fromString(str); }
  // m_data must have space for the digits needed to represent
  // the value of val, which may need up to ten digits.
  DecNumber(uint32_t val) { fromUInt32(val); }
  DecNumber(int32_t val) { fromInt32(val); }
  DecNumber(double d) { fromDouble(d); }

  // Conversion constructors using composite types
  DecNumber(const DecSingle& s);
  DecNumber(const DecDouble& d);
  DecNumber(const DecQuad& q);
  DecNumber(const DecPacked& p);
  
  //! Copy constructor
  DecNumber(const DecNumber& o)
  { decNumberCopy(this, &o); }

  //! Copy assignment
  DecNumber& operator=(const DecNumber& o)
  { if(this != &o) decNumberCopy(this, &o); return *this; }

  //! Type conversion operator to decNumber*
  operator decNumberPtr_t() { return this; }


  // ACCESSORS
  const decNumber* data() const
  { return this; }

  // MODIFIERS
  decNumber* data() 
  { return this; }


  // CONVERSIONS
  DecNumber& fromBCD(std::string& bcd) {
    decNumberSetBCD(this, (const uint8_t*)bcd.data(), bcd.size());
    return *this;
  }

  DecNumber& fromDouble(double d);
  
  DecNumber& fromInt32(int32_t val)
  { decNumberFromInt32(this, val); return *this; }

  DecNumber& fromUInt32(uint32_t val)
  { decNumberFromUInt32(this, val); return *this; }

  DecNumber& fromString(const char* str, DecContext* c = nullptr)
  { decNumberFromString(this, str, Cxt(c)); return *this; }

  DecNumber& fromString(const std::string& str, DecContext* c = nullptr)
  { decNumberFromString(this, str.data(), Cxt(c)); return *this; }

  std::string toBCD() const {
    std::string bcd(digits+1, '\0');
    decNumberGetBCD(this, (uint8_t*)bcd.data());
    return bcd;
  }

  double toDouble() const;
      
  std::string toEngString() const {
    char str[MaxStrSize] = { 0 };
    return decNumberToEngString(this, str);
  }

  std::string toString() const {
    char str[MaxStrSize] = { 0 };
    return decNumberToString(this, str);
  }

  int32_t toInt32(DecContext* c = nullptr) const
  { return decNumberToInt32(this, Cxt(c)); }
  
  uint32_t toUInt32(DecContext* c = nullptr) const
  { return decNumberToUInt32(this, Cxt(c)); }

  
  // COMPUTATIONAL FUNCTIONS
  DecNumber abs(DecContext* c = nullptr) const
  { decNumber n; return decNumberAbs(&n, this, Cxt(c)); }

  DecNumber add(const DecNumber& o, DecContext* c = nullptr) const
  { decNumber n; return decNumberAdd(&n, this, &o, Cxt(c)); }

  DecNumber digitAnd(const DecNumber& o, DecContext* c = nullptr) const
  { decNumber n; return decNumberAnd(&n, this, &o, Cxt(c)); }

  DecNumber compare(const DecNumber& o, DecContext* c = nullptr) const
  { decNumber n; return decNumberCompare(&n, this, &o, Cxt(c)); }

  DecNumber compareSignal(const DecNumber& o, DecContext* c = nullptr) const
  { decNumber n; return decNumberCompareSignal(&n, this, &o, Cxt(c)); }

  DecNumber compareTotal(const DecNumber& o, DecContext* c = nullptr) const
  { decNumber n; return decNumberCompareTotal(&n, this, &o, Cxt(c)); }

  DecNumber compareTotalMag(const DecNumber& o, DecContext* c = nullptr) const
  { decNumber n; return decNumberCompareTotalMag(&n, this, &o, Cxt(c)); }
  
  DecNumber divide(const DecNumber& o, DecContext* c = nullptr) const
  { decNumber n; return decNumberDivide(&n, this, &o, Cxt(c)); }

  DecNumber divideInteger(const DecNumber& o, DecContext* c = nullptr) const
  { decNumber n; return decNumberDivideInteger(&n, this, &o, Cxt(c)); }

  DecNumber exp(DecContext* c = nullptr) const
  { decNumber n; return decNumberExp(&n, this, Cxt(c)); }

  DecNumber fma(const DecNumber& mo, const DecNumber& ao,
                 DecContext* c = nullptr) const {
    decNumber n; 
    return decNumberFMA(&n, this, &mo, &ao, Cxt(c));
  }

  DecNumber invert(DecContext* c = nullptr) const
  { decNumber n; return decNumberInvert(&n, this, Cxt(c));  }

  DecNumber ln(DecContext* c = nullptr) const
  { decNumber n; return decNumberLn(&n, this, Cxt(c));  }

  DecNumber logB(DecContext* c = nullptr) const
  { decNumber n; return decNumberLogB(&n, this, Cxt(c));  }

  DecNumber log10(DecContext* c = nullptr) const
  { decNumber n; return decNumberLog10(&n, this, Cxt(c));  }

  DecNumber max(const DecNumber& o, DecContext* c = nullptr) const
  { decNumber n; return decNumberMax(&n, this, &o, Cxt(c)); }

  DecNumber maxMag(const DecNumber& o, DecContext* c = nullptr) const
  { decNumber n; return decNumberMaxMag(&n, this, &o, Cxt(c)); }
  
  DecNumber min(const DecNumber& o, DecContext* c = nullptr) const
  { decNumber n; return decNumberMin(&n, this, &o, Cxt(c)); }

  DecNumber minMag(const DecNumber& o, DecContext* c = nullptr) const
  { decNumber n; return decNumberMinMag(&n, this, &o, Cxt(c)); }

  DecNumber minus(DecContext* c = nullptr) const
  { decNumber n; return decNumberMinus(&n, this, Cxt(c)); }

  DecNumber multiply(const DecNumber& o, DecContext* c = nullptr) const
  { decNumber n; return decNumberMultiply(&n, this, &o, Cxt(c)); }

  DecNumber nextMinus(DecContext* c = nullptr)
  { decNumber n; return decNumberNextMinus(&n, this, Cxt(c)); }
  
  DecNumber nextPlus(DecContext* c = nullptr)
  { decNumber n; return decNumberNextPlus(&n, this, Cxt(c)); } 

  DecNumber nextToward(const DecNumber& o, DecContext* c = nullptr)
  { decNumber n; return decNumberNextToward(&n, this, &o, Cxt(c)); }

  DecNumber normalize(DecContext* c = nullptr) const
  { decNumber n; return decNumberNormalize(&n, this, Cxt(c));  }

  DecNumber digitOr(const DecNumber& o, DecContext* c = nullptr) const
  { decNumber n; return decNumberOr(&n, this, &o, Cxt(c)); } 
  
  DecNumber plus(DecContext* c = nullptr) const
  { decNumber n; return decNumberPlus(&n, this, Cxt(c)); }

  DecNumber power(const DecNumber& o, DecContext* c = nullptr) const
  { decNumber n; return decNumberPower(&n, this, &o, Cxt(c)); }

  DecNumber quantize(const DecNumber& o, DecContext* c = nullptr) const
  { decNumber n; return decNumberQuantize(&n, this, &o, Cxt(c)); }

  DecNumber reduce(DecContext* c = nullptr) const
  { decNumber n; return decNumberReduce(&n, this, Cxt(c)); }
  
  DecNumber remainder(const DecNumber& o, DecContext* c = nullptr) const
  { decNumber n; return decNumberRemainder(&n, this, &o, Cxt(c)); }

  DecNumber remainderNear(const DecNumber& o, DecContext* c = nullptr) const
  { decNumber n; return decNumberRemainderNear(&n, this, &o, Cxt(c)); }

  DecNumber rescale(const DecNumber& o, DecContext* c = nullptr) const
  { decNumber n; return decNumberRescale(&n, this, &o, Cxt(c)); }

  DecNumber rotate(const DecNumber& o, DecContext* c = nullptr) const
  { decNumber n; return decNumberRotate(&n, this, &o, Cxt(c)); }
  
  bool sameQuantum(const DecNumber& o) const {
    decNumber n; 
    DecNumber r = decNumberSameQuantum(&n, data(), o.data());
    return !r.isZero();
  }

  DecNumber scaleB(const DecNumber& o, DecContext* c = nullptr) const
  { decNumber n; return decNumberScaleB(&n, this, &o, Cxt(c)); }

  DecNumber shift(const DecNumber& o, DecContext* c = nullptr) const
  { decNumber n; return decNumberShift(&n, this, &o, Cxt(c)); }
  
  DecNumber squareRoot(DecContext* c = nullptr) const
  { decNumber n; return decNumberSquareRoot(&n, this, Cxt(c));  }

  DecNumber subtract(const DecNumber& o, DecContext* c = nullptr) const
  { decNumber n; return decNumberSubtract(&n, this, &o, Cxt(c)); }

  DecNumber toIntegralExact(DecContext* c = nullptr) const
  { decNumber n; return decNumberToIntegralExact(&n, this, Cxt(c)); }

  DecNumber toIntegralValue(DecContext* c = nullptr) const
  { decNumber n; return decNumberToIntegralValue(&n, this, Cxt(c)); }

  DecNumber digitXor(const DecNumber& o, DecContext* c = nullptr) const
  { decNumber n; return decNumberXor(&n, this, &o, Cxt(c)); } 

  
  // TESTING FUNCTIONS
  bool isCanonical() const
  { return decNumberIsCanonical(this); }

  bool isFinite() const
  { return decNumberIsFinite(this); }

  bool isInfinite() const
  { return decNumberIsInfinite(this); }

  bool isNaN() const
  { return decNumberIsNaN(this); }

  bool isNegative() const
  { return decNumberIsNegative(this); }

  bool isQNaN() const
  { return decNumberIsQNaN(this); }

  bool isSNaN() const
  { return decNumberIsSNaN(this); }

  bool isSpecial() const
  { return decNumberIsSpecial(this); }

  bool isZero() const
  { return decNumberIsZero(this); }
  

  // TEST FUNCTIONS (CONTEXT DEPENDENT)
  bool isNormal(DecContext* c = nullptr) const
  { return decNumberIsNormal(this, Cxt(c)); }

  bool isSubnormal(DecContext* c = nullptr) const
  { return decNumberIsSubnormal(this, Cxt(c)); }

  

  // UTILITIES
  enum decClass toClass(DecContext* c = nullptr) const
  { return decNumberClass(this, Cxt(c)); }

  DecNumber& copy(const DecNumber& o)
  { decNumberCopy(this, &o); return *this; }

  DecNumber& copyAbs(const DecNumber& o)
  { decNumberCopyAbs(this, &o); return *this; }
    
  DecNumber& copyNegate(const DecNumber& o)
  { decNumberCopyNegate(this, &o); return *this; }

  DecNumber& copySign(const DecNumber& o, const DecNumber& so)
  { decNumberCopySign(this, &o, &so); return *this; }

  uint32_t radix() const
  { return decNumberRadix(this); }
  
  DecNumber& trim()
  { decNumberTrim(this); return *this; }

  const char* version() const
  { return decNumberVersion(); }

  DecNumber& zero()
  { decNumberZero(this); return *this; }

  
  // STATIC FUNCTIONS (UTILITIES)
  static const char* ClassToString(enum decClass dc)
  { return decNumberClassToString(dc); }
  
  static const char* Version()
  { return decNumberVersion(); }

  static DecContext& Context()
  { return s_DecContext; }

  static decContext* Cxt(DecContext* c)
  { return (c ? c->data() : s_DecContext.data() ); }


  // RELATIONAL AND LOGICAL OPERATORS
  bool operator==(const DecNumber& o) const
  { return compare(o).isZero(); }

  bool operator!=(const DecNumber& o) const
  { return !(this->operator==(o)); }

  bool operator<(const DecNumber& o) const
  { return compare(o).isNegative(); }

  bool operator<=(const DecNumber& o) const
  { 
    const DecNumber& r = compare(o);
    return r.isNegative() || r.isZero(); 
  }

  bool operator>(const DecNumber& o) const
  { return !(this->operator<=(o)); }

  bool operator>=(const DecNumber& o) const
  { 
    const DecNumber& r = compare(o);
    return !r.isNegative() || r.isZero(); 
  }

  // BITWISE OPERATORS
  DecNumber operator&(const DecNumber& o) const
  { return digitAnd(o); }

  DecNumber operator|(const DecNumber& o) const
  { return digitOr(o); }

  DecNumber operator^(const DecNumber& o) const
  { return digitXor(o); }


  // ARITHMETIC OPERATORS
  DecNumber operator+(const DecNumber& o) const
  { return add(o); }

  DecNumber operator-(const DecNumber& o) const
  { return subtract(o); }

  DecNumber operator*(const DecNumber& o) const
  { return multiply(o); }

  DecNumber operator/(const DecNumber& o) const
  { return divide(o); }

  DecNumber operator%(const DecNumber& o) const
  { return remainder(o); }


  // COMPOUND ASSIGNMENT OPERATORS
  DecNumber& operator+=(const DecNumber& o)
  { return copy(add(o)); }

  DecNumber& operator-=(const DecNumber& o)
  { return copy(subtract(o)); }

  DecNumber& operator*=(const DecNumber& o)
  { return copy(multiply(o)); }

  DecNumber& operator/=(const DecNumber& o)
  { return copy(divide(o)); }

  DecNumber& operator%=(const DecNumber& o)
  { return copy(remainder(o)); }

  DecNumber& operator&=(const DecNumber& o)
  { return copy(digitAnd(o)); }

  DecNumber& operator|=(const DecNumber& o)
  { return copy(digitOr(o)); }

  DecNumber& operator^=(const DecNumber& o)
  { return copy(digitXor(o)); }


}; // end class


/*!
  ostream inserter to pretty-print DecNumber contents.
 */
std::ostream& operator<<(std::ostream& os, const DecNumber&);


}; // end namespace

#endif /* Include guard */
