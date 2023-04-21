/** \file DecNumber.cc
 * Definitions for the class DecNumber.
 *
 * (C) Copyright by Semih Cemiloglu
 * All rights reserved, see COPYRIGHT file for details.
 *
 * $Id$
 *
 *
 */

#include "DecNumber.hh"
extern "C" {
 #include "decimal32.h"
 #include "decimal64.h"
 #include "decimal128.h"
}
#include "DecSingle.hh"
#include "DecDouble.hh"
#include "DecQuad.hh"
#include "DecPacked.hh"

#include <cstdlib>
#include <iostream>

#include "short_alloc.h"

using namespace std;


namespace dec {

thread_local DecContext DecNumber::s_DecContext;


DecNumber::DecNumber(const DecSingle& s)
{ decSingleToNumber(s.data(), this); } 

DecNumber::DecNumber(const DecDouble& d)
{ decDoubleToNumber(d.data(), this); } 

DecNumber::DecNumber(const DecQuad& q)
{ decQuadToNumber(q.data(), this); } 

DecNumber::DecNumber(const DecPacked& p)
{ *this = p.toDecNumber(); }


DecNumber& DecNumber::fromDouble(double d)
{
  char str[MaxStrSize] = { 0 };

 #if defined(_MSC_VER)
  _snprintf(str, MaxStrSize, "%.*g", DecNumDigits, d);
 #else
  snprintf(str, MaxStrSize, "%.*g", DecNumDigits, d);
 #endif
  return fromString(str);
}


double DecNumber::toDouble() const
{
  char str[MaxStrSize] = { 0 };
  
  decNumberToString(this, str);
  return strtod(str, 0);
}


ostream& operator<<(ostream& os, const DecNumber& n)
{
  return (os << n.toString());
}


}; // end namespace

