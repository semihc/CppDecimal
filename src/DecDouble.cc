/** \file DecDouble.cc
 * Definitions for the class DecDouble.
 *
 * (C) Copyright by Semih Cemiloglu
 * All rights reserved, see COPYRIGHT file for details.
 *
 *
 */

#include "DecDouble.hh"
extern "C" {
 #include "decimal64.h"
}
#include <cstdlib>
#include <cstring>
#include <cassert>
#include <string>
#include <iostream>
#include "DecNumber.hh"
#include "DecPacked.hh"
#include "DecSingle.hh"
#include "DecQuad.hh"


using namespace std;

namespace dec {

thread_local DecContext DecDouble::s_DecContext;


DecDouble& DecDouble::fromDouble(double d)
{
  char str[MaxStrSize] = { 0 };

 #if defined(_MSC_VER)
  _snprintf(str, MaxStrSize, "%.*g", DecDoubleDigits, d);
 #else
  snprintf(str, MaxStrSize, "%.*g", DecDoubleDigits, d);
 #endif

  return fromString(str);
}

DecDouble& DecDouble::fromHexString(const char* str)
{

  unsigned long long hv = strtoull(str, nullptr, 16);
  static_assert(sizeof(hv) == sizeof(m_data));
  memcpy(&m_data, &hv, sizeof(hv));
  
  //-clog << sizeof(hv) << ' ' << sizeof(m_data) << "hv=" << hv << endl;
  
  return *this;
}

DecDouble& DecDouble::fromDecSingle(const DecSingle& s)
{
  decSingleToWider(s.data(), &m_data);
  return *this;
}

DecDouble& DecDouble::fromDecNumber(const DecNumber& n, DecContext* c)
{
  decDoubleFromNumber(&m_data, n.data(), Cxt(c));
  return *this;
}
  
DecDouble& DecDouble::fromDecPacked(const DecPacked& p)
{
  fromDecNumber(p.toDecNumber());
  return *this;
}

DecDouble& DecDouble::fromWider(const DecQuad& q, DecContext* c)
{
  decDoubleFromWider(&m_data, q.data(), Cxt(c));
  return *this;
}

double DecDouble::toDouble() const
{
  char str[MaxStrSize] = { 0 };
  decDoubleToString(&m_data, str);
  return strtod(str, 0);
}

DecSingle DecDouble::toDecSingle(DecContext* c) const
{
  decSingle s;
  return decSingleFromWider(&s, &m_data, Cxt(c));
}

DecQuad DecDouble::toDecQuad() const
{
  decQuad q;
  return decDoubleToWider(&m_data, &q);
}
  
DecPacked DecDouble::toDecPacked() const
{
  return DecPacked(toDecNumber());
}  
  
DecNumber DecDouble::toDecNumber() const
{
  decNumber n;
  return decDoubleToNumber(&m_data, &n);
}

DecQuad DecDouble::toWider() const
{
  decQuad q;
  return decDoubleToWider(&m_data, &q);
}

ostream& operator<<(ostream& os, const DecDouble& d)
{
  return (os << d.toString());
}


}; // end namespace
