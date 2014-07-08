
#ifndef INCLUDED_PLATFORM_OPENCL_CUH
#define INCLUDED_PLATFORM_OPENCL_CUH

//
// Toggle double support
//
#pragma OPENCL EXTENSION cl_khr_fp64: enable


//
// Toggle 3D image writes
//
// OpenCL does not by default support 3D image writes and requires this extension
// This extension is not implemented on nVidia drivers and gives the warning:
//
//    warning: unknown '#pragma OPENCL EXTENSION' - ignored
//
// http://www.khronos.org/registry/cl/sdk/1.2/docs/man/xhtml/cl_khr_3d_image_writes.html
// http://www.khronos.org/registry/cl/sdk/1.2/docs/man/xhtml/write_image3d.html
//
// NOTE: This warning won't show up with oclpc unless there is an error!
// 
#pragma OPENCL EXTENSION cl_khr_3d_image_writes: enable


//
// Specify device functions local to the kernel
//
#define cmp_device_fn


//
// Specify device functions that can be entry points to the kernel
//
#define cmp_kernel_fn kernel


//
// Calculate global thread indices
//
#define cmp_thread_idx_x (get_global_id(0))
#define cmp_thread_idx_y (get_global_id(1))
#define cmp_thread_idx_z (get_global_id(2))


//
// Memory space declarations
//
#define cmp_global __global
#define cmp_constant __constant
#define cmp_shared __local


//
// Texture sampling abstractions
//
#define cmp_tex_1d(t, x) 0
#define cmp_tex_2d(t, x, y) 0
#define cmp_tex_3d(t, x, y, z) 0


//
// Surface writing abstractions
//
#define cmp_surf_write_1d(d, s, x)
#define cmp_surf_write_2d(d, s, x, y)
#define cmp_surf_write_3d(d, s, x, y, z)


//
// Vector construction abstractions
//
cmp_device_fn float2 cmp_make_float2(float x, float y)
{
	return (float2){ x, y };
}
cmp_device_fn float3 cmp_make_float3(float x, float y, float z)
{
	return (float3){ x, y, z };
}
cmp_device_fn float4 cmp_make_float4(float x, float y, float z, float w)
{
	return (float4){ x, y, z, w };
}

cmp_device_fn double2 cmp_make_double2(double x, double y)
{
	return (double2){ x, y };
}
cmp_device_fn double3 cmp_make_double3(double x, double y, double z)
{
	return (double3){ x, y, z };
}
cmp_device_fn double4 cmp_make_double4(double x, double y, double z, double w)
{
	return (double4){ x, y, z, w };
}


//
// Vector conversion abstractions
//
cmp_device_fn int3 cmp_float3_to_int3(float3 f)
{
	return convert_int3(f);
}
cmp_device_fn float3 cmp_float4_to_float3(float4 f)
{
	return f.xyz;
}
cmp_device_fn double3 cmp_float4_to_double3(float4 f)
{
	return convert_double3(f.xyz);
}
cmp_device_fn double3 cmp_double4_to_double3(double4 f)
{
	return f.xyz;
}

cmp_device_fn float3 cmp_double3_to_float3(double3 f)
{
	return convert_float3(f);
}
cmp_device_fn int3 cmp_double3_to_int3(double3 f)
{
	return convert_int3(f);
}

cmp_device_fn float3 cmp_int3_to_float3(int3 i)
{
	return convert_float3(i);
}
cmp_device_fn double3 cmp_int3_to_double3(int3 i)
{
	return convert_double3(i);
}


//
// Atomic operations
//
cmp_device_fn uint cmp_atomic_or(cmp_global uint* p, uint val)
{
	return atomic_or(p, val);
}


cmp_device_fn float3 cmp_floor(float3 v)
{
	return floor(v);
}


//
// Texture macros injected into OpenCL code during cbpp codegen
//
#define cmp_texture_type(type, channels, read, name) typedef image##channels##d_t name
#define cmp_kernel_texture_decl(channels, name) image##channels##d_t name
#define cmp_kernel_texture_decl_comma(channels, name) cmp_kernel_texture_decl(channels, name),
#define cmp_kernel_texture_global_def(type, global_name)
#define cmp_kernel_texture_local_def(type, local_name, global_name)


//
// Surface macros injected into OpenCL code during cbpp codegen
//
#define cmp_surface_type(channels, name) typedef image##channels##d_t name
#define cmp_kernel_surface_decl(channels, name) image##channels##d_t name
#define cmp_kernel_surface_decl_comma(channels, name) cmp_kernel_surface_decl(channels, name),
#define cmp_kernel_surface_global_def(type, global_name)
#define cmp_kernel_surface_local_def(type, local_name, global_name)


#endif
