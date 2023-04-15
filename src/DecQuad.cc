/** \file DecQuad.cc
 * Definitions for the class DecQuad.
 *
 * (C) Copyright by Semih Cemiloglu
 * All rights reserved, see COPYRIGHT file for details.
 *
 *
 */

#include "DecQuad.hh"
extern "C" {
 #include "decimal128.h"
}
#include <cstdlib>
#include <cstring>
#include <cassert>
#include <iostream>
#include <bit>
#include "DecNumber.hh"
#include "DecPacked.hh"
#include "DecSingle.hh"
#include "DecDouble.hh"


using namespace std;


namespace dec {

thread_local DecContext DecQuad::s_DecContext;


DecQuad& DecQuad::fromDouble(double d)
{
  char str[MaxStrSize] = { 0 };

 #if defined(_MSC_VER)
  _snprintf(str, MaxStrSize, "%.*g", DecQuadDigits, d);
 #else
  snprintf(str, MaxStrSize, "%.*g", DecQuadDigits, d);
 #endif
  
  return fromString(str);
}


DecQuad& DecQuad::fromHexString(const char* str)
{
  constexpr int dlen = sizeof(m_data); 
  constexpr int half = dlen/2;
  static_assert(dlen == 16); // Size of DecQuad
  static_assert(sizeof(long long)*2 == dlen);
 
  int slen = strlen(str);
  char* s = const_cast<char*>(str);
  char* a = reinterpret_cast<char*>(&m_data);

  char s1[dlen+1], s2[dlen+1];
  s1[0] = s2[0] = '\0';
  strncpy(s1, str, dlen);
  strncpy(s2, str+dlen, dlen); // Because string could be x2 times of half

  unsigned long long h1, h2;
  h1 = h2 = 0ULL;
  h1 = strtoull(s1, nullptr, 16);
  h2 = strtoull(s2, nullptr, 16);

  // Little and big endian analysis needs to be done:
  // https://stackoverflow.com/questions/1001307/detecting-endianness-programmatically-in-a-c-program
  constexpr bool littleEndian = (std::endian::native == std::endian::little);
  if( littleEndian ) {
    // Little endian.
    memcpy(a, &h2, half);
    memcpy(&a[half], &h1, half);
  } else {
    // Big endian
    memcpy(a, &h1, half);
    memcpy(&a[half], &h2, half);
  }

  //-clog << std::hex << "h1=" << h1 << " h2=" << h2 << std::dec << endl;

  return *this;
}


DecQuad& DecQuad::fromDecDouble(const DecDouble& d)
{
  decDoubleToWider(d.data(), &m_data);
  return *this;
}

DecQuad& DecQuad::fromDecNumber(const DecNumber& n, DecContext* c)
{
  decQuadFromNumber(&m_data, n.data(), Cxt(c));
  return *this;
}
  
DecQuad& DecQuad::fromDecPacked(const DecPacked& p)
{
  fromDecNumber(p.toDecNumber());
  return *this;
}

double DecQuad::toDouble() const
{
  char str[MaxStrSize] = { 0 };
  decQuadToString(&m_data, str);
  return strtod(str, 0);
}

DecDouble DecQuad::toDecDouble(DecContext* c) const
{
  decDouble d;
  return decDoubleFromWider(&d, &m_data, Cxt(c));
}
  
DecPacked DecQuad::toDecPacked() const
{
  return DecPacked(toDecNumber());
}  
  
DecNumber DecQuad::toDecNumber() const
{
  decNumber n;
  return decQuadToNumber(&m_data, &n);
}


ostream& operator<<(ostream& os, const DecQuad& d)
{
  return (os << d.toString());
}


}; // end namespace
