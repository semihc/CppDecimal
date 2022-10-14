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
#include <iostream>
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
