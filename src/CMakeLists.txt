add_library(decimal  
 STATIC
  decContext.c 
  decimal32.c
  decimal64.c
  decimal128.c  
  decNumber.c 
  decPacked.c 
  decSingle.c 
  decDouble.c
  decQuad.c  
  DecContext.cc
  DecNumber.cc
  DecSingle.cc
  DecDouble.cc
  DecQuad.cc
  DecPacked.cc
)

#target_include_directories(decnumber 
# INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}
#)
target_include_directories(decimal PUBLIC .)
