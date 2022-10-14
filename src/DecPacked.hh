#ifndef QDECPACKED_HH
#define QDECPACKED_HH

/** \file DecPacked.hh 
 * Declarations for the class QDecPacked.
 *
 * (C) Copyright by Semih Cemiloglu
 * All rights reserved, see COPYRIGHT file for details.
 *
 */

#include "DecFwd.hh"
extern "C" {
 #include "decPacked.h"
}
#include <cstdint>
#include <string>
#include <vector>



namespace dec {

// TYPES
typedef std::vector<uint8_t> ByteArr_t;


/*!
  \class DecPacked
  DecPacked augments decPacked by encapsulating reference counted byte
  array and scale of the decimal point as members variables, thus, freeing up
  user of this class from memory management and keeping track of scale value.
  The decPacked format is the classic packed decimal format implemented
  by IBM S/360 and later machines, where each digit is encoded as
  a 4-bit binary sequence (BCD) and a number is ended by a 4-bit
  sign indicator. The decPacked module accepts variable lengths,
  allowing for very large numbers (up to a billion digits), and also
  allows the specification of a scale.
 */
class DecPacked
{
  // MEMBERS
  //! Byte array containing BCD sequence
  ByteArr_t m_bytes;
  //! Scale of the decimal number (point)
  int32_t m_scale;

 public:
  // CREATORS
  //! Default constructor
  DecPacked() : m_scale(0) {}
  DecPacked(int32_t length, int32_t scale = 0)
      : m_bytes(length,'\0'), m_scale(scale)  {}
  DecPacked(const ByteArr_t& bytes, int32_t scale = 0)
      : m_bytes(bytes), m_scale(scale) {}

  // Default copy constructor and destructor are ok to use
  
  // Conversion constructors using simple types
  DecPacked(const char* str);
  DecPacked(double d);
  
  // Conversion constructors using composite types
  DecPacked(const DecNumber& d)  { fromDecNumber(d); }
  DecPacked(const DecSingle& s);
  DecPacked(const DecDouble& d);
  DecPacked(const DecQuad& d);

  // ACCESSORS
  const char* data() const
  { return reinterpret_cast<const char*>(m_bytes.data()); }

  ByteArr_t bytes() const
  { return m_bytes; }

  const uint8_t* bytesRaw() const
  { return reinterpret_cast<const uint8_t*>(m_bytes.data()); }

  int32_t length() const
  { return m_bytes.size(); }
    
  int32_t scale() const
  { return m_scale; }

  std::string toString() const;

  
  // MODIFIERS
  uint8_t* bytesRaw()
  { return reinterpret_cast<uint8_t*>(m_bytes.data()); }

  DecPacked& fromDouble(double d);

  DecPacked& fromString(const char* str);

  void setLength(int32_t length)
  { m_bytes.resize(length); }
  
  void setScale(int32_t scale)
  { m_scale = scale; }

  // CONVERSIONS
  DecNumber toDecNumber() const;
  DecPacked& fromDecNumber(const DecNumber& d);


}; // end class

//TODO:
//iostream inserter

}; // end namespace


#endif /* Include guard */
