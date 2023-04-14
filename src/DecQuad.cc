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
#include <sstream>
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
  /*TODO
  QByteArray ba = QByteArray::fromHex(str);
  int size = sizeof(m_data);
  char* p = (char*)&m_data;
  int i = 0;
  int j = size-1;
  for(; i<size; i++,j--)
    p[j] = ba.at(i);
  */
  
  // https://stackoverflow.com/questions/3221170/how-to-turn-a-hex-string-into-an-unsigned-char-array
  clog << "DecQuad fromHexString " << str << endl;

  /*
  std::istringstream hexstm(string{str});
  char* a = reinterpret_cast<char*>(&m_data);
  int i = 0;
  unsigned c;
  int dlen = sizeof(m_data);

  while (hexstm >> std::hex >> c) {
    a[i++] = c;
    clog << hex << "a[" << i << "]=" << c << endl;
    if(i>dlen) break;
  }
  */

  
  int slen = strlen(str);
  char* s = const_cast<char*>(str);
  constexpr int dlen = sizeof(m_data); 
  constexpr int half = dlen/2;
  static_assert(dlen == 16); // Size of DecQuad
  static_assert(sizeof(long long)*2 == dlen);
  char* a = reinterpret_cast<char*>(&m_data);
  //-memset(a, 0, dlen);

  char s1[dlen+1], s2[dlen+1];
  s1[0] = s2[0] = '\0';
  strncpy(s1, str, dlen);
  strncpy(s2, str+dlen, dlen); // Because string could be x2 times of half

  unsigned long long h1, h2;
  h1 = h2 = 0ULL;
  h1 = strtoull(s1, nullptr, 16);
  h2 = strtoull(s2, nullptr, 16);

  clog << std::hex << "h1=" << h1 << " h2=" << h2 << std::dec << endl;

  /*ERASE
  h1 = strtoull(str, &s, 16);
  h2 = strtoull(s, nullptr, 16);

  clog << std::hex << "h1=" << h1 << " h2=" << h2 << std::dec << endl;
  */

  //TBD: Little and big endian analysis needs to be done:
  memcpy(a, &h2, half);
  memcpy(&a[half], &h1, half);


  /*
  for (int i=0; i<slen && isxdigit(*s); i++) {
    a[i] = static_cast<char>(strtol(s, &s, 16));
    clog << hex << "a[" << i << "]=" << int(a[i]) << endl;
  }
  */

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
