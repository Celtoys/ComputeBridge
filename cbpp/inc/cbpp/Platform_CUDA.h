
#ifndef INCLUDED_PLATFORM_CUDA_H
#define INCLUDED_PLATFORM_CUDA_H


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
