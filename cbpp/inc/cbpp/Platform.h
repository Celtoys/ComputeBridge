
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
#else
	// Don't know of any positive identification of HLSL compiler
	#define CMP_HLSL 1
#endif


#endif	// INCLUDED_CBPP_PLATFORM_H
