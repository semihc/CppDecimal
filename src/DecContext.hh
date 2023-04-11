#ifndef DECCONTEXT_HH
#define DECCONTEXT_HH

/** \file DecContext.hh 
 * Declarations for the class DecContext.
 *
 * (C) Copyright Semih Cemiloglu
 * All rights reserved, see COPYRIGHT file for details.
 *
 *
 */

#include "DecFwd.hh"
extern "C" {
 #include "decContext.h"
}
#include <cstdint>
#include <string>
#include <iosfwd>
#include <stdexcept>




namespace dec {

/*! Default context type or kind, should be set to one of these:
 * DEC_INIT_BASE
 * DEC_INIT_DECIMAL32
 * DEC_INIT_DECIMAL64
 * DEC_INIT_DECIMAL128
 */
constexpr int DecContextDefKind = DEC_INIT_BASE;


//! Maximum precision allowed in precision (digits) field
constexpr int32_t DecMaxPrecision = DEC_MAX_DIGITS;
constexpr int32_t DecMaxExponent  = DEC_MAX_EMAX;
constexpr int32_t DecMinExponent  = DEC_MIN_EMIN;

/*!
  \class DecContext
   DecContext encapsulates decContext structure as member and
   exposes free-standing functions as member functions.
   Most functions in the decNumber module take as an argument a
   decContext structure, which provides the context for operations
   (precision, rounding mode, etc.) and also controls the handling of
   exceptional conditions (corresponding to the flags and trap enablers
   in a hardware floating-point implementation).
 */
class DecContext : public decContext
{
 public:
  // TYPES
  typedef decContext* decContextPtr_t;
  typedef enum rounding Rounding_e;

  // CREATORS
  //! Default constructor
  DecContext(int32_t kind = DecContextDefKind);
  DecContext(const decContext* cptr) : decContext(*cptr) {}
  DecContext(const decContext& data) : decContext(data) {}
  // Default Copy Ctor and Dtor and Copy assignment are ok
  

  // ACCESSORS
  //! Get decContext member
  const decContext* data() const 
  { return this; }

  //! Get clamp flag of the context (IEEE exponent clamp)
  uint8_t getClamp() const
  { return clamp; }

  //! Get digits field of the context (working precision)
  int32_t getDigits() const
  { return digits; }

  //! Get emax field of the context (maximum positive exponent)
  int32_t getEmax() const
  { return emax; }

  //! Get emin field of the context (minimum negative exponent)
  int32_t getEmin() const
  { return emin; }

  //! Get extended flag of the context (special values)
  uint8_t getExtended() const;

  //! Get round field of the context (rounding mode)
  Rounding_e getRound() const
  { return round; }

  //! Get status flags of the context
  uint32_t getStatus() const
  { return status; }

  //! Get trap-enabler flags of the context
  uint32_t getTraps() const
  { return traps; }


  // MODIFIERS
  //! Get decContext member
  decContext* data() 
  { return this; }

  //! Set clamp flag of the context (IEEE exponent clamp)
  void setClamp(uint32_t clampp)
  { clamp = clampp; }

  //! Set digits field of the context (working precision)
  void setDigits(int32_t digitsp)
  { digits = digitsp; }

  //! Set emax field of the context (maximum positive exponent)
  void setEmax(int32_t emaxp)
  { emax = emaxp; }

  //! Set emin field of the context (minimum negative exponent)
  void setEmin(int32_t eminp)
  { emin = eminp; }

  //! Set extended flag of the context (special values)
  void setExtended(uint8_t ext);

  //! This function is used to return the round (rounding mode) field
  //! of a decContext.
  void setRound(Rounding_e roundp)
  { round = roundp; }

  void setTraps(uint32_t trapsp)
  { traps = trapsp; }

  /*!
    This function is used to set one or more status bits in the status
    field of a decContext. If any of the bits being set have the
    corresponding bit set in the traps field, a trap is raised
    (regardless of whether the bit is already set in the status field).
    Only one trap is raised even if more than one bit is being set.
  */
  void setStatus(uint32_t statusp = 0)
  { status = statusp; }

  //! This function is identical to setStatus except that
  //! the context traps field is ignored (i.e., no trap is raised).
  void setStatusQuiet(uint32_t status = 0)
  { decContextSetStatusQuiet(this, status); }
  
  // ROUTINES
  //! This function is used to clear (set to zero) one or more status
  //! bits in the status field of a decContext.
  DecContext& clearStatus(uint32_t status)
  { decContextClearStatus(this, status); return *this; }

  //! This function is used to restore one or more status bits in
  //! the status field of a decContext from a saved status field.
  DecContext& restoreStatus(uint32_t status, uint32_t mask)
  { decContextRestoreStatus(this, status, mask); return *this; }

  /*!
    This function is used to initialize a decContext structure to
    default values. It is stongly recommended that this function always
    be used to initialize a decContext structure, even if most or all
    of the fields are to be set explicitly (in case new fields are added
    to a later version of the structure).
  */
  DecContext& setDefault(int32_t kind = DecContextDefKind)
  { decContextDefault(this, kind); return *this; }

  //! This function is used to save one or more status bits from
  //! the status field of a decContext.
  uint32_t saveStatus(uint32_t mask) 
  { return decContextSaveStatus(this, mask); }

  /*!
    This function is used to set a status bit in the status field of a
    decContext, using the name of the bit as returned by the
    decContextStatusToString function. If the bit being set has the
    corresponding bit set in the traps field, a trap is raised
    (regardless of whether the bit is already set in the status field).
  */
  DecContext& setStatusFromString(const char* str) 
  { decContextSetStatusFromString(this, str); return *this; }

  //! This function is identical to setStatusFromString except
  //! that the context traps field is ignored (i.e., no trap is raised).
  DecContext& setStatusFromStringQuiet(const char* str) 
  { decContextSetStatusFromStringQuiet(this, str); return *this; }

  //! This function returns a pointer (char *) to a human-readable
  //! description of a status bit. The string pointed to will be a constant.
  const char* statusToString() const
  { return decContextStatusToString(this); }

  //! This function is used to test one or more status bits in a context.
  uint32_t testStatus(uint32_t mask) //const
  { return decContextTestStatus(this, mask); }

  //! This function is used to clear (set to zero) all the
  //! status bits in the status field of a decContext.
  DecContext& zeroStatus()
  { decContextZeroStatus(this); return *this; }

  
  // UTILITY ROUTINES
  //! Get status flags (fields) in string form
  //! separated by | character
  std::string statusFlags() const;

  //! Throw exception if status flags are set.
  void throwOnError() const
  { if(status) throw std::runtime_error(statusToString()); }

  //! Type conversion operator to decContext*
  operator decContextPtr_t() { return this; }


  // STATIC FUNCTIONS
  //! This function checks that the DECLITEND tuning
  //! parameter is set correctly.
  //! Returns 0 if the DECLITEND parameter is correct,
  //! 1 if it is incorrect and should be set to 1, and
  //! -1 if it is incorrect and should be set to 0.
  static int TestEndian()
  { return decContextTestEndian(1 /*Quiet*/); }

  //! This function is used to test one or more status
  //! bits in a saved status field.
  static uint32_t TestSavedStatus(uint32_t status, uint32_t mask)
  { return decContextTestSavedStatus(status, mask); }
  

}; // end class

//! Utility function to create a tempory DecContext variable on stack if necessary.
//! To satisfy decNumber functions which expects a DecContext pointer argument
inline decContext* GetCxt(DecContext* c = nullptr)
{ return (c ? c->data() : DecContext().data() ); }

/*!
  ostream inserter to pretty-print DecContext contents.
 */
std::ostream& operator<<(std::ostream& os, const DecContext&);



}; // end namespace

#endif /* Include guard */
