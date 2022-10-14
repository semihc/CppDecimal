/** \file DecPacked.cc
 * Definitions for the class DecPacked.
 *
 * (C) Copyright by Semih Cemiloglu
 * All rights reserved, see COPYRIGHT file for details.
 *
 *
 */

#include "DecPacked.hh"
#include "DecNumber.hh"
#include "DecSingle.hh"
#include "DecDouble.hh"
#include "DecQuad.hh"

using namespace std;


namespace dec {

DecPacked::DecPacked(const char* str)
{ *this = fromDecNumber(DecNumber(str)); }

DecPacked::DecPacked(double d)
{ *this = fromDecNumber(DecNumber(d)); }

DecPacked::DecPacked(const DecSingle& s)
{ *this = s.toDecPacked(); }

DecPacked::DecPacked(const DecDouble& d)
{ *this = d.toDecPacked(); }

DecPacked::DecPacked(const DecQuad& q)
{ *this = q.toDecPacked(); }


DecPacked& DecPacked::fromDecNumber(const DecNumber& d)
{
  uint8_t bfr[DecNumDigits] = { 0 };
  ByteArr_t bfrvec(DecNumDigits, 0);
  int32_t scale = 0;

  uint8_t* rv = decPackedFromNumber(bfrvec.data(), DecNumDigits, &scale, d.data());

  if(rv) {
    m_scale = scale;
    m_bytes = bfrvec;
    /*ERASE
    char* p = (char*)bfr;
    int i = 0;
    // Skip null bytes at the left
    for(; p[i] == '\0' || i==DecNumDigits; ++i);

    // Construct byte array from the beginning of BCD bytes
    m_bytes = ByteArr_t(&p[i], DecNumDigits-i);
    */
  }

  //TODO: move semantics
  return *this;
}

DecNumber DecPacked::toDecNumber() const
{
  if(length() > 0) {
    decNumber n;
    return decPackedToNumber(bytesRaw(), length(), &m_scale, &n);
  }
  else
    // Not initialized, return default DecNumber value
    return DecNumber();
}

DecPacked& DecPacked::fromDouble(double d)
{
  *this = fromDecNumber(DecNumber(d));
  return *this;
}

DecPacked& DecPacked::fromString(const char* str)
{
  *this = fromDecNumber(DecNumber(str));
  return *this;
}

string DecPacked::toString() const
{
  return toDecNumber().toString();
}


}; // end namespace