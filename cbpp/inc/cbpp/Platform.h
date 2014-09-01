
#ifndef INCLUDED_CBPP_PLATFORM_H
#define INCLUDED_CBPP_PLATFORM_H


// Figure out which platform we're on
// TODO: Change these to CBPP_
#if defined(__OPENCL_VERSION__)
	#define CMP_OPENCL 1
#elif defined(__CUDACC__)
	#define CMP_CUDA 1
#elif defined(__cplusplus)
	#define CMP_CPP 1
#elif defined(D3DX_VERSION)
	#define CMP_HLSL 1
#endif


// Prologue check
#if defined(CMP_CPP) || defined(CMP_HLSL) || defined(CMP_WITHIN_PROLOGUE)


//
// Include the necessary platform header
//
#if defined(CMP_OPENCL)
	#include "Platform_OpenCL.h"
#elif defined(CMP_CUDA)
	#include "Platform_CUDA.h"
#elif defined(CMP_CPP)
	#include "Platform_CPlusPlus.h"
#endif


// Temporary location until HLSL gets its own file
#ifdef CMP_HLSL
	#define cmp_out out
	#define cmp_device_fn
#else
	#define cmp_out
#endif


#endif	// Prologue check


#endif	// INCLUDED_CBPP_PLATFORM_H
