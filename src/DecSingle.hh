#ifndef DECSINGLE_HH
#define DECSINGLE_HH

/** \file DecSingle.hh 
 * Declarations for the class DecSingle.
 *
 * (C) Copyright by Semih Cemiloglu
 * All rights reserved, see COPYRIGHT file for details.
 *
 *
 */

#include "DecFwd.hh"
extern "C" {
 #include "decSingle.h"
}
#include "DecContext.hh"
#include "DecPacked.hh"
#include <cstdint>
#include <string>
#include <iosfwd>


namespace dec {

/*!
  \class DecSingle
  DecSingle encapsulates decSingle and provides decNumber
  library functions that operates upon decSingle as member functions
  with the same name.
  decimal32 is a 32-bit decimal floating-point representation which
  provides 7 decimal digits of precision in a compressed format.
  decSingle module provides the functions for the decimal32 format;
  this format is intended for storage and interchange only and so
  the module provides utilities and conversions but no arithmetic functions.
  DecSingle cannot derive from decSingle as it's a union and C++ rules don't 
  allow direct derivation. 
 */
class DecSingle 
{
  // MEMBERS
  //! Embedded decSingle structure
  decSingle m_data;

  // STATIC MEMBERS
  static thread_local DecContext s_DecContext;

 public:
  // TYPES
  typedef decSingle* decSinglePtr_t;
  enum {
    MaxStrSize = DECMAXSTRSIZE
  };

 
  // CREATORS
  //! Default constructor
  DecSingle() { decSingleZero(&m_data); }
  
  // Default Dtor and Copy Ctor are ok

  // Constructors using decSingle structure
  DecSingle(decSingle d) : m_data(d) {}
  DecSingle(const decSingle* p) : m_data(*p) {}
  
  // Conversion constructors using simple types
  DecSingle(const char* str) { fromString(str); }
  DecSingle(double d) { fromDouble(d); }
  
  // Conversion constructors using composite types
  DecSingle(const DecDouble& d) { fromDecDouble(d); }
  DecSingle(const DecPacked& p) { fromDecPacked(p); }
  DecSingle(const DecNumber& n) { fromDecNumber(n); }

  //! Copy assignment
  DecSingle& operator=(const DecSingle& o)
  { if(this != &o) m_data = o.m_data; return *this; }

  //! Conversion operator to decSingle*
  operator decSinglePtr_t() { return &m_data; }

  
  // ACCESSORS
  const decSingle* data() const
  { return &m_data; }

  // MODIFIERS
  decSingle* data()
  { return &m_data; }


  // UTILITIES & CONVERSIONS
  DecSingle& fromBCD(int32_t exp, const ByteArr_t& bcd, int32_t sign) {
    decSingleFromBCD(&m_data, exp, (const uint8_t*)bcd.data(), sign);
    return *this;
  }
  
  DecSingle& fromDouble(double d);

  DecSingle& fromPacked(int32_t exp, const ByteArr_t& pack) {
    decSingleFromPacked(&m_data, exp, (const uint8_t*)pack.data());
    return *this;
  }
  
  DecSingle& fromPackedChecked(int32_t exp, const ByteArr_t& pack) {
    decSingleFromPackedChecked(&m_data, exp, (const uint8_t*)pack.data());
    return *this;
  }

  DecSingle& fromString(const char* str, DecContext* c = nullptr) {
    decSingleFromString(&m_data, str, Cxt(c));
    return *this; 
  }

  DecSingle& fromString(const std::string& str, DecContext* c = nullptr) {
    decSingleFromString(&m_data, str.data(), Cxt(c));
    return *this; 
  }

  //! Hexadecimal string in network byte order
  DecSingle& fromHexString(const char* str);

  DecSingle& fromDecDouble(const DecDouble& d, DecContext* c = nullptr)
  { return fromWider(d,c); }
  
  DecSingle& fromDecNumber(const DecNumber& n, DecContext* c = nullptr);
  
  DecSingle& fromDecPacked(const DecPacked& p);
  
  DecSingle& fromWider(const DecDouble& d, DecContext* c = nullptr);
  
  int32_t getCoefficient(ByteArr_t& bcd) const {
    bcd.resize(DECSINGLE_Pmax);
    return decSingleGetCoefficient(&m_data, (uint8_t*)bcd.data());
  }
 
  int32_t getExponent() const
  { return decSingleGetExponent(&m_data); }

  DecSingle& setCoefficient(const ByteArr_t& bcd, int32_t sign) {
    decSingleSetCoefficient(&m_data, (const uint8_t*)bcd.data(), sign);
    return *this;
  }
  
  DecSingle& setExponent(int32_t exp, DecContext* c = nullptr ) {
    decSingleSetExponent(&m_data, Cxt(c), exp);
    return *this;
  }

  int32_t toBCD(int32_t& exp, ByteArr_t& bcd) {
    bcd.resize(DECSINGLE_Pmax);
    return decSingleToBCD(&m_data, &exp, (uint8_t*)bcd.data());
  }

  double toDouble() const;

  std::string toString() const {
    char str[MaxStrSize] = { 0 };
    return decSingleToString(&m_data, str); 
  }

  std::string toEngString() const {
    char str[MaxStrSize] = { 0 };
    return decSingleToEngString(&m_data, str);
  }
    
  int32_t toPacked(int32_t& exp, ByteArr_t& pack) {
    pack.resize(DECSINGLE_Pmax);
    return decSingleToPacked(&m_data, &exp, (uint8_t*)pack.data());
  }

  DecDouble toDecDouble() const;
  
  DecPacked toDecPacked() const;
  
  DecNumber toDecNumber() const;

  DecDouble toWider() const;
    
  DecSingle& zero()
  { decSingleZero(&m_data); return *this; }


  // ARITHMETIC
  // No arithmetic routines defines for DecSingle

  // NON-COMPUTATIONAL
  
  uint32_t radix() const
  { return decSingleRadix(&m_data); }
  
  const char* version() const
  { return decSingleVersion(); }


  // STATIC FUNCTIONS   
  static DecContext& Context()
  { return s_DecContext; }

  static decContext* Cxt(DecContext* c)
  { return (c ? c->data() : s_DecContext.data() ); }


}; // end class


/*!
  ostream inserter to pretty-print DecNumber contents.
 */
std::ostream& operator<<(std::ostream& os, const DecSingle&);


}; // end namespace

#endif /* Include guard */
