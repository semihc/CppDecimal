* The CppDecimal Library
~~~~~~~~~~~~~~~~~~~~~~~~

Overview
~~~~~~~~

The CppDecimal is a thin layer around IBM's decNumber library which implements the General Decimal Arithmetic Specification in ANSI C. [1]

This specification defines a decimal arithmetic which meets the requirements of commercial, financial, and 
human-oriented applications. It also matches the decimal arithmetic in the IEEE 754 Standard for Floating Point 
Arithmetic.

The decNumber library also matches the decimal arithmetic in the IEEE 754 Standard for Floating Point Arithmetic.

The CppDecimal/decNumber library [2] fully implements the specification, and hence supports integer, fixed-point, and floating-point decimal numbers directly, including infinite, NaN (Not a Number), and subnormal values. Both arbitrary-precision and fixed-size representations are supported.


The aim of the CppDecimal library is to extend decNumber functionality to C++ programing language and to recent C++ standards by using idioms, tecniques and best practices in them. For instance, inline functions are used heavily to aid optimization, operator overloading and conversion operators are defined to aid type casting in between the types defined by CppDecimal. Further these types are integrated with Qt object model by introducing them to Qt meta type system.


Following classes are defined by CppDecimal library:


DecNumber (based on decNumber):

The decNumber module uses an arbitrary-precision decimal number representation designed for efficient computation in software and implements the arithmetic and logical operations, together with a number of conversions and utilities. Once a number is held as a decNumber, no further conversions are necessary to carry out arithmetic. The decNumber representation is variable-length and machine-dependent (for example, it contains integers which may be big-endian or little-endian).

The DecNumber C++ class encapsulates decNumber and reimplements global functions that operates upon decNumber as member functions with the same name.


DecContext (based on decContext):

Most functions in the decNumber module take as an argument a decContext structure, which provides the context for operations (precision, rounding mode, etc.) and also controls the handling of exceptional conditions (corresponding to the flags and trap enablers in a hardware floating-point implementation).

The DecContext C++ class encapsulates decContext for easy access to unerlying context operations.


DecSingle (based on decSingle/decimal32):

The decimal32 type is a 32-bit decimal floating-point representation which provides 7 decimal digits of precision in a compressed format.

The decSingle module provides the functions for the decimal32 format; this format is intended for storage and interchange only and so the module provides utilities and conversions but no arithmetic functions.

The DecSingle C++ class encapsulates decSingle and provides decNumber library functions that operates upon decSingle as member functions with the same name.



DecDouble (based on decDouble/decimal64):

The decimal64 type is a 64-bit decimal floating-point representation which provides 16 decimal digits of precision in a compressed format.

The decDouble module provides the functions for the decimal64 format; this format is an IEEE 754 basic format and so a full set of arithmetic and other functions is included.

The DecDouble C++ class encapsulates decDouble and provides decNumber library functions that operates upon decSingle as member functions with the same name.



DecQuad (based on decQuad/decimal128):

The decimal128 type is a 128-bit decimal floating-point representation which provides 34 decimal digits of precision in a compressed format.

The decQuad module provides the functions for the decimal128 format; this format is an IEEE 754 basic format; it contains the same set of functions as decDouble.

The DecQuad C++ class encapsulates decQuad and provides decNumber library functions that operates upon decSingle as member functions with the same name.



DecPacked (based on decPacked):

The decPacked format is the classic packed decimal format implemented by IBM S/360 and later machines, where each digit is encoded as a 4-bit binary sequence (BCD) and a number is ended by a 4-bit sign indicator. The decPacked module accepts variable lengths, allowing for very large numbers (up to a billion digits), and also allows the specification of a scale.

The DecPacked C++ class augments decPacked by encapsulating reference counted byte array and scale of the decimal point as members variables, thus, freeing up user of this class from memory management and keeping track of scale value.


References
~~~~~~~~~~

[1] General Decimal Arithmetic Specification
http://speleotrove.com/decimal/decarith.html 

[2] The decNumber Library
http://speleotrove.com/decimal/decnumber.html

[3] General Decimal Arithmetic
http://speleotrove.com/decimal

[4] IEEE 754 Technical Standard
https://en.wikipedia.org/wiki/IEEE_754

[5] CppDecimal Project Home
https://github.com/semihc/CppDecimal




License
~~~~~~~
CppDecimal is under the terms of the MIT License.
decNumber is under the terms of ICU v1.8.1
See COPYRIGHT.txt file for terms of the these licenses.


Installation
~~~~~~~~~~~~~~
Read INSTALL.txt to build and install CppDecimal.


Credits
~~~~~~~
We are grateful to Mike Cowlishaw et al. from IBM for making decNumber package available. 
Mr. Cowlishaw has kindly helped while making CppDecimal production ready.


In memoriam
~~~~~~~~~~~
CppDecimal library is dedicated to cherished memory of my late uncle:
  Muharrem Saffet Bozkurt
He is sadly missed.

