# CppDecimal

## Overview

The CppDecimal is a C++ library which encapsulates, augments, elevates the IBM decNumber library to the C++ language. Even though decNumber is excellent library which provides near complete impelementation of IEEE 754-2019 and ISO/IEC 60559:2020 decimal arithmetic specifications, using it in a way that is compatible C++ standard library is not easy due to fact that it has been implemented in ANSI C.

Hence, the overall design goal of the CppDecimal project is to provide decimal arithmetic types that is compatible with C++ language rules, recent C++ standards, C++/STL standard library containers, algorithms and finally expectations of the C++ community. In summary CppDecimal aims to provide decimal arithmatic types, modules and data interchange formats in way that we as C++ programmers has been accustomed to, similar to what we have been using with (float/double/long double) C++ language types.

```c++
#include <decimal>        // One inclusion is sufficient
using namespace dec;      // All CppDecimal types are in this namespace
                          // Rest is omitted for clarity
{
  decd one{1};            // using decd = DecDouble; 
  decd two = 2.0;         // 16 digits of decimal precision
  decd three = one + two;  

                          // Euler's number
  decn e = 2.71828;       // using decn = DecNumber; (Arbitrary precision decimal number)  
  decp pi{22.0/7.0};      // using decp = DecPacked; (Binary Coded Decimal with arbitrary length)
}
```

In summary CppDecimal library aspires to provide decimal types that can directly replace standard floating point types which are provided by the C++ language (double et al.)

## decNumber Overview
The decNumber library implements the General Decimal Arithmetic Specification[1,2] in ANSI C. This specification defines a decimal arithmetic which meets the requirements of commercial, financial, and human-oriented applications. It also matches the decimal arithmetic in the IEEE 754 Standard for Floating Point Arithmetic.[4] 
The library fully implements the specification, and hence supports integer, fixed-point, and floating-point decimal numbers directly, including infinite, NaN (Not a Number), and subnormal values. Both arbitrary-precision and fixed-size representations are supported.

The arbitrary-precision code is optimized and tunable for common values (tens of digits) but can be used without alteration for up to a billion digits of precision and 9-digit exponents. It also provides functions for conversions between concrete representations of decimal numbers, including Packed BCD (4-bit Binary Coded Decimal) and the three primary IEEE 754 fixed-size formats of decimal floating-point (decimal32, decimal64, and decimal128).

The three fixed-size formats are also supported by three modules called **decFloats**, which have an extensive set of functions that work directly from the formats and provide arithmetical, logical, and shifting operations, together with conversions to binary integers, Packed BCD, and 8-bit BCD. Most of the functions defined in IEEE 754 are included, together with other functions outside the scope of that standard but essential for a decimal-only language implementation.

## Library Structure

The library comprises several modules (corresponding to classes in an object-oriented implementation). Each module has a header file (for example, decNumber.h) which defines its data structure, and a source file of the same name (e.g., decNumber.c) which implements the operations on that data structure. These correspond to the instance variables and methods of an object-oriented design.
The core of the library is the **decNumber** module. This uses an arbitrary-precision decimal number representation designed for efficient computation in software and implements the arithmetic and logical operations, together with a number of conversions and utilities. Once a number is held as a decNumber, no further conversions are necessary to carry out arithmetic.

Most functions in the decNumber module take as an argument a decContext structure, which provides the context for operations (precision, rounding mode, etc.) and also controls the handling of exceptional conditions (corresponding to the flags and trap enablers in a hardware floating-point implementation).

The decNumber representation is variable-length and machine-dependent (for example, it contains integers which may be big-endian or little-endian).

In addition to the arbitrary-precision decNumber format, three fixed-size compact formats are provided for conversions and interchange.[3]  These formats are endian-dependent but otherwise are machine-independent:

**decimal32**
a 32-bit decimal floating-point representation which provides 7 decimal digits of precision in a compressed format
**decimal64**
a 64-bit decimal floating-point representation which provides 16 decimal digits of precision in a compressed format
**decimal128**
a 128-bit decimal floating-point representation which provides 34 decimal digits of precision in a compressed format.
A fourth, machine-independent, Binary Coded Decimal (BCD) format is also provided:

**decPacked**
The decPacked format is the classic packed decimal format implemented by IBM S/360 and later machines, where each digit is encoded as a 4-bit binary sequence (BCD) and a number is ended by a 4-bit sign indicator. The decPacked module accepts variable lengths, allowing for very large numbers (up to a billion digits), and also allows the specification of a scale.
The module for each format provides conversions to and from the core decNumber format. The decimal32, decimal64, and decimal128 modules also provide conversions to and from character string format.
The decimal32, decimal64, and decimal128 formats are also supported directly by three modules which can be used stand-alone (that is, they have no dependency on the decNumber module). These are:

**decSingle**
a module that provides the functions for the decimal32 format; this format is intended for storage and interchange only and so the module provides utilities and conversions but no arithmetic functions
**decDouble**
a module that provides the functions for the decimal64 format; this format is an IEEE 754 basic format and so a full set of arithmetic and other functions is included
**decQuad**
a module that provides the functions for the decimal128 format; this format is an IEEE 754 basic format; it contains the same set of functions as decDouble.[4] 
These modules use the same context mechanism (decContext) as decNumber and so can be used together with the decNumber module when required in order to use the mathematical functions in that module or to use its arbitrary-precision capability.

## Relevant standards
It is intended that, where applicable, functions provided in the decNumber package follow the requirements of:
the decimal arithmetic requirements of IEEE 754 except that:
the IEEE remainder operator (decNumberRemainderNear) is restricted to those values where the intermediate integer can be represented in the current precision, because the conventional implementation of this operator would be very long-running for the range of numbers supported (up to ±101,000,000,000).
the mathematical functions in the decNumber module do not, in general, correspond to the recommended functions in IEEE 754 with the same or similar names; in particular, the power function has some different special cases, and most of the functions may be up to one unit wrong in the last place (note, however, that the squareroot function is correctly rounded)
the floating-point decimal arithmetic defined in ANSI X3.274-1996[4]  (including errata through 2001); note that this applies to functions in the decNumber module only, with appropriate context.

## decFloats modules
The decFloats modules are decSingle, decDouble, and decQuad. These are based on the 32-bit, 64-bit, and 128-bit decimal types in the IEEE 754 Standard for Floating Point Arithmetic.
In contrast to the arbitrary-precision decNumber module, these modules work directly from the decimal-encoded formats designed by the IEEE 754 committee.[1] 

Conversions to and from the decNumber internal format are not needed (typically the numbers are represented internally in ‘unpacked’ BCD or in a base of some other power of ten), and no memory allocation is necessary, so these modules are much faster than using decNumber for implementing the types.

Like the decNumber module, the decFloats modules 
- need only 32-bit integer support; 64-bit integers are not required and binary floating-point is not used
- support both big-endian and little-endian encodings and platforms
- support both ASCII/UTF8 and EBCDIC strings
- are reentrant and use only aligned integers and strict aliasing
- use only ANSI C.

The modules should therefore be usable on any platform with an ANSI C compiler that supports 32-bit integers.
The decFloats modules define the data structures and a large set of functions for working directly with the same compressed formats as decimal32, decimal64, and decimal128. The names are different to allow them to be used stand-alone or with the decNumber module.

These three modules all share many of the same functions (working on the different sizes of the formats). The decQuad module has all the same functions as decDouble except for two functions which would convert to or from a wider format. The decSingle module is a limited (‘storage’) format which has a only a few conversion and miscellaneous functions; it is intended that any computation be carried out in a wider format.

## References

[1] General Decimal Arithmetic Specification
http://speleotrove.com/decimal/decarith.html 

[2] The decNumber Library
http://speleotrove.com/decimal/decnumber.html

[3] General Decimal Arithmetic
http://speleotrove.com/decimal

[4] IEEE 754 Technical Standard
https://en.wikipedia.org/wiki/IEEE_754

