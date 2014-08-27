
#ifndef INCLUDED_CBPP_PROLOGUE_H
#define INCLUDED_CBPP_PROLOGUE_H


//
// Figure out which platform we're on
//
#if defined(__OPENCL_VERSION__)
	#define CMP_OPENCL 1
#elif defined(__CUDACC__)
	#define CMP_CUDA 1
#elif defined(__cplusplus)
	#define CMP_CPP 1
#endif


//
// Include the necessary platform header
//

#if defined(CMP_OPENCL)
	#include "Prologue_OpenCL.cuh"
#elif defined(CMP_CUDA)
	#include "Prologue_CUDA.cuh"
#elif defined(CMP_CPP)
	#include "Prologue_CPlusPlus.cuh"
#endif


#endif