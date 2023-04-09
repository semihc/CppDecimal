#ifndef DECFWD_HH
#define DECFWD_HH
#pragma once

/** \file DecFwd.hh 
 * Forward declarations for Decimal types
 *
 * (C) Copyright by Semih Cemiloglu
 * All rights reserved, see COPYRIGHT file for details.
 *
 *
 */

#ifndef DECNUMDIGITS
//! Work with up to 80 digits as default, resulting in 64 bytes
//! decNumber structure.
# define DECNUMDIGITS 80
#endif

#ifndef DECMAXSTRSIZE
//! Maximum length of a conversion string, default if not specified
# define DECMAXSTRSIZE 512
#endif

//! Extended flags are used by default
#ifndef DECEXTFLAG
# define DECEXTFLAG 1
#endif

//! DecSubset is used by default
#ifndef DECSUBSET
# define DECSUBSET 1
#endif


namespace dec {
	
//! Digits of decimal precision for DecNumber, decNumber.
//! This is set at compile time via DECNUMDIGITS macro.
constexpr int DecNumDigits = DECNUMDIGITS;

//! Maximum length of a conversion string, default if not specified
constexpr int DecMaxStrSize = DECMAXSTRSIZE;

//! Digits of decimal precision for DecSingle, decSingle, decimal32
constexpr int DecSingleDigits = 7;

//! Digits of decimal precision for DecDouble, decDouble, decimal64
constexpr int DecDoubleDigits = 16;

//! Digits of decimal precision for DecQuad, decQuad, decimal128
constexpr int DecQuadDigits = 34;


// Forward declarations of the classes inroduced by this namespace
class DecContext;
class DecNumber;
class DecPacked;
class DecSingle;
class DecDouble;
class DecQuad;

}; // namespace

#endif /* Include guard */
