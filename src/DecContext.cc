/** \file DecContext.cc
 * Definitions for the class QDecContext.
 *
 * (C) Copyright by Semih Cemiloglu
 * All rights reserved, see COPYRIGHT file for details.
 *
 *
 */

#include "DecContext.hh"
#include <sstream>



using namespace std;


namespace dec {

DecContext::DecContext(int32_t kind)
{
  switch(kind) {
  case DEC_INIT_BASE:
  case DEC_INIT_DECIMAL32:
  case DEC_INIT_DECIMAL64:
  case DEC_INIT_DECIMAL128:
    // Above kinds must be specified
    break;

  default:
    // Invalid kind
    throw logic_error("Invalid DecContext kind");
  }

  decContextDefault(this, kind);

  // No SIGFPE trap is allowed by default
  // as this will disrupt most calculations.
  this->traps = 0;

  // By default allow maximum allowable precision
  setDigits(DecNumDigits);
}


string DecContext::statusFlags() const
{
  uint32_t status = this->status;
  ostringstream os;
  const char sep = '|'; // Separator

  if(status & DEC_Conversion_syntax) 
    os << DEC_Condition_CS << sep;
  if(status & DEC_Division_by_zero)
    os << DEC_Condition_DZ << sep;
  if(status & DEC_Division_impossible)
    os << DEC_Condition_DI << sep;
  if(status & DEC_Division_undefined)
    os << DEC_Condition_DU << sep;
  if(status & DEC_Inexact)
    os << DEC_Condition_IE << sep;
  if(status & DEC_Invalid_context)
    os << DEC_Condition_IC << sep;
  if(status & DEC_Insufficient_storage)
    os << DEC_Condition_IS << sep;
  if(status & DEC_Invalid_operation)
    os << DEC_Condition_IO << sep;
#if DECSUBSET  
  if(status & DEC_Lost_digits)
    os << DEC_Condition_LD << sep;
#endif  
  if(status & DEC_Overflow)
    os << DEC_Condition_OV << sep;
  if(status & DEC_Clamped)
    os << DEC_Condition_PA << sep;
  if(status & DEC_Rounded)
    os << DEC_Condition_RO << sep;
  if(status & DEC_Subnormal)
    os << DEC_Condition_SU << sep;
  if(status & DEC_Underflow)
    os << DEC_Condition_UN << sep;
  if(0==status)
    os << DEC_Condition_ZE << sep;
  
  os << "0x" << ios::hex << status;

  os.flush(); //??
  return os.str();
}


uint8_t DecContext::getExtended() const
{
#if DECSUBSET
  return extended;
#else
  return 0;
#endif
}


void DecContext::setExtended(uint8_t ext)
{
#if DECSUBSET
  extended = ext;
#else
  (void)ext; // To disable compiler warning
#endif
}


ostream& operator<<(ostream& os, const DecContext& ctx)
{
  char c = ' ';
  os << "digits=" << ctx.digits
     << c << "emax=" << ctx.emax
     << c << "emin=" << ctx.emin
     << c << "extended=" << ctx.extended
     << c << "clamp=" << ctx.clamp
     << c << "round=" << ctx.round
     << c << "traps=" << ctx.traps
     << c << "status=" << ctx.status
     << c << ctx.statusToString();
  
  return os;
}


}; // namespace