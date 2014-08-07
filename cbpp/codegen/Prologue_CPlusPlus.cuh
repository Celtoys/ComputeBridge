
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


namespace cmp {


//
// Basic type aliases
//
typedef unsigned char uchar;
typedef unsigned short ushort;
typedef unsigned int uint;
typedef unsigned long ulong;


//
// Implementation of vector kernel types
// Use macros instead of templates so that the types can be reflected
//
#define tuple2(type, name)							\
	struct clcpp_attr(reflect_part) name			\
	{												\
		name() : x(0), y(0) { }						\
		name(type x, type y) : x(x), y(y) { }		\
		type x, y;									\
	};
#define tuple3(type, name)										\
	struct clcpp_attr(reflect_part) name						\
	{															\
		name() : x(0), y(0), z(0) { }							\
		name(type x, type y, type z) : x(x), y(y), z(z) { }		\
		type x, y, z;											\
	};
#define tuple4(type, name)														\
	struct clcpp_attr(reflect_part) name										\
	{																			\
		name() : x(0), y(0), z(0), w(0) { }										\
		name(type x, type y, type z, type w) : x(x), y(y), z(z), w(w) { }		\
		type x, y, z, w;														\
	};

tuple2(float, float2);
tuple3(float, float3);
tuple4(float, float4);

tuple2(double, double2);
tuple3(double, double3);
tuple4(double, double4);

tuple2(char, char2);
tuple3(char, char3);
tuple4(char, char4);

tuple2(uchar, uchar2);
tuple3(uchar, uchar3);
tuple4(uchar, uchar4);

tuple2(short, short2);
tuple3(short, short3);
tuple4(short, short4);

tuple2(ushort, ushort2);
tuple3(ushort, ushort3);
tuple4(ushort, ushort4);

tuple2(int, int2);
tuple3(int, int3);
tuple4(int, int4);

tuple2(uint, uint2);
tuple3(uint, uint3);
tuple4(uint, uint4);

tuple2(long, long2);
tuple3(long, long3);
tuple4(long, long4);

tuple2(ulong, ulong2);
tuple3(ulong, ulong3);
tuple4(ulong, ulong4);


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


//
// Vector construction abstractions
//
cmp_device_fn uint2 cmp_make_uint2(uint x, uint y)
{
	return uint2(x, y);
}
cmp_device_fn uint3 cmp_make_uint3(uint x, uint y, uint z)
{
	return uint3(x, y, z);
}
cmp_device_fn uint4 cmp_make_uint4(uint x, uint y, uint z, uint w)
{
	return uint4(x, y, z, w);
}
cmp_device_fn float2 cmp_make_float2(float x, float y)
{
	return float2(x, y);
}
cmp_device_fn float3 cmp_make_float3(float x, float y, float z)
{
	return float3(x, y, z);
}
cmp_device_fn float4 cmp_make_float4(float x, float y, float z, float w)
{
	return float4(x, y, z, w);
}

cmp_device_fn double2 cmp_make_double2(double x, double y)
{
	return double2(x, y);
}
cmp_device_fn double3 cmp_make_double3(double x, double y, double z)
{
	return double3(x, y, z);
}
cmp_device_fn double4 cmp_make_double4(double x, double y, double z, double w)
{
	return double4(x, y, z, w);
}


//
// Vector conversion abstractions
//
cmp_device_fn int3 cmp_float3_to_int3(float3 f)
{
	return int3((int)f.x, (int)f.y, (int)f.z);
}
cmp_device_fn float3 cmp_float4_to_float3(float4 f)
{
	return float3(f.x, f.y, f.z);
}
cmp_device_fn double3 cmp_float4_to_double3(float4 f)
{
	return double3(double(f.x), double(f.y), double(f.z));
}
cmp_device_fn double3 cmp_double4_to_double3(double4 f)
{
	return double3(f.x, f.y, f.z);
}

cmp_device_fn float3 cmp_double3_to_float3(double3 f)
{
	return float3(float(f.x), float(f.y), float(f.z));
}
cmp_device_fn int3 cmp_double3_to_int3(double3 f)
{
	return int3(int(f.x), int(f.y), int(f.z));
}

cmp_device_fn float3 cmp_int3_to_float3(int3 i)
{
	return float3(float(i.x), float(i.y), float(i.z));
}
cmp_device_fn double3 cmp_int3_to_double3(int3 i)
{
	return double3(double(i.x), double(i.y), double(i.z));
}


//
// Atomic operations
// TODO: NOT atomic in C++ land - is this necessary?
//
cmp_device_fn uint cmp_atomic_or(uint* p, uint val)
{
	uint old = *p;
	*p |= val;
	return old;
}
cmp_device_fn uint cmp_atomic_min(uint* p, uint val)
{
	uint old = *p;
	*p = (old < val) ? old : val;
	return old;
}
cmp_device_fn uint cmp_atomic_max(uint* p, uint val)
{
	uint old = *p;
	*p = (old < val) ? old : val;
	return old;
}


cmp_device_fn float3 cmp_floor(float3 v)
{
	return float3(floorf(v.x), floorf(v.y), floorf(v.z));
}


} // End namespace cmp


#endif
