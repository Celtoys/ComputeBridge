
#ifndef INCLUDED_CBPP_PROLOGUE_H
#define INCLUDED_CBPP_PROLOGUE_H


#include "cbpp/Platform.h"


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