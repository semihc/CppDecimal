/** \file DecSingle.cc
 * Definitions for the class DecSingle.
 *
 * (C) Copyright by Semih Cemiloglu
 * All rights reserved, see COPYRIGHT file for details.
 *
 *
 */

#include "DecSingle.hh"
extern "C" {
 #include "decimal32.h"
}
#include <cstdint>
#include <cstring>
#include <cassert>
#include <iostream>
#include "DecNumber.hh"
#include "DecPacked.hh"
#include "DecDouble.hh"


using namespace std;


namespace dec {

thread_local DecContext DecSingle::s_DecContext;


DecSingle& DecSingle::fromDouble(double d)
{
  char str[MaxStrSize] = {0};

 #if defined(_MSC_VER)
  _snprintf(str, MaxStrSize, "%.*g", DecSingleDigits, d);
 #else
  snprintf(str, MaxStrSize, "%.*g", DecSingleDigits, d);
 #endif

  return fromString(str);
}

DecSingle& DecSingle::fromHexString(const char* str)
{
  // unsigned long is wider on some platforms, thus
  uint32_t /*unsigned long*/ hv = strtoul(str, nullptr, 16);

  static_assert(sizeof(hv) == sizeof(m_data));  
  memcpy(&m_data, &hv, sizeof(hv));

  //-clog << sizeof(hv) << ' ' << sizeof(m_data) << "hv=" << hv << endl;

  return *this;
}


DecSingle& DecSingle::fromDecNumber(const DecNumber& n, DecContext* c)
{
  decSingleFromNumber(&m_data, n.data(), Cxt(c));
  return *this;
}
  
DecSingle& DecSingle::fromDecPacked(const DecPacked& p)
{
  fromDecNumber(p.toDecNumber());
  return *this;
}
  
DecSingle& DecSingle::fromWider(const DecDouble& d, DecContext* c)
{
  decSingleFromWider(&m_data, d.data(), Cxt(c));
  return *this;
}


double DecSingle::toDouble() const
{
  char str[MaxStrSize] = {0};
  decSingleToString(&m_data, str);
  return strtod(str, 0);
}

DecDouble DecSingle::toDecDouble() const
{
  return toWider();
}
  
DecPacked DecSingle::toDecPacked() const
{
  return DecPacked(toDecNumber());
}
  
DecNumber DecSingle::toDecNumber() const
{
  decNumber n;
  return decSingleToNumber(&m_data, &n);
}

DecDouble DecSingle::toWider() const
{
  decDouble d;
  return decSingleToWider(&m_data, &d);
}

ostream& operator<<(ostream& os, const DecSingle& d)
{
  return (os << d.toString());
}


}; // end namespace