#ifndef DECDOUBLE_HH
#define DECDOUBLE_HH
#pragma once

/** \file DecDouble.hh 
 * Declarations for the class DecDouble.
 *
 * (C) Copyright by Semih Cemiloglu
 * All rights reserved, see COPYRIGHT file for details.
 *
 *
 */

#include "DecFwd.hh"
extern "C" {
 #include "decDouble.h"
}
#include "DecContext.hh"
#include "DecPacked.hh"
#include <string>
#include <iosfwd>

namespace dec {

/*!
  \class DecDouble
  DecDouble encapsulates decDouble and provides decNumber library
  functions that operates upon decSingle as member functions with the same name.
  decimal64 is a 64-bit decimal floating-point representation which
  provides 16 decimal digits of precision in a compressed format.
  decDouble module provides the functions for the decimal64 format;
  this format is an IEEE 754 basic format and so a full set of arithmetic
  and other functions is included.
 */
class DecDouble
{
  // MEMBERS
  //! Embedded decDouble structure
  decDouble m_data;

  // STATIC MEMBERS
  static thread_local DecContext s_DecContext;

 public:
  // TYPES
  typedef decDouble* decDoublePtr_t;
  enum {
    MaxStrSize = DECMAXSTRSIZE
  };


  // CREATORS
  //! Default constructor
  DecDouble() { decDoubleZero(&m_data); }

  // Default Dtor and Copy Ctor are ok

  // Constructors using decDouble structure
  DecDouble(decDouble d) : m_data(d) {}
  DecDouble(const decDouble* p) : m_data(*p) {}

  // Conversion constructors using simple types
  DecDouble(const char* str) { fromString(str); }
  DecDouble(int32_t i) { fromInt32(i); }
  DecDouble(uint32_t i) { fromUInt32(i); }
  DecDouble(double d) { fromDouble(d); }
  

  // Conversion constructors using composite types
  DecDouble(const DecQuad& q) { fromDecQuad(q); }
  DecDouble(const DecSingle& s) { fromDecSingle(s); }
  DecDouble(const DecPacked& p) { fromDecPacked(p); }
  DecDouble(const DecNumber& n) { fromDecNumber(n); }

  
  //! Copy assignment
  DecDouble& operator=(const DecDouble& o)
  { return (this != &o ? copy(o) : *this); }

  //! Conversion operator to decDouble*
  operator decDoublePtr_t() { return &m_data; }

  
  // ACCESSORS
  const decDouble* data() const
  { return &m_data; }

  // MODIFIERS
  decDouble* data()
  { return &m_data; }

  // UTILITIES & CONVERSIONS
  DecDouble& fromBCD(int32_t exp, const ByteArr_t& bcd, int32_t sign) {
    decDoubleFromBCD(&m_data, exp, (const uint8_t*)bcd.data(), sign);
    return *this;
  }

  DecDouble& fromDouble(double d);

  DecDouble& fromInt32(int32_t i)
  { decDoubleFromInt32(&m_data, i); return *this; }

  DecDouble& fromPacked(int32_t exp, const ByteArr_t& pack) {
    decDoubleFromPacked(&m_data, exp, (const uint8_t*)pack.data());
    return *this;
  }
  
  DecDouble& fromPackedChecked(int32_t exp, const ByteArr_t& pack) {
    decDoubleFromPackedChecked(&m_data, exp, (const uint8_t*)pack.data());
    return *this;
  }
  
  DecDouble& fromString(const char* str, DecContext* c = nullptr)
  { decDoubleFromString(&m_data, str, Cxt(c)); return *this; }

  //! Hexadecimal string in network byte order
  DecDouble& fromHexString(const char* str);

  DecDouble& fromDecSingle(const DecSingle& s);

  DecDouble& fromDecQuad(const DecQuad& q, DecContext* c = nullptr)
  { return fromWider(q, c); }
  
  DecDouble& fromDecNumber(const DecNumber& n, DecContext* c = nullptr);
  
  DecDouble& fromDecPacked(const DecPacked& p);
  
  DecDouble& fromUInt32(uint32_t i)
  { decDoubleFromUInt32(&m_data, i); return *this; }

  DecDouble& fromWider(const DecQuad& q, DecContext* c = nullptr);
  
  int32_t getCoefficient(ByteArr_t& bcd) const {
    bcd.resize(DECDOUBLE_Pmax);
    return decDoubleGetCoefficient(&m_data, (uint8_t*)bcd.data());
  }

  DecDouble& setCoefficient(const ByteArr_t& bcd, int32_t sign) {
    decDoubleSetCoefficient(&m_data, (const uint8_t*)bcd.data(), sign);
    return *this;
  }
  
  DecDouble& setExponent(int32_t exp, DecContext* c = nullptr ) {
    decDoubleSetExponent(&m_data, Cxt(c), exp);
    return *this;
  }
  
  int32_t toBCD(int32_t& exp, ByteArr_t& bcd) {
    bcd.resize(DECDOUBLE_Pmax);
    return decDoubleToBCD(&m_data, &exp, (uint8_t*)bcd.data());
  }

  double toDouble() const;

  std::string toString() const {
    char str[MaxStrSize] = { 0 };
    return decDoubleToString(&m_data, str);
  }  
  std::string toEngString() const {
    char str[MaxStrSize] = { 0 };
    return decDoubleToEngString(&m_data, str);
  }

  int32_t toPacked(int32_t& exp, ByteArr_t& pack) {
    pack.resize(DECDOUBLE_Pmax);
    return decDoubleToPacked(&m_data, &exp, (uint8_t*)pack.data());
  }
  
  DecSingle toDecSingle(DecContext* c = nullptr) const;

  DecQuad toDecQuad() const;
  
  DecPacked toDecPacked() const;
  
  DecNumber toDecNumber() const;

  DecQuad toWider() const;

  DecDouble& zero()
  { decDoubleZero(&m_data); return *this; }

 
  // COMPUTATIONAL
  //! Returns the absolute value
  DecDouble abs(DecContext* c = nullptr) const
  { decDouble d; return decDoubleAbs(&d, &m_data, Cxt(c)); }

  DecDouble add(const DecDouble& o, DecContext* c = nullptr) const
  { decDouble d; return decDoubleAdd(&d, &m_data, &o.m_data, Cxt(c)); }

  DecDouble digitAnd(const DecDouble& o, DecContext* c = nullptr) const
  { decDouble d; return decDoubleAnd(&d, &m_data, &o.m_data, Cxt(c)); }

  DecDouble divide(const DecDouble& o, DecContext* c = nullptr) const
  { decDouble d; return decDoubleDivide(&d, &m_data, &o.m_data, Cxt(c)); }

  DecDouble divideInteger(const DecDouble& o, DecContext* c = nullptr) const
  { decDouble d; return decDoubleDivideInteger(&d, &m_data, &o.m_data, Cxt(c)); }

  DecDouble fma(const DecDouble& o, const DecDouble& o2,
                 DecContext* c = nullptr) const
  { decDouble d; return decDoubleFMA(&d, &m_data, &o.m_data, &o2.m_data, Cxt(c)); }

  DecDouble invert(DecContext* c = nullptr) const
  { decDouble d; return decDoubleInvert(&d, &m_data, Cxt(c)); }

  DecDouble logB(DecContext* c = nullptr) const
  { decDouble d; return decDoubleLogB(&d, &m_data, Cxt(c)); }

  DecDouble max(const DecDouble& o, DecContext* c = nullptr) const
  { decDouble d; return decDoubleMax(&d, &m_data, &o.m_data, Cxt(c)); }

  DecDouble maxMag(const DecDouble& o, DecContext* c = nullptr) const
  { decDouble d; return decDoubleMaxMag(&d, &m_data, &o.m_data, Cxt(c)); }

  DecDouble minus(DecContext* c = nullptr) const
  { decDouble d; return decDoubleMinus(&d, &m_data, Cxt(c)); }

  DecDouble multiply(const DecDouble& o, DecContext* c = nullptr) const
  { decDouble d; return decDoubleMultiply(&d, &m_data, &o.m_data, Cxt(c)); }

  DecDouble nextMinus(DecContext* c = nullptr) const
  { decDouble d; return decDoubleNextMinus(&d, &m_data, Cxt(c)); }

  DecDouble nextPlus(DecContext* c = nullptr) const
  { decDouble d; return decDoubleNextPlus(&d, &m_data, Cxt(c)); }

  DecDouble nextToward(const DecDouble& o, DecContext* c = nullptr) const
  { decDouble d; return decDoubleNextToward(&d, &m_data, &o.m_data, Cxt(c)); }

  DecDouble digitOr(const DecDouble& o, DecContext* c = nullptr) const
  { decDouble d; return decDoubleOr(&d, &m_data, &o.m_data, Cxt(c)); }

  DecDouble plus(DecContext* c = nullptr) const
  { decDouble d; return decDoublePlus(&d, &m_data, Cxt(c)); }

  DecDouble quantize(const DecDouble& o, DecContext* c = nullptr) const
  { decDouble d; return decDoubleQuantize(&d, &m_data, &o.m_data, Cxt(c)); }

  DecDouble reduce(DecContext* c = nullptr) const
  { decDouble d; return decDoubleReduce(&d, &m_data, Cxt(c)); }
  
  DecDouble remainder(const DecDouble& o, DecContext* c = nullptr) const
  { decDouble d; return decDoubleRemainder(&d, &m_data, &o.m_data, Cxt(c)); }

  DecDouble remainderNear(const DecDouble& o, DecContext* c = nullptr) const
  { decDouble d; return decDoubleRemainderNear(&d, &m_data, &o.m_data, Cxt(c)); }

  DecDouble rotate(const DecDouble& o, DecContext* c = nullptr) const
  { decDouble d; return decDoubleRotate(&d, &m_data, &o.m_data, Cxt(c)); }

  DecDouble scaleB(const DecDouble& o, DecContext* c = nullptr) const
  { decDouble d; return decDoubleScaleB(&d, &m_data, &o.m_data, Cxt(c)); }

  DecDouble shift(const DecDouble& o, DecContext* c = nullptr) const
  { decDouble d; return decDoubleShift(&d, &m_data, &o.m_data, Cxt(c)); }

  DecDouble subtract(const DecDouble& o, DecContext* c = nullptr) const
  { decDouble d; return decDoubleSubtract(&d, &m_data, &o.m_data, Cxt(c)); }

  DecDouble toIntegralValue(enum rounding r, DecContext* c = nullptr) const
  { decDouble d; return decDoubleToIntegralValue(&d, &m_data, Cxt(c), r); }

  DecDouble toIntegralExact(DecContext* c = nullptr) const
  { decDouble d; return decDoubleToIntegralExact(&d, &m_data, Cxt(c)); }

  DecDouble digitXor(const DecDouble& o, DecContext* c = nullptr) const
  { decDouble d; return decDoubleXor(&d, &m_data, &o.m_data, Cxt(c)); }


  // COMPARISONS
  DecDouble compare(const DecDouble& o, DecContext* c = nullptr) const
  { decDouble d; return decDoubleCompare(&d, &m_data, &o.m_data, Cxt(c)); }

  DecDouble compareSignal(const DecDouble& o, DecContext* c = nullptr) const
  { decDouble d; return decDoubleCompareSignal(&d, &m_data, &o.m_data, Cxt(c)); }

  DecDouble compareTotal(const DecDouble& o) const
  { decDouble d; return decDoubleCompareTotal(&d, &m_data, &o.m_data); }

  DecDouble compareTotalMag(const DecDouble& o) const
  { decDouble d; return decDoubleCompareTotalMag(&d, &m_data, &o.m_data); }

  
  // COPIES
  DecDouble& canonical(const DecDouble& d)
  { decDoubleCanonical(&m_data, &d.m_data); return *this; }

  DecDouble& copy(const DecDouble& d)
  { decDoubleCopy(&m_data, &d.m_data); return *this; }

  DecDouble& copyAbs(const DecDouble& d)
  { decDoubleCopyAbs(&m_data, &d.m_data); return *this; }

  DecDouble& copyNegate(const DecDouble& d)
  { decDoubleCopyNegate(&m_data, &d.m_data); return *this; }

  DecDouble& copySign(const DecDouble& d, const DecDouble& sd)
  { decDoubleCopySign(&m_data, &d.m_data, &sd.m_data); return *this; }

  
  // NON-COMPUTATIONAL
  // "class" is a reserved word in C++
  enum decClass classification() const
  { return decDoubleClass(&m_data); }

  const char* classString() const
  { return decDoubleClassString(&m_data); }

  uint32_t digits() const
  { return decDoubleDigits(&m_data); }

  bool isCanonical() const
  { return decDoubleIsCanonical(&m_data); }

  bool isFinite() const
  { return decDoubleIsFinite(&m_data); }
  
  bool isInteger() const
  { return decDoubleIsInteger(&m_data); }

  bool isLogical() const
  { return decDoubleIsLogical(&m_data); }

  bool isInfinite() const
  { return decDoubleIsInfinite(&m_data); }

  bool isNaN() const
  { return decDoubleIsNaN(&m_data); }

  bool isNegative() const
  { return decDoubleIsNegative(&m_data); }

  bool isNormal() const
  { return decDoubleIsNormal(&m_data); }

  bool isPositive() const
  { return decDoubleIsPositive(&m_data); }

  bool isSignaling() const
  { return decDoubleIsSignaling(&m_data); }

  bool isSignalling() const
  { return decDoubleIsSignalling(&m_data); }

  bool isSigned() const
  { return decDoubleIsSigned(&m_data); }

  bool isSubnormal() const
  { return decDoubleIsSubnormal(&m_data); }
  
  bool isZero() const
  { return decDoubleIsZero(&m_data); }
  
  uint32_t radix() const
  { return decDoubleRadix(&m_data); }
  
  const char* version() const
  { return decDoubleVersion(); }


  // RELATIONAL AND LOGICAL OPERATORS
  bool operator==(const DecDouble& o) const
  { return compare(o).isZero(); }

  bool operator!=(const DecDouble& o) const
  { return !(this->operator==(o)); }

  bool operator<(const DecDouble& o) const
  { return compare(o).isNegative(); }

  bool operator<=(const DecDouble& o) const
  { 
    const DecDouble& r = compare(o);
    return r.isNegative() || r.isZero(); 
  }

  bool operator>(const DecDouble& o) const
  { return !(this->operator<=(o)); }

  bool operator>=(const DecDouble& o) const
  { 
    const DecDouble& r = compare(o);
    return !r.isNegative() || r.isZero(); 
  }

  // BITWISE OPERATORS
  DecDouble operator&(const DecDouble& o) const
  { return digitAnd(o); }

  DecDouble operator|(const DecDouble& o) const
  { return digitOr(o); }

  DecDouble operator^(const DecDouble& o) const
  { return digitXor(o); }

  
  // ARITHMETIC OPERATORS
  DecDouble operator+(const DecDouble& o) const
  { return add(o); }

  DecDouble operator-(const DecDouble& o) const
  { return subtract(o); }

  DecDouble operator*(const DecDouble& o) const
  { return multiply(o); }

  DecDouble operator/(const DecDouble& o) const
  { return divide(o); }

  DecDouble operator%(const DecDouble& o) const
  { return remainder(o); }


  // COMPOUND ASSIGNMENT OPERATORS
  DecDouble& operator+=(const DecDouble& o)
  { return copy(add(o)); }

  DecDouble& operator-=(const DecDouble& o)
  { return copy(subtract(o)); }

  DecDouble& operator*=(const DecDouble& o)
  { return copy(multiply(o)); }

  DecDouble& operator/=(const DecDouble& o)
  { return copy(divide(o)); }

  DecDouble& operator%=(const DecDouble& o)
  { return copy(remainder(o)); }

  DecDouble& operator&=(const DecDouble& o)
  { return copy(digitAnd(o)); }

  DecDouble& operator|=(const DecDouble& o)
  { return copy(digitOr(o)); }

  DecDouble& operator^=(const DecDouble& o)
  { return copy(digitXor(o)); }


  // STATIC FUNCTIONS   
  static DecContext& Context()
  { return s_DecContext; }

  static decContext* Cxt(DecContext* c)
  { return (c ? c->data() : s_DecContext.data() ); }


  
}; // end class


/*!
  ostream inserter to pretty-print DecNumber contents.
 */
std::ostream& operator<<(std::ostream& os, const DecDouble& d);


}; // end namespace

#endif /* Include guard */
