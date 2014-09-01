
#ifndef INCLUDED_CBPP_PROLOGUE_H
#define INCLUDED_CBPP_PROLOGUE_H


// This header should only be included from the prologue for: OpenCL, CUDA and HLSL.

// Some shader/compute languages have vectors, math operations and other standard library functions
// defined by default without having to include extra header files.
// Within these languages, some have only half of those defined.
// The prologue header file is included automatically in any generated shader/compute code and should
// not be included manually anywhere.
// HOWEVER... C++ code is not generated and so must have these header files included manually.
// This macro allows any files to included ComputeBridge header files without worrying how they are
// being used.
// TODO: This wouldn't be required if ComputeBridge generated C++ files.
#define CMP_WITHIN_PROLOGUE

#include "cbpp/Platform.h"
#include "cbpp/Math.h"
#include "cbpp/Atomics.h"

#undef CMP_WITHIN_PROLOGUE


#endif