
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
