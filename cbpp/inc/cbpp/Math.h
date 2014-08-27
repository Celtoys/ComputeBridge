
#ifndef INCLUDED_CBPP_MATH_H
#define INCLUDED_CBPP_MATH_H


// Allow math types to be reflected by clReflect
// https://bitbucket.org/dwilliamson/clreflect
#if defined(__cplusplus) && defined(CBPP_USE_CLREFLECT)
	#include <clcpp/clcpp.h>
#else
	#define clcpp_attr(x)
#endif


// Basic type aliases
typedef unsigned char uchar;
typedef unsigned short ushort;
typedef unsigned int uint;
typedef unsigned long ulong;


// "Generic"implementation of vector types
// Use macros instead of templates so that the types can be reflected
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


// Instantiations of vector types
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


#endif	// INCLUDED_CBPP_MATH_H
