#ifndef DECQUAD_HH
#define DECQUAD_HH

/** \file DecQuad.hh 
 * Declarations for the class DecQuad.
 *
 * (C) Copyright by Semih Cemiloglu
 * All rights reserved, see COPYRIGHT file for details.
 *
 *
 */

#include "DecFwd.hh"
extern "C" {
 #include "decQuad.h"
}
#include "DecContext.hh"
#include "DecPacked.hh"
#include <string>
#include <iosfwd>


namespace dec {

/*!
  \class DecQuad
  DecQuad encapsulates decQuad and provides decNumber library functions
  that operates upon decSingle as member functions with the same name.
  decimal128 is a 128-bit decimal floating-point representation which
  provides 34 decimal digits of precision in a compressed format.
  decQuad module provides the functions for the decimal128 format;
  this format is an IEEE 754 basic format; it contains the same set of
  functions as decDouble.
 */
class DecQuad
{
  // MEMBERS
  //! Embedded decQuad structure
  decQuad m_data;
  
  // STATIC MEMBERS
  static thread_local DecContext s_DecContext;

 public:
  // TYPES
  typedef decQuad* decQuadPtr_t;
  enum {
    MaxStrSize = DECMAXSTRSIZE
  };

  // CREATORS
  //! Default constructor
  DecQuad() { decQuadZero(&m_data); }

  // Default Dtor and Copy Ctor are ok

  // Constructors using decQuad structure
  DecQuad(decQuad d) : m_data(d) {}
  DecQuad(const decQuad* p) : m_data(*p) {}

  // Conversion constructors using simple types
  DecQuad(const char* str) { fromString(str); }
  DecQuad(int32_t i) { fromInt32(i); }
  DecQuad(uint32_t i) { fromUInt32(i); }
  DecQuad(double d) { fromDouble(d); }
  

  // Conversion constructors using composite types
  DecQuad(const DecDouble& d) { fromDecDouble(d); }
  DecQuad(const DecPacked& p) { fromDecPacked(p); }
  DecQuad(const DecNumber& n) { fromDecNumber(n); }

  
  //! Copy assignment
  DecQuad& operator=(const DecQuad& o)
  { return (this != &o ? copy(o) : *this); }

  //! Conversion operator to decQuad*
  operator decQuadPtr_t() { return &m_data; }


  // ACCESSORS
  const decQuad* data() const
  { return &m_data; }

  // MODIFIERS
  decQuad* data()
  { return &m_data; }

  // UTILITIES & CONVERSIONS
  DecQuad& fromBCD(int32_t exp, const ByteArr_t& bcd, int32_t sign) {
    decQuadFromBCD(&m_data, exp, (const uint8_t*)bcd.data(), sign);
    return *this;
  }

  DecQuad& fromDouble(double d);

  DecQuad& fromInt32(int32_t i)
  { decQuadFromInt32(&m_data, i); return *this; }

  DecQuad& fromPacked(int32_t exp, const ByteArr_t& pack) {
    decQuadFromPacked(&m_data, exp, (const uint8_t*)pack.data());
    return *this;
  }
  
  DecQuad& fromPackedChecked(int32_t exp, const ByteArr_t& pack) {
    decQuadFromPackedChecked(&m_data, exp, (const uint8_t*)pack.data());
    return *this;
  }
  
  DecQuad& fromString(const char* str, DecContext* c = nullptr)
  { decQuadFromString(&m_data, str, Cxt(c)); return *this; }

  //! Hexadecimal string in network byte order
  DecQuad& fromHexString(const char* str);
  
  DecQuad& fromDecDouble(const DecDouble& d);

  DecQuad& fromDecNumber(const DecNumber& n, DecContext* c = nullptr);
  
  DecQuad& fromDecPacked(const DecPacked& p);
  
  DecQuad& fromUInt32(uint32_t i)
  { decQuadFromUInt32(&m_data, i); return *this; }

  int32_t getCoefficient(ByteArr_t& bcd) const {
    bcd.resize(DECQUAD_Pmax);
    return decQuadGetCoefficient(&m_data, (uint8_t*)bcd.data());
  }

  DecQuad& setCoefficient(const ByteArr_t& bcd, int32_t sign) {
    decQuadSetCoefficient(&m_data, (const uint8_t*)bcd.data(), sign);
    return *this;
  }
  
  DecQuad& setExponent(int32_t exp, DecContext* c = nullptr) {
    decQuadSetExponent(&m_data, Cxt(c), exp);
    return *this;
  }
  
  int32_t toBCD(int32_t& exp, ByteArr_t& bcd) {
    bcd.resize(DECQUAD_Pmax);
    return decQuadToBCD(&m_data, &exp, (uint8_t*)bcd.data());
  }

  double toDouble() const;

  std::string toString() const {
    char str[MaxStrSize] = { 0 };
    return decQuadToString(&m_data, str);
  }
  
  std::string toEngString() const {
    char str[MaxStrSize] = { 0 };
    return decQuadToEngString(&m_data, str);
  }

  int32_t toPacked(int32_t& exp, ByteArr_t& pack) {
    pack.resize(DECQUAD_Pmax);
    return decQuadToPacked(&m_data, &exp, (uint8_t*)pack.data());
  }
  
  
  DecDouble toDecDouble(DecContext* c = nullptr) const;
  
  DecPacked toDecPacked() const;
  
  DecNumber toDecNumber() const;
  
  DecQuad& zero()
  { decQuadZero(&m_data); return *this; }

 
  // COMPUTATIONAL
  DecQuad abs(DecContext* c = nullptr) const
  { decQuad q; return decQuadAbs(&q, &m_data, Cxt(c)); }

  DecQuad add(const DecQuad& o, DecContext* c = nullptr) const
  { decQuad q; return decQuadAdd(&q, &m_data, &o.m_data, Cxt(c)); }

  DecQuad digitAnd(const DecQuad& o, DecContext* c = nullptr) const
  { decQuad q; return decQuadAnd(&q, &m_data, &o.m_data, Cxt(c)); }

  DecQuad divide(const DecQuad& o, DecContext* c = nullptr) const
  { decQuad q; return decQuadDivide(&q, &m_data, &o.m_data, Cxt(c)); }

  DecQuad divideInteger(const DecQuad& o, DecContext* c = nullptr) const
  { decQuad q; return decQuadDivideInteger(&q, &m_data, &o.m_data, Cxt(c)); }

  DecQuad fma(const DecQuad& o, const DecQuad& o2,
               DecContext* c = nullptr) const
  { decQuad q; return decQuadFMA(&q, &m_data, &o.m_data, &o2.m_data, Cxt(c)); }

  DecQuad invert(DecContext* c = nullptr) const
  { decQuad q; return decQuadInvert(&q, &m_data, Cxt(c)); }

  DecQuad logB(DecContext* c = nullptr) const
  { decQuad q; return decQuadLogB(&q, &m_data, Cxt(c)); }

  DecQuad max(const DecQuad& o, DecContext* c = nullptr) const
  { decQuad q; return decQuadMax(&q, &m_data, &o.m_data, Cxt(c)); }

  DecQuad maxMag(const DecQuad& o, DecContext* c = nullptr) const
  { decQuad q; return decQuadMaxMag(&q, &m_data, &o.m_data, Cxt(c)); }

  DecQuad minus(DecContext* c = nullptr) const
  { decQuad q; return decQuadMinus(&q, &m_data, Cxt(c)); }

  DecQuad multiply(const DecQuad& o, DecContext* c = nullptr) const
  { decQuad q; return decQuadMultiply(&q, &m_data, &o.m_data, Cxt(c)); }

  DecQuad nextMinus(DecContext* c = nullptr) const
  { decQuad q; return decQuadNextMinus(&q, &m_data, Cxt(c)); }

  DecQuad nextPlus(DecContext* c = nullptr) const
  { decQuad q; return decQuadNextPlus(&q, &m_data, Cxt(c)); }

  DecQuad nextToward(const DecQuad& o, DecContext* c = nullptr) const
  { decQuad q; return decQuadNextToward(&q, &m_data, &o.m_data, Cxt(c)); }

  DecQuad digitOr(const DecQuad& o, DecContext* c = nullptr) const
  { decQuad q; return decQuadOr(&q, &m_data, &o.m_data, Cxt(c)); }

  DecQuad plus(DecContext* c = nullptr) const
  { decQuad q; return decQuadPlus(&q, &m_data, Cxt(c)); }

  DecQuad quantize(const DecQuad& o, DecContext* c = nullptr) const
  { decQuad q; return decQuadQuantize(&q, &m_data, &o.m_data, Cxt(c)); }

  DecQuad reduce(DecContext* c = nullptr) const
  { decQuad q; return decQuadReduce(&q, &m_data, Cxt(c)); }
  
  DecQuad remainder(const DecQuad& o, DecContext* c = nullptr) const
  { decQuad q; return decQuadRemainder(&q, &m_data, &o.m_data, Cxt(c)); }

  DecQuad remainderNear(const DecQuad& o, DecContext* c = nullptr) const
  { decQuad q; return decQuadRemainderNear(&q, &m_data, &o.m_data, Cxt(c)); }

  DecQuad rotate(const DecQuad& o, DecContext* c = nullptr) const
  { decQuad q; return decQuadRotate(&q, &m_data, &o.m_data, Cxt(c)); }

  DecQuad scaleB(const DecQuad& o, DecContext* c = nullptr) const
  { decQuad q; return decQuadScaleB(&q, &m_data, &o.m_data, Cxt(c)); }

  DecQuad shift(const DecQuad& o, DecContext* c = nullptr) const
  { decQuad q; return decQuadShift(&q, &m_data, &o.m_data, Cxt(c)); }

  DecQuad subtract(const DecQuad& o, DecContext* c = nullptr) const
  { decQuad q; return decQuadSubtract(&q, &m_data, &o.m_data, Cxt(c)); }

  DecQuad toIntegralValue(enum rounding r, DecContext* c = nullptr) const
  { decQuad q; return decQuadToIntegralValue(&q, &m_data, Cxt(c), r); }

  DecQuad toIntegralExact(DecContext* c = nullptr) const
  { decQuad q; return decQuadToIntegralExact(&q, &m_data, Cxt(c)); }

  DecQuad digitXor(const DecQuad& o, DecContext* c = nullptr) const
  { decQuad q; return decQuadXor(&q, &m_data, &o.m_data, Cxt(c)); }


  // COMPARISONS
  DecQuad compare(const DecQuad& o, DecContext* c = nullptr) const
  { decQuad q; return decQuadCompare(&q, &m_data, &o.m_data, Cxt(c)); }

  DecQuad compareSignal(const DecQuad& o, DecContext* c = nullptr) const
  { decQuad q; return decQuadCompareSignal(&q, &m_data, &o.m_data, Cxt(c)); }

  DecQuad compareTotal(const DecQuad& o) const
  { decQuad q; return decQuadCompareTotal(&q, &m_data, &o.m_data); }

  DecQuad compareTotalMag(const DecQuad& o) const
  { decQuad q; return decQuadCompareTotalMag(&q, &m_data, &o.m_data); }

  
  // COPIES
  DecQuad& canonical(const DecQuad& d)
  { decQuadCanonical(&m_data, &d.m_data); return *this; }

  DecQuad& copy(const DecQuad& d)
  { decQuadCopy(&m_data, &d.m_data); return *this; }

  DecQuad& copyAbs(const DecQuad& d)
  { decQuadCopyAbs(&m_data, &d.m_data); return *this; }

  DecQuad& copyNegate(const DecQuad& d)
  { decQuadCopyNegate(&m_data, &d.m_data); return *this; }

  DecQuad& copySign(const DecQuad& d, const DecQuad& sd)
  { decQuadCopySign(&m_data, &d.m_data, &sd.m_data); return *this; }

  
  // NON-COMPUTATIONAL
  // "class" is a reserved word in C++
  enum decClass classification() const
  { return decQuadClass(&m_data); }

  const char* classString() const
  { return decQuadClassString(&m_data); }

  uint32_t digits() const
  { return decQuadDigits(&m_data); }

  bool isCanonical() const
  { return decQuadIsCanonical(&m_data); }

  bool isFinite() const
  { return decQuadIsFinite(&m_data); }
  
  bool isInteger() const
  { return decQuadIsInteger(&m_data); }

  bool isLogical() const
  { return decQuadIsLogical(&m_data); }

  bool isInfinite() const
  { return decQuadIsInfinite(&m_data); }

  bool isNaN() const
  { return decQuadIsNaN(&m_data); }

  bool isNegative() const
  { return decQuadIsNegative(&m_data); }

  bool isNormal() const
  { return decQuadIsNormal(&m_data); }

  bool isPositive() const
  { return decQuadIsPositive(&m_data); }

  bool isSignaling() const
  { return decQuadIsSignaling(&m_data); }

  bool isSignalling() const
  { return decQuadIsSignalling(&m_data); }

  bool isSigned() const
  { return decQuadIsSigned(&m_data); }

  bool isSubnormal() const
  { return decQuadIsSubnormal(&m_data); }
  
  bool isZero() const
  { return decQuadIsZero(&m_data); }
  
  uint32_t radix() const
  { return decQuadRadix(&m_data); }
  
  const char* version() const
  { return decQuadVersion(); }


  // RELATIONAL AND LOGICAL OPERATORS
  bool operator==(const DecQuad& o) const
  { return compare(o).isZero(); }

  bool operator!=(const DecQuad& o) const
  { return !(this->operator==(o)); }

  bool operator<(const DecQuad& o) const
  { return compare(o).isNegative(); }

  bool operator<=(const DecQuad& o) const
  { 
    const DecQuad& r = compare(o);
    return r.isNegative() || r.isZero(); 
  }

  bool operator>(const DecQuad& o) const
  { return !(this->operator<=(o)); }

  bool operator>=(const DecQuad& o) const
  { 
    const DecQuad& r = compare(o);
    return !r.isNegative() || r.isZero(); 
  }

  // BITWISE OPERATORS
  DecQuad operator&(const DecQuad& o) const
  { return digitAnd(o); }

  DecQuad operator|(const DecQuad& o) const
  { return digitOr(o); }

  DecQuad operator^(const DecQuad& o) const
  { return digitXor(o); }

  
  // ARITHMETIC OPERATORS
  DecQuad operator+(const DecQuad& o) const
  { return add(o); }

  DecQuad operator-(const DecQuad& o) const
  { return subtract(o); }

  DecQuad operator*(const DecQuad& o) const
  { return multiply(o); }

  DecQuad operator/(const DecQuad& o) const
  { return divide(o); }

  DecQuad operator%(const DecQuad& o) const
  { return remainder(o); }


  // COMPOUND ASSIGNMENT OPERATORS
  DecQuad& operator+=(const DecQuad& o)
  { return copy(add(o)); }

  DecQuad& operator-=(const DecQuad& o)
  { return copy(subtract(o)); }

  DecQuad& operator*=(const DecQuad& o)
  { return copy(multiply(o)); }

  DecQuad& operator/=(const DecQuad& o)
  { return copy(divide(o)); }

  DecQuad& operator%=(const DecQuad& o)
  { return copy(remainder(o)); }

  DecQuad& operator&=(const DecQuad& o)
  { return copy(digitAnd(o)); }

  DecQuad& operator|=(const DecQuad& o)
  { return copy(digitOr(o)); }

  DecQuad& operator^=(const DecQuad& o)
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
std::ostream& operator<<(std::ostream& os, const DecQuad& d);



}; // end namespace

#endif /* Include guard */
