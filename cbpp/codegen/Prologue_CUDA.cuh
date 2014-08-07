
#ifndef INCLUDED_PLATFORM_CUDA_H
#define INCLUDED_PLATFORM_CUDA_H


//
// CUDA doesn't have built-in vector types, meaning that there are no operators built in as a result.
// While this mimics modern-day GPU scalar hardware, it requires that the vector types and their
// operators be defined in code.
//
// This header file is located in the CUDA v6 samples common include directory
#include <helper_math.h>

// The helper libraries don't have double versions of vector math functions or operator overloads
#include "CUDADoubleMath.cuh"


//
// Specify device functions local to the kernel
//
#define cmp_device_fn __device__


//
// Specify device functions that can be entry points to the kernel
//
#define cmp_kernel_fn extern "C" __global__


//
// Calculate global thread indices
//
#define cmp_thread_idx_x (blockIdx.x * blockDim.x + threadIdx.x)
#define cmp_thread_idx_y (blockIdx.y * blockDim.y + threadIdx.y)
#define cmp_thread_idx_z (blockIdx.z * blockDim.z + threadIdx.z)


//
// Memory space declarations
//
#define cmp_global
#define cmp_constant __constant__
#define cmp_shared __shared__


//
// Texture sampling abstractions
//
#define cmp_tex_1d(t, x) tex1D(t, (x))
#define cmp_tex_2d(t, x, y) tex2D(t, (x), (y))
#define cmp_tex_3d(t, x, y, z) tex3D(t, (x), (y), (z))


//
// Surface writing abstractions
//
#define cmp_surf_write_1d(d, s, x) surf1Dwrite(d, s, (x) * sizeof(d))
#define cmp_surf_write_2d(d, s, x, y) surf2Dwrite(d, s, (x) * sizeof(d), (y))
#define cmp_surf_write_3d(d, s, x, y, z) surf3Dwrite(d, s, (x) * sizeof(d), (y), (z))


//
// Vector construction abstractions
//
cmp_device_fn uint2 cmp_make_uint2(uint x, uint y)
{
	return make_uint2(x, y);
}
cmp_device_fn uint3 cmp_make_uint3(uint x, uint y, uint z)
{
	return make_uint3(x, y, z);
}
cmp_device_fn uint4 cmp_make_uint4(uint x, uint y, uint z, uint w)
{
	return make_uint4(x, y, z, w);
}
cmp_device_fn float2 cmp_make_float2(float x, float y)
{
	return make_float2(x, y);
}
cmp_device_fn float3 cmp_make_float3(float x, float y, float z)
{
	return make_float3(x, y, z);
}
cmp_device_fn float4 cmp_make_float4(float x, float y, float z, float w)
{
	return make_float4(x, y, z, w);
}

cmp_device_fn double2 cmp_make_double2(double x, double y)
{
	return make_double2(x, y);
}
cmp_device_fn double3 cmp_make_double3(double x, double y, double z)
{
	return make_double3(x, y, z);
}
cmp_device_fn double4 cmp_make_double4(double x, double y, double z, double w)
{
	return make_double4(x, y, z, w);
}


//
// Vector conversion abstractions
//
cmp_device_fn int3 cmp_float3_to_int3(float3 f)
{
	return make_int3((int)f.x, (int)f.y, (int)f.z);
}
cmp_device_fn float3 cmp_float4_to_float3(float4 f)
{
	return make_float3(f.x, f.y, f.z);
}
cmp_device_fn double3 cmp_float4_to_double3(float4 f)
{
	return make_double3((double)f.x, (double)f.y, (double)f.z);
}

cmp_device_fn float3 cmp_double3_to_float3(double3 f)
{
	return make_float3((float)f.x, (float)f.y, (float)f.z);
}
cmp_device_fn int3 cmp_double3_to_int3(double3 f)
{
	return make_int3((int)f.x, (int)f.y, (int)f.z);
}
cmp_device_fn double3 cmp_double4_to_double3(double4 f)
{
	return make_double3(f.x, f.y, f.z);
}

cmp_device_fn float3 cmp_int3_to_float3(int3 i)
{
	return make_float3((float)i.x, (float)i.y, (float)i.z);
}
cmp_device_fn double3 cmp_int3_to_double3(int3 i)
{
	return make_double3((double)i.x, (double)i.y, (double)i.z);
}


//
// Atomic operations
//
cmp_device_fn uint cmp_atomic_or(uint* p, uint val)
{
	return atomicOr(p, val);
}
cmp_device_fn uint cmp_atomic_min(uint* p, uint val)
{
	return atomicMin(p, val);
}
cmp_device_fn uint cmp_atomic_max(uint* p, uint val)
{
	return atomicMax(p, val);
}


cmp_device_fn float3 cmp_floor(float3 v)
{
	return make_float3(floor(v.x), floor(v.y), floor(v.z));
}


//
// Texture macros injected into CUDA code during cbpp codegen
//
#define cmp_texture_type(type, channels, read, name) typedef texture<type, channels, read> name
#define cmp_kernel_texture_decl(channels, name)
#define cmp_kernel_texture_decl_comma(channels, name)
#define cmp_kernel_texture_global_def(type, global_name) type global_name
#define cmp_kernel_texture_local_def(type, local_name, global_name) type local_name = global_name


//
// Surface macros injected into CUDA code during cbpp codegen
//
#define cmp_surface_type(channels, name) typedef surface<void, channels> name
#define cmp_kernel_surface_decl(channels, name)
#define cmp_kernel_surface_decl_comma(channels, name)
#define cmp_kernel_surface_global_def(type, global_name) type global_name
#define cmp_kernel_surface_local_def(type, local_name, global_name) type local_name = global_name


#endif
