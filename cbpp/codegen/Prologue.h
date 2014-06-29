
#ifndef INCLUDED_CBPP_PROLOGUE_H
#define INCLUDED_CBPP_PROLOGUE_H


//
// Figure out which platform we're on
//
#if defined(__OPENCL_VERSION__)
	#define CMP_OPENCL 1
#elif defined(__CUDACC__)
	#define CMP_CUDA 1
#endif


//
// CUDA doesn't have built-in vector types, meaning that there are no operators built in as a result.
// While this mimics modern-day GPU scalar hardware, it requires that the vector types and their
// operators be defined in code.
//
#if CMP_CUDA

	// This header file is located in the CUDA v6 samples common include directory
	#include <helper_math.h>

	// The helper libraries don't have double versions of vector math functions or operator overloads
	#include "CUDADoubleMath.h"

#endif


//
// ComputeBridge macros that get injected into the source code when textures are in use.
//
#if CMP_OPENCL

	#define cmp_texture_type(type, channels, read, name) typedef image##channels##d_t name
	#define cmp_kernel_texture_decl(channels, name) image##channels##d_t name
	#define cmp_kernel_texture_decl_comma(channels, name) cmp_kernel_texture_decl(channels, name),
	#define cmp_kernel_texture_global_def(type, global_name)
	#define cmp_kernel_texture_local_def(type, local_name, global_name)

#else

	#define cmp_texture_type(type, channels, read, name) typedef texture<type, channels, read> name
	#define cmp_kernel_texture_decl(channels, name)
	#define cmp_kernel_texture_decl_comma(channels, name)
	#define cmp_kernel_texture_global_def(type, global_name) type global_name
	#define cmp_kernel_texture_local_def(type, local_name, global_name) type local_name = global_name

#endif


#endif