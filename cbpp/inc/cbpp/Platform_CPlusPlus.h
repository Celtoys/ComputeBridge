
#ifndef INCLUDED_PLATFORM_CPLUSPLUS_CUH
#define INCLUDED_PLATFORM_CPLUSPLUS_CUH


//
// Specify device functions local to the kernel
//
#define cmp_device_fn inline


//
// Specify device functions that can be entry points to the kernel
//
#define cmp_kernel_fn inline


//
// Calculate global thread indices
//
#define cmp_thread_idx_x (0)
#define cmp_thread_idx_y (0)
#define cmp_thread_idx_z (0)


//
// Memory space declarations
//
#define cmp_global
#define cmp_constant
#define cmp_shared


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
// Templated types to allow functions with texture parameters to compile.
// This types are empty, meaning the functions can't be called from the host.
//
template <typename> struct Texture3Du { };
template <typename> struct Texture3Dn { };
template <typename> struct Texture2Du { };
template <typename> struct Texture2Dn { };
template <typename> struct Texture1Du { };
template <typename> struct Texture1Dn { };


#endif
