
//
// Packing Rules for Constant Variables (HLSL)
//    http://msdn.microsoft.com/en-us/library/windows/desktop/bb509632(v=vs.85).aspx
//
// Device Memory Accesses
//    http://docs.nvidia.com/cuda/cuda-c-programming-guide/index.html#device-memory-accesses
//
// Alignment Requirements in Device Code
//    http://docs.nvidia.com/cuda/cuda-c-programming-guide/index.html#vector-types__alignment-requirements-in-device-code
//
// There are only alignment/packing requirements on each platform. The size of the individual vector types
// does not differ. This alignment/packing is only relevant when sending data from the host to the device,
// or back.
//

#ifndef INCLUDED_CBPP_MATH_H
#define INCLUDED_CBPP_MATH_H


#include "Platform.h"


// Prologue check
#if defined(CMP_CPP) || defined(CMP_HLSL) || defined(CMP_WITHIN_PROLOGUE)


// This header file uses pycgen to generate functions and types that can be forward-declared
// and stepped-through in the debugger.
/*$pycgen
	basic_types = "float,double,int,uint"

	# These types are not supported by HLSL. For now, HLSL can't use them.
	extended_types = "char,uchar,short,ushort,long,ulong"

	all_types = basic_types + "," + extended_types
*/


// Allow math types to be reflected by clReflect
// https://bitbucket.org/dwilliamson/clreflect
#if defined(CMP_CPP) && defined(CBPP_USE_CLREFLECT)
	#include <clcpp/clcpp.h>
#else
	#define clcpp_attr(x)
#endif


// CRT math header
#if defined(CMP_CPP)

	#if defined(CBPP_USE_TINYCRT)
		#include <tinycrt/tinycrt.h>
	#else
		#include <math.h>
	#endif

#endif


// Treat all CUDA functions as device functions
#if defined(CMP_CUDA)
	#define cmp_math_fn __device__
#else
	#define cmp_math_fn
#endif


#if defined(__cplusplus)

	// Basic type aliases
	typedef unsigned char uchar;
	typedef unsigned short ushort;
	typedef unsigned int uint;
	typedef unsigned long ulong;

#endif


#if defined(CMP_CPP)


	// Vector Types ------------------------------------------------------------------------------------
	/*$pycgen
		tuple2 = """
		struct clcpp_attr(reflect_part) T2
		{
			T2() : x(0), y(0) { }
			T2(T x, T y) : x(x), y(y) { }
			T x, y;
		};
		"""

		tuple3 = """
		struct clcpp_attr(reflect_part) T3
		{
			T3() : x(0), y(0), z(0) { }
			T3(T x, T y, T z) : x(x), y(y), z(z) { }
			T x, y, z;
		};
		"""

		tuple4 = """
		struct clcpp_attr(reflect_part) T4
		{
			T4() : x(0), y(0), z(0), w(0) { }
			T4(T x, T y, T z, T w) : x(x), y(y), z(z), w(w) { }
			T x, y, z, w;
		};
		"""

		EmitRepl(tuple2, "T:" + all_types)
		EmitRepl(tuple3, "T:" + all_types)
		EmitRepl(tuple4, "T:" + all_types)
	*/
	//$pycgen-begin
		struct clcpp_attr(reflect_part) float2
		{
			float2() : x(0), y(0) { }
			float2(float x, float y) : x(x), y(y) { }
			float x, y;
		};
		struct clcpp_attr(reflect_part) double2
		{
			double2() : x(0), y(0) { }
			double2(double x, double y) : x(x), y(y) { }
			double x, y;
		};
		struct clcpp_attr(reflect_part) int2
		{
			int2() : x(0), y(0) { }
			int2(int x, int y) : x(x), y(y) { }
			int x, y;
		};
		struct clcpp_attr(reflect_part) uint2
		{
			uint2() : x(0), y(0) { }
			uint2(uint x, uint y) : x(x), y(y) { }
			uint x, y;
		};
		struct clcpp_attr(reflect_part) char2
		{
			char2() : x(0), y(0) { }
			char2(char x, char y) : x(x), y(y) { }
			char x, y;
		};
		struct clcpp_attr(reflect_part) uchar2
		{
			uchar2() : x(0), y(0) { }
			uchar2(uchar x, uchar y) : x(x), y(y) { }
			uchar x, y;
		};
		struct clcpp_attr(reflect_part) short2
		{
			short2() : x(0), y(0) { }
			short2(short x, short y) : x(x), y(y) { }
			short x, y;
		};
		struct clcpp_attr(reflect_part) ushort2
		{
			ushort2() : x(0), y(0) { }
			ushort2(ushort x, ushort y) : x(x), y(y) { }
			ushort x, y;
		};
		struct clcpp_attr(reflect_part) long2
		{
			long2() : x(0), y(0) { }
			long2(long x, long y) : x(x), y(y) { }
			long x, y;
		};
		struct clcpp_attr(reflect_part) ulong2
		{
			ulong2() : x(0), y(0) { }
			ulong2(ulong x, ulong y) : x(x), y(y) { }
			ulong x, y;
		};
		struct clcpp_attr(reflect_part) float3
		{
			float3() : x(0), y(0), z(0) { }
			float3(float x, float y, float z) : x(x), y(y), z(z) { }
			float x, y, z;
		};
		struct clcpp_attr(reflect_part) double3
		{
			double3() : x(0), y(0), z(0) { }
			double3(double x, double y, double z) : x(x), y(y), z(z) { }
			double x, y, z;
		};
		struct clcpp_attr(reflect_part) int3
		{
			int3() : x(0), y(0), z(0) { }
			int3(int x, int y, int z) : x(x), y(y), z(z) { }
			int x, y, z;
		};
		struct clcpp_attr(reflect_part) uint3
		{
			uint3() : x(0), y(0), z(0) { }
			uint3(uint x, uint y, uint z) : x(x), y(y), z(z) { }
			uint x, y, z;
		};
		struct clcpp_attr(reflect_part) char3
		{
			char3() : x(0), y(0), z(0) { }
			char3(char x, char y, char z) : x(x), y(y), z(z) { }
			char x, y, z;
		};
		struct clcpp_attr(reflect_part) uchar3
		{
			uchar3() : x(0), y(0), z(0) { }
			uchar3(uchar x, uchar y, uchar z) : x(x), y(y), z(z) { }
			uchar x, y, z;
		};
		struct clcpp_attr(reflect_part) short3
		{
			short3() : x(0), y(0), z(0) { }
			short3(short x, short y, short z) : x(x), y(y), z(z) { }
			short x, y, z;
		};
		struct clcpp_attr(reflect_part) ushort3
		{
			ushort3() : x(0), y(0), z(0) { }
			ushort3(ushort x, ushort y, ushort z) : x(x), y(y), z(z) { }
			ushort x, y, z;
		};
		struct clcpp_attr(reflect_part) long3
		{
			long3() : x(0), y(0), z(0) { }
			long3(long x, long y, long z) : x(x), y(y), z(z) { }
			long x, y, z;
		};
		struct clcpp_attr(reflect_part) ulong3
		{
			ulong3() : x(0), y(0), z(0) { }
			ulong3(ulong x, ulong y, ulong z) : x(x), y(y), z(z) { }
			ulong x, y, z;
		};
		struct clcpp_attr(reflect_part) float4
		{
			float4() : x(0), y(0), z(0), w(0) { }
			float4(float x, float y, float z, float w) : x(x), y(y), z(z), w(w) { }
			float x, y, z, w;
		};
		struct clcpp_attr(reflect_part) double4
		{
			double4() : x(0), y(0), z(0), w(0) { }
			double4(double x, double y, double z, double w) : x(x), y(y), z(z), w(w) { }
			double x, y, z, w;
		};
		struct clcpp_attr(reflect_part) int4
		{
			int4() : x(0), y(0), z(0), w(0) { }
			int4(int x, int y, int z, int w) : x(x), y(y), z(z), w(w) { }
			int x, y, z, w;
		};
		struct clcpp_attr(reflect_part) uint4
		{
			uint4() : x(0), y(0), z(0), w(0) { }
			uint4(uint x, uint y, uint z, uint w) : x(x), y(y), z(z), w(w) { }
			uint x, y, z, w;
		};
		struct clcpp_attr(reflect_part) char4
		{
			char4() : x(0), y(0), z(0), w(0) { }
			char4(char x, char y, char z, char w) : x(x), y(y), z(z), w(w) { }
			char x, y, z, w;
		};
		struct clcpp_attr(reflect_part) uchar4
		{
			uchar4() : x(0), y(0), z(0), w(0) { }
			uchar4(uchar x, uchar y, uchar z, uchar w) : x(x), y(y), z(z), w(w) { }
			uchar x, y, z, w;
		};
		struct clcpp_attr(reflect_part) short4
		{
			short4() : x(0), y(0), z(0), w(0) { }
			short4(short x, short y, short z, short w) : x(x), y(y), z(z), w(w) { }
			short x, y, z, w;
		};
		struct clcpp_attr(reflect_part) ushort4
		{
			ushort4() : x(0), y(0), z(0), w(0) { }
			ushort4(ushort x, ushort y, ushort z, ushort w) : x(x), y(y), z(z), w(w) { }
			ushort x, y, z, w;
		};
		struct clcpp_attr(reflect_part) long4
		{
			long4() : x(0), y(0), z(0), w(0) { }
			long4(long x, long y, long z, long w) : x(x), y(y), z(z), w(w) { }
			long x, y, z, w;
		};
		struct clcpp_attr(reflect_part) ulong4
		{
			ulong4() : x(0), y(0), z(0), w(0) { }
			ulong4(ulong x, ulong y, ulong z, ulong w) : x(x), y(y), z(z), w(w) { }
			ulong x, y, z, w;
		};
	//$pycgen-end
	

	// Reciprocal Square Root --------------------------------------------------------------------------
	inline cmp_math_fn float rsqrtf(float v)
	{
		return 1.0f / sqrtf(v);
	}
	inline cmp_math_fn double rsqrt(double v)
	{
		return 1.0 / sqrt(v);
	}


#endif


#if defined(CMP_HLSL)


	// Transcendentals --------------------------------------------------------------------------------
	inline cmp_math_fn float sinf(float v)
	{
		return (float)sin(v);
	}
	inline cmp_math_fn float cosf(float v)
	{
		return (float)cos(v);
	}
	inline cmp_math_fn float tanf(float v)
	{
		return (float)tan(v);
	}
	inline cmp_math_fn float sqrtf(float v)
	{
		return (float)sqrt(v);
	}


#endif


// Cross-language vector construction (until ComputeBridge gets constructors) ----------------------
/*$pycgen
	make2 = """
	inline cmp_math_fn T2 T3_to_T2(T3 v)
	{
		T2 r; r.x = v.x; r.y = v.y;
		return r;
	}
	inline cmp_math_fn T2 T2_make(T x, T y)
	{
		T2 r; r.x = x; r.y = y;
		return r;
	}
	"""

	make3 = """
	inline cmp_math_fn T3 T4_to_T3(T4 v)
	{
		T3 r; r.x = v.x; r.y = v.y; r.z = v.z;
		return r;
	}
	inline cmp_math_fn T3 T3_make(T x, T y, T z)
	{
		T3 r; r.x = x; r.y = y; r.z = z;
		return r;
	}
	"""

	make4 = """
	inline cmp_math_fn T4 T4_make(T x, T y, T z, T w)
	{
		T4 r; r.x = x; r.y = y; r.z = z; r.w = w;
		return r;
	}
	"""

	# All target languges support the basic types
	EmitRepl(make2, "T:" + basic_types)
	EmitRepl(make3, "T:" + basic_types)
	EmitRepl(make4, "T:" + basic_types)

	# HLSL does not support the extended types
	EmitLn("#if !defined(CMP_HLSL)")
	EmitRepl(make2, "T:" + extended_types)
	EmitRepl(make3, "T:" + extended_types)
	EmitRepl(make4, "T:" + extended_types)
	EmitLn("#endif")
*/
//$pycgen-begin
	inline cmp_math_fn float2 float3_to_float2(float3 v)
	{
		float2 r; r.x = v.x; r.y = v.y;
		return r;
	}
	inline cmp_math_fn float2 float2_make(float x, float y)
	{
		float2 r; r.x = x; r.y = y;
		return r;
	}
	inline cmp_math_fn double2 double3_to_double2(double3 v)
	{
		double2 r; r.x = v.x; r.y = v.y;
		return r;
	}
	inline cmp_math_fn double2 double2_make(double x, double y)
	{
		double2 r; r.x = x; r.y = y;
		return r;
	}
	inline cmp_math_fn int2 int3_to_int2(int3 v)
	{
		int2 r; r.x = v.x; r.y = v.y;
		return r;
	}
	inline cmp_math_fn int2 int2_make(int x, int y)
	{
		int2 r; r.x = x; r.y = y;
		return r;
	}
	inline cmp_math_fn uint2 uint3_to_uint2(uint3 v)
	{
		uint2 r; r.x = v.x; r.y = v.y;
		return r;
	}
	inline cmp_math_fn uint2 uint2_make(uint x, uint y)
	{
		uint2 r; r.x = x; r.y = y;
		return r;
	}
	inline cmp_math_fn float3 float4_to_float3(float4 v)
	{
		float3 r; r.x = v.x; r.y = v.y; r.z = v.z;
		return r;
	}
	inline cmp_math_fn float3 float3_make(float x, float y, float z)
	{
		float3 r; r.x = x; r.y = y; r.z = z;
		return r;
	}
	inline cmp_math_fn double3 double4_to_double3(double4 v)
	{
		double3 r; r.x = v.x; r.y = v.y; r.z = v.z;
		return r;
	}
	inline cmp_math_fn double3 double3_make(double x, double y, double z)
	{
		double3 r; r.x = x; r.y = y; r.z = z;
		return r;
	}
	inline cmp_math_fn int3 int4_to_int3(int4 v)
	{
		int3 r; r.x = v.x; r.y = v.y; r.z = v.z;
		return r;
	}
	inline cmp_math_fn int3 int3_make(int x, int y, int z)
	{
		int3 r; r.x = x; r.y = y; r.z = z;
		return r;
	}
	inline cmp_math_fn uint3 uint4_to_uint3(uint4 v)
	{
		uint3 r; r.x = v.x; r.y = v.y; r.z = v.z;
		return r;
	}
	inline cmp_math_fn uint3 uint3_make(uint x, uint y, uint z)
	{
		uint3 r; r.x = x; r.y = y; r.z = z;
		return r;
	}
	inline cmp_math_fn float4 float4_make(float x, float y, float z, float w)
	{
		float4 r; r.x = x; r.y = y; r.z = z; r.w = w;
		return r;
	}
	inline cmp_math_fn double4 double4_make(double x, double y, double z, double w)
	{
		double4 r; r.x = x; r.y = y; r.z = z; r.w = w;
		return r;
	}
	inline cmp_math_fn int4 int4_make(int x, int y, int z, int w)
	{
		int4 r; r.x = x; r.y = y; r.z = z; r.w = w;
		return r;
	}
	inline cmp_math_fn uint4 uint4_make(uint x, uint y, uint z, uint w)
	{
		uint4 r; r.x = x; r.y = y; r.z = z; r.w = w;
		return r;
	}
	#if !defined(CMP_HLSL)
	inline cmp_math_fn char2 char3_to_char2(char3 v)
	{
		char2 r; r.x = v.x; r.y = v.y;
		return r;
	}
	inline cmp_math_fn char2 char2_make(char x, char y)
	{
		char2 r; r.x = x; r.y = y;
		return r;
	}
	inline cmp_math_fn uchar2 uchar3_to_uchar2(uchar3 v)
	{
		uchar2 r; r.x = v.x; r.y = v.y;
		return r;
	}
	inline cmp_math_fn uchar2 uchar2_make(uchar x, uchar y)
	{
		uchar2 r; r.x = x; r.y = y;
		return r;
	}
	inline cmp_math_fn short2 short3_to_short2(short3 v)
	{
		short2 r; r.x = v.x; r.y = v.y;
		return r;
	}
	inline cmp_math_fn short2 short2_make(short x, short y)
	{
		short2 r; r.x = x; r.y = y;
		return r;
	}
	inline cmp_math_fn ushort2 ushort3_to_ushort2(ushort3 v)
	{
		ushort2 r; r.x = v.x; r.y = v.y;
		return r;
	}
	inline cmp_math_fn ushort2 ushort2_make(ushort x, ushort y)
	{
		ushort2 r; r.x = x; r.y = y;
		return r;
	}
	inline cmp_math_fn long2 long3_to_long2(long3 v)
	{
		long2 r; r.x = v.x; r.y = v.y;
		return r;
	}
	inline cmp_math_fn long2 long2_make(long x, long y)
	{
		long2 r; r.x = x; r.y = y;
		return r;
	}
	inline cmp_math_fn ulong2 ulong3_to_ulong2(ulong3 v)
	{
		ulong2 r; r.x = v.x; r.y = v.y;
		return r;
	}
	inline cmp_math_fn ulong2 ulong2_make(ulong x, ulong y)
	{
		ulong2 r; r.x = x; r.y = y;
		return r;
	}
	inline cmp_math_fn char3 char4_to_char3(char4 v)
	{
		char3 r; r.x = v.x; r.y = v.y; r.z = v.z;
		return r;
	}
	inline cmp_math_fn char3 char3_make(char x, char y, char z)
	{
		char3 r; r.x = x; r.y = y; r.z = z;
		return r;
	}
	inline cmp_math_fn uchar3 uchar4_to_uchar3(uchar4 v)
	{
		uchar3 r; r.x = v.x; r.y = v.y; r.z = v.z;
		return r;
	}
	inline cmp_math_fn uchar3 uchar3_make(uchar x, uchar y, uchar z)
	{
		uchar3 r; r.x = x; r.y = y; r.z = z;
		return r;
	}
	inline cmp_math_fn short3 short4_to_short3(short4 v)
	{
		short3 r; r.x = v.x; r.y = v.y; r.z = v.z;
		return r;
	}
	inline cmp_math_fn short3 short3_make(short x, short y, short z)
	{
		short3 r; r.x = x; r.y = y; r.z = z;
		return r;
	}
	inline cmp_math_fn ushort3 ushort4_to_ushort3(ushort4 v)
	{
		ushort3 r; r.x = v.x; r.y = v.y; r.z = v.z;
		return r;
	}
	inline cmp_math_fn ushort3 ushort3_make(ushort x, ushort y, ushort z)
	{
		ushort3 r; r.x = x; r.y = y; r.z = z;
		return r;
	}
	inline cmp_math_fn long3 long4_to_long3(long4 v)
	{
		long3 r; r.x = v.x; r.y = v.y; r.z = v.z;
		return r;
	}
	inline cmp_math_fn long3 long3_make(long x, long y, long z)
	{
		long3 r; r.x = x; r.y = y; r.z = z;
		return r;
	}
	inline cmp_math_fn ulong3 ulong4_to_ulong3(ulong4 v)
	{
		ulong3 r; r.x = v.x; r.y = v.y; r.z = v.z;
		return r;
	}
	inline cmp_math_fn ulong3 ulong3_make(ulong x, ulong y, ulong z)
	{
		ulong3 r; r.x = x; r.y = y; r.z = z;
		return r;
	}
	inline cmp_math_fn char4 char4_make(char x, char y, char z, char w)
	{
		char4 r; r.x = x; r.y = y; r.z = z; r.w = w;
		return r;
	}
	inline cmp_math_fn uchar4 uchar4_make(uchar x, uchar y, uchar z, uchar w)
	{
		uchar4 r; r.x = x; r.y = y; r.z = z; r.w = w;
		return r;
	}
	inline cmp_math_fn short4 short4_make(short x, short y, short z, short w)
	{
		short4 r; r.x = x; r.y = y; r.z = z; r.w = w;
		return r;
	}
	inline cmp_math_fn ushort4 ushort4_make(ushort x, ushort y, ushort z, ushort w)
	{
		ushort4 r; r.x = x; r.y = y; r.z = z; r.w = w;
		return r;
	}
	inline cmp_math_fn long4 long4_make(long x, long y, long z, long w)
	{
		long4 r; r.x = x; r.y = y; r.z = z; r.w = w;
		return r;
	}
	inline cmp_math_fn ulong4 ulong4_make(ulong x, ulong y, ulong z, ulong w)
	{
		ulong4 r; r.x = x; r.y = y; r.z = z; r.w = w;
		return r;
	}
	#endif
//$pycgen-end


#if defined(CMP_CPP) || defined(CMP_CUDA)


	// Add operator ------------------------------------------------------------------------------------
	/*$pycgen
		add2 = """
		inline cmp_math_fn T2 operator + (T2 a, T2 b)
		{
			return T2_make(a.x + b.x, a.y + b.y);
		}
		inline cmp_math_fn T2 operator + (T2 a, T b)
		{
			return T2_make(a.x + b, a.y + b);
		}
		inline cmp_math_fn T2 operator + (T a, T2 b)
		{
			return b + a;
		}
		inline cmp_math_fn void operator += (T2& a, T2 b)
		{
			a.x += b.x; a.y += b.y;
		}
		inline cmp_math_fn void operator += (T2& a, T b)
		{
			a.x += b; a.y += b;
		}
		"""

		add3 = """
		inline cmp_math_fn T3 operator + (T3 a, T3 b)
		{
			return T3_make(a.x + b.x, a.y + b.y, a.z + b.z);
		}
		inline cmp_math_fn T3 operator + (T3 a, T b)
		{
			return T3_make(a.x + b, a.y + b, a.z + b);
		}
		inline cmp_math_fn T3 operator + (T a, T3 b)
		{
			return b + a;
		}
		inline cmp_math_fn void operator += (T3& a, T3 b)
		{
			a.x += b.x; a.y += b.y; a.z += b.z;
		}
		inline cmp_math_fn void operator += (T3& a, T b)
		{
			a.x += b; a.y += b; a.z += b;
		}
		"""

		add4 = """
		inline cmp_math_fn T4 operator + (T4 a, T4 b)
		{
			return T4_make(a.x + b.x, a.y + b.y, a.z + b.z, a.w + b.w);
		}
		inline cmp_math_fn T4 operator + (T4 a, T b)
		{
			return T4_make(a.x + b, a.y + b, a.z + b, a.w + b);
		}
		inline cmp_math_fn T4 operator + (T a, T4 b)
		{
			return b + a;
		}
		inline cmp_math_fn void operator += (T4& a, T4 b)
		{
			a.x += b.x; a.y += b.y; a.z += b.z; a.w += b.w;
		}
		inline cmp_math_fn void operator += (T4& a, T b)
		{
			a.x += b; a.y += b; a.z += b; a.w += b;
		}
		"""

		EmitRepl(add2, "T:" + all_types)
		EmitRepl(add3, "T:" + all_types)
		EmitRepl(add4, "T:" + all_types)
	*/
	//$pycgen-begin
		inline cmp_math_fn float2 operator + (float2 a, float2 b)
		{
			return float2_make(a.x + b.x, a.y + b.y);
		}
		inline cmp_math_fn float2 operator + (float2 a, float b)
		{
			return float2_make(a.x + b, a.y + b);
		}
		inline cmp_math_fn float2 operator + (float a, float2 b)
		{
			return b + a;
		}
		inline cmp_math_fn void operator += (float2& a, float2 b)
		{
			a.x += b.x; a.y += b.y;
		}
		inline cmp_math_fn void operator += (float2& a, float b)
		{
			a.x += b; a.y += b;
		}
		inline cmp_math_fn double2 operator + (double2 a, double2 b)
		{
			return double2_make(a.x + b.x, a.y + b.y);
		}
		inline cmp_math_fn double2 operator + (double2 a, double b)
		{
			return double2_make(a.x + b, a.y + b);
		}
		inline cmp_math_fn double2 operator + (double a, double2 b)
		{
			return b + a;
		}
		inline cmp_math_fn void operator += (double2& a, double2 b)
		{
			a.x += b.x; a.y += b.y;
		}
		inline cmp_math_fn void operator += (double2& a, double b)
		{
			a.x += b; a.y += b;
		}
		inline cmp_math_fn int2 operator + (int2 a, int2 b)
		{
			return int2_make(a.x + b.x, a.y + b.y);
		}
		inline cmp_math_fn int2 operator + (int2 a, int b)
		{
			return int2_make(a.x + b, a.y + b);
		}
		inline cmp_math_fn int2 operator + (int a, int2 b)
		{
			return b + a;
		}
		inline cmp_math_fn void operator += (int2& a, int2 b)
		{
			a.x += b.x; a.y += b.y;
		}
		inline cmp_math_fn void operator += (int2& a, int b)
		{
			a.x += b; a.y += b;
		}
		inline cmp_math_fn uint2 operator + (uint2 a, uint2 b)
		{
			return uint2_make(a.x + b.x, a.y + b.y);
		}
		inline cmp_math_fn uint2 operator + (uint2 a, uint b)
		{
			return uint2_make(a.x + b, a.y + b);
		}
		inline cmp_math_fn uint2 operator + (uint a, uint2 b)
		{
			return b + a;
		}
		inline cmp_math_fn void operator += (uint2& a, uint2 b)
		{
			a.x += b.x; a.y += b.y;
		}
		inline cmp_math_fn void operator += (uint2& a, uint b)
		{
			a.x += b; a.y += b;
		}
		inline cmp_math_fn char2 operator + (char2 a, char2 b)
		{
			return char2_make(a.x + b.x, a.y + b.y);
		}
		inline cmp_math_fn char2 operator + (char2 a, char b)
		{
			return char2_make(a.x + b, a.y + b);
		}
		inline cmp_math_fn char2 operator + (char a, char2 b)
		{
			return b + a;
		}
		inline cmp_math_fn void operator += (char2& a, char2 b)
		{
			a.x += b.x; a.y += b.y;
		}
		inline cmp_math_fn void operator += (char2& a, char b)
		{
			a.x += b; a.y += b;
		}
		inline cmp_math_fn uchar2 operator + (uchar2 a, uchar2 b)
		{
			return uchar2_make(a.x + b.x, a.y + b.y);
		}
		inline cmp_math_fn uchar2 operator + (uchar2 a, uchar b)
		{
			return uchar2_make(a.x + b, a.y + b);
		}
		inline cmp_math_fn uchar2 operator + (uchar a, uchar2 b)
		{
			return b + a;
		}
		inline cmp_math_fn void operator += (uchar2& a, uchar2 b)
		{
			a.x += b.x; a.y += b.y;
		}
		inline cmp_math_fn void operator += (uchar2& a, uchar b)
		{
			a.x += b; a.y += b;
		}
		inline cmp_math_fn short2 operator + (short2 a, short2 b)
		{
			return short2_make(a.x + b.x, a.y + b.y);
		}
		inline cmp_math_fn short2 operator + (short2 a, short b)
		{
			return short2_make(a.x + b, a.y + b);
		}
		inline cmp_math_fn short2 operator + (short a, short2 b)
		{
			return b + a;
		}
		inline cmp_math_fn void operator += (short2& a, short2 b)
		{
			a.x += b.x; a.y += b.y;
		}
		inline cmp_math_fn void operator += (short2& a, short b)
		{
			a.x += b; a.y += b;
		}
		inline cmp_math_fn ushort2 operator + (ushort2 a, ushort2 b)
		{
			return ushort2_make(a.x + b.x, a.y + b.y);
		}
		inline cmp_math_fn ushort2 operator + (ushort2 a, ushort b)
		{
			return ushort2_make(a.x + b, a.y + b);
		}
		inline cmp_math_fn ushort2 operator + (ushort a, ushort2 b)
		{
			return b + a;
		}
		inline cmp_math_fn void operator += (ushort2& a, ushort2 b)
		{
			a.x += b.x; a.y += b.y;
		}
		inline cmp_math_fn void operator += (ushort2& a, ushort b)
		{
			a.x += b; a.y += b;
		}
		inline cmp_math_fn long2 operator + (long2 a, long2 b)
		{
			return long2_make(a.x + b.x, a.y + b.y);
		}
		inline cmp_math_fn long2 operator + (long2 a, long b)
		{
			return long2_make(a.x + b, a.y + b);
		}
		inline cmp_math_fn long2 operator + (long a, long2 b)
		{
			return b + a;
		}
		inline cmp_math_fn void operator += (long2& a, long2 b)
		{
			a.x += b.x; a.y += b.y;
		}
		inline cmp_math_fn void operator += (long2& a, long b)
		{
			a.x += b; a.y += b;
		}
		inline cmp_math_fn ulong2 operator + (ulong2 a, ulong2 b)
		{
			return ulong2_make(a.x + b.x, a.y + b.y);
		}
		inline cmp_math_fn ulong2 operator + (ulong2 a, ulong b)
		{
			return ulong2_make(a.x + b, a.y + b);
		}
		inline cmp_math_fn ulong2 operator + (ulong a, ulong2 b)
		{
			return b + a;
		}
		inline cmp_math_fn void operator += (ulong2& a, ulong2 b)
		{
			a.x += b.x; a.y += b.y;
		}
		inline cmp_math_fn void operator += (ulong2& a, ulong b)
		{
			a.x += b; a.y += b;
		}
		inline cmp_math_fn float3 operator + (float3 a, float3 b)
		{
			return float3_make(a.x + b.x, a.y + b.y, a.z + b.z);
		}
		inline cmp_math_fn float3 operator + (float3 a, float b)
		{
			return float3_make(a.x + b, a.y + b, a.z + b);
		}
		inline cmp_math_fn float3 operator + (float a, float3 b)
		{
			return b + a;
		}
		inline cmp_math_fn void operator += (float3& a, float3 b)
		{
			a.x += b.x; a.y += b.y; a.z += b.z;
		}
		inline cmp_math_fn void operator += (float3& a, float b)
		{
			a.x += b; a.y += b; a.z += b;
		}
		inline cmp_math_fn double3 operator + (double3 a, double3 b)
		{
			return double3_make(a.x + b.x, a.y + b.y, a.z + b.z);
		}
		inline cmp_math_fn double3 operator + (double3 a, double b)
		{
			return double3_make(a.x + b, a.y + b, a.z + b);
		}
		inline cmp_math_fn double3 operator + (double a, double3 b)
		{
			return b + a;
		}
		inline cmp_math_fn void operator += (double3& a, double3 b)
		{
			a.x += b.x; a.y += b.y; a.z += b.z;
		}
		inline cmp_math_fn void operator += (double3& a, double b)
		{
			a.x += b; a.y += b; a.z += b;
		}
		inline cmp_math_fn int3 operator + (int3 a, int3 b)
		{
			return int3_make(a.x + b.x, a.y + b.y, a.z + b.z);
		}
		inline cmp_math_fn int3 operator + (int3 a, int b)
		{
			return int3_make(a.x + b, a.y + b, a.z + b);
		}
		inline cmp_math_fn int3 operator + (int a, int3 b)
		{
			return b + a;
		}
		inline cmp_math_fn void operator += (int3& a, int3 b)
		{
			a.x += b.x; a.y += b.y; a.z += b.z;
		}
		inline cmp_math_fn void operator += (int3& a, int b)
		{
			a.x += b; a.y += b; a.z += b;
		}
		inline cmp_math_fn uint3 operator + (uint3 a, uint3 b)
		{
			return uint3_make(a.x + b.x, a.y + b.y, a.z + b.z);
		}
		inline cmp_math_fn uint3 operator + (uint3 a, uint b)
		{
			return uint3_make(a.x + b, a.y + b, a.z + b);
		}
		inline cmp_math_fn uint3 operator + (uint a, uint3 b)
		{
			return b + a;
		}
		inline cmp_math_fn void operator += (uint3& a, uint3 b)
		{
			a.x += b.x; a.y += b.y; a.z += b.z;
		}
		inline cmp_math_fn void operator += (uint3& a, uint b)
		{
			a.x += b; a.y += b; a.z += b;
		}
		inline cmp_math_fn char3 operator + (char3 a, char3 b)
		{
			return char3_make(a.x + b.x, a.y + b.y, a.z + b.z);
		}
		inline cmp_math_fn char3 operator + (char3 a, char b)
		{
			return char3_make(a.x + b, a.y + b, a.z + b);
		}
		inline cmp_math_fn char3 operator + (char a, char3 b)
		{
			return b + a;
		}
		inline cmp_math_fn void operator += (char3& a, char3 b)
		{
			a.x += b.x; a.y += b.y; a.z += b.z;
		}
		inline cmp_math_fn void operator += (char3& a, char b)
		{
			a.x += b; a.y += b; a.z += b;
		}
		inline cmp_math_fn uchar3 operator + (uchar3 a, uchar3 b)
		{
			return uchar3_make(a.x + b.x, a.y + b.y, a.z + b.z);
		}
		inline cmp_math_fn uchar3 operator + (uchar3 a, uchar b)
		{
			return uchar3_make(a.x + b, a.y + b, a.z + b);
		}
		inline cmp_math_fn uchar3 operator + (uchar a, uchar3 b)
		{
			return b + a;
		}
		inline cmp_math_fn void operator += (uchar3& a, uchar3 b)
		{
			a.x += b.x; a.y += b.y; a.z += b.z;
		}
		inline cmp_math_fn void operator += (uchar3& a, uchar b)
		{
			a.x += b; a.y += b; a.z += b;
		}
		inline cmp_math_fn short3 operator + (short3 a, short3 b)
		{
			return short3_make(a.x + b.x, a.y + b.y, a.z + b.z);
		}
		inline cmp_math_fn short3 operator + (short3 a, short b)
		{
			return short3_make(a.x + b, a.y + b, a.z + b);
		}
		inline cmp_math_fn short3 operator + (short a, short3 b)
		{
			return b + a;
		}
		inline cmp_math_fn void operator += (short3& a, short3 b)
		{
			a.x += b.x; a.y += b.y; a.z += b.z;
		}
		inline cmp_math_fn void operator += (short3& a, short b)
		{
			a.x += b; a.y += b; a.z += b;
		}
		inline cmp_math_fn ushort3 operator + (ushort3 a, ushort3 b)
		{
			return ushort3_make(a.x + b.x, a.y + b.y, a.z + b.z);
		}
		inline cmp_math_fn ushort3 operator + (ushort3 a, ushort b)
		{
			return ushort3_make(a.x + b, a.y + b, a.z + b);
		}
		inline cmp_math_fn ushort3 operator + (ushort a, ushort3 b)
		{
			return b + a;
		}
		inline cmp_math_fn void operator += (ushort3& a, ushort3 b)
		{
			a.x += b.x; a.y += b.y; a.z += b.z;
		}
		inline cmp_math_fn void operator += (ushort3& a, ushort b)
		{
			a.x += b; a.y += b; a.z += b;
		}
		inline cmp_math_fn long3 operator + (long3 a, long3 b)
		{
			return long3_make(a.x + b.x, a.y + b.y, a.z + b.z);
		}
		inline cmp_math_fn long3 operator + (long3 a, long b)
		{
			return long3_make(a.x + b, a.y + b, a.z + b);
		}
		inline cmp_math_fn long3 operator + (long a, long3 b)
		{
			return b + a;
		}
		inline cmp_math_fn void operator += (long3& a, long3 b)
		{
			a.x += b.x; a.y += b.y; a.z += b.z;
		}
		inline cmp_math_fn void operator += (long3& a, long b)
		{
			a.x += b; a.y += b; a.z += b;
		}
		inline cmp_math_fn ulong3 operator + (ulong3 a, ulong3 b)
		{
			return ulong3_make(a.x + b.x, a.y + b.y, a.z + b.z);
		}
		inline cmp_math_fn ulong3 operator + (ulong3 a, ulong b)
		{
			return ulong3_make(a.x + b, a.y + b, a.z + b);
		}
		inline cmp_math_fn ulong3 operator + (ulong a, ulong3 b)
		{
			return b + a;
		}
		inline cmp_math_fn void operator += (ulong3& a, ulong3 b)
		{
			a.x += b.x; a.y += b.y; a.z += b.z;
		}
		inline cmp_math_fn void operator += (ulong3& a, ulong b)
		{
			a.x += b; a.y += b; a.z += b;
		}
		inline cmp_math_fn float4 operator + (float4 a, float4 b)
		{
			return float4_make(a.x + b.x, a.y + b.y, a.z + b.z, a.w + b.w);
		}
		inline cmp_math_fn float4 operator + (float4 a, float b)
		{
			return float4_make(a.x + b, a.y + b, a.z + b, a.w + b);
		}
		inline cmp_math_fn float4 operator + (float a, float4 b)
		{
			return b + a;
		}
		inline cmp_math_fn void operator += (float4& a, float4 b)
		{
			a.x += b.x; a.y += b.y; a.z += b.z; a.w += b.w;
		}
		inline cmp_math_fn void operator += (float4& a, float b)
		{
			a.x += b; a.y += b; a.z += b; a.w += b;
		}
		inline cmp_math_fn double4 operator + (double4 a, double4 b)
		{
			return double4_make(a.x + b.x, a.y + b.y, a.z + b.z, a.w + b.w);
		}
		inline cmp_math_fn double4 operator + (double4 a, double b)
		{
			return double4_make(a.x + b, a.y + b, a.z + b, a.w + b);
		}
		inline cmp_math_fn double4 operator + (double a, double4 b)
		{
			return b + a;
		}
		inline cmp_math_fn void operator += (double4& a, double4 b)
		{
			a.x += b.x; a.y += b.y; a.z += b.z; a.w += b.w;
		}
		inline cmp_math_fn void operator += (double4& a, double b)
		{
			a.x += b; a.y += b; a.z += b; a.w += b;
		}
		inline cmp_math_fn int4 operator + (int4 a, int4 b)
		{
			return int4_make(a.x + b.x, a.y + b.y, a.z + b.z, a.w + b.w);
		}
		inline cmp_math_fn int4 operator + (int4 a, int b)
		{
			return int4_make(a.x + b, a.y + b, a.z + b, a.w + b);
		}
		inline cmp_math_fn int4 operator + (int a, int4 b)
		{
			return b + a;
		}
		inline cmp_math_fn void operator += (int4& a, int4 b)
		{
			a.x += b.x; a.y += b.y; a.z += b.z; a.w += b.w;
		}
		inline cmp_math_fn void operator += (int4& a, int b)
		{
			a.x += b; a.y += b; a.z += b; a.w += b;
		}
		inline cmp_math_fn uint4 operator + (uint4 a, uint4 b)
		{
			return uint4_make(a.x + b.x, a.y + b.y, a.z + b.z, a.w + b.w);
		}
		inline cmp_math_fn uint4 operator + (uint4 a, uint b)
		{
			return uint4_make(a.x + b, a.y + b, a.z + b, a.w + b);
		}
		inline cmp_math_fn uint4 operator + (uint a, uint4 b)
		{
			return b + a;
		}
		inline cmp_math_fn void operator += (uint4& a, uint4 b)
		{
			a.x += b.x; a.y += b.y; a.z += b.z; a.w += b.w;
		}
		inline cmp_math_fn void operator += (uint4& a, uint b)
		{
			a.x += b; a.y += b; a.z += b; a.w += b;
		}
		inline cmp_math_fn char4 operator + (char4 a, char4 b)
		{
			return char4_make(a.x + b.x, a.y + b.y, a.z + b.z, a.w + b.w);
		}
		inline cmp_math_fn char4 operator + (char4 a, char b)
		{
			return char4_make(a.x + b, a.y + b, a.z + b, a.w + b);
		}
		inline cmp_math_fn char4 operator + (char a, char4 b)
		{
			return b + a;
		}
		inline cmp_math_fn void operator += (char4& a, char4 b)
		{
			a.x += b.x; a.y += b.y; a.z += b.z; a.w += b.w;
		}
		inline cmp_math_fn void operator += (char4& a, char b)
		{
			a.x += b; a.y += b; a.z += b; a.w += b;
		}
		inline cmp_math_fn uchar4 operator + (uchar4 a, uchar4 b)
		{
			return uchar4_make(a.x + b.x, a.y + b.y, a.z + b.z, a.w + b.w);
		}
		inline cmp_math_fn uchar4 operator + (uchar4 a, uchar b)
		{
			return uchar4_make(a.x + b, a.y + b, a.z + b, a.w + b);
		}
		inline cmp_math_fn uchar4 operator + (uchar a, uchar4 b)
		{
			return b + a;
		}
		inline cmp_math_fn void operator += (uchar4& a, uchar4 b)
		{
			a.x += b.x; a.y += b.y; a.z += b.z; a.w += b.w;
		}
		inline cmp_math_fn void operator += (uchar4& a, uchar b)
		{
			a.x += b; a.y += b; a.z += b; a.w += b;
		}
		inline cmp_math_fn short4 operator + (short4 a, short4 b)
		{
			return short4_make(a.x + b.x, a.y + b.y, a.z + b.z, a.w + b.w);
		}
		inline cmp_math_fn short4 operator + (short4 a, short b)
		{
			return short4_make(a.x + b, a.y + b, a.z + b, a.w + b);
		}
		inline cmp_math_fn short4 operator + (short a, short4 b)
		{
			return b + a;
		}
		inline cmp_math_fn void operator += (short4& a, short4 b)
		{
			a.x += b.x; a.y += b.y; a.z += b.z; a.w += b.w;
		}
		inline cmp_math_fn void operator += (short4& a, short b)
		{
			a.x += b; a.y += b; a.z += b; a.w += b;
		}
		inline cmp_math_fn ushort4 operator + (ushort4 a, ushort4 b)
		{
			return ushort4_make(a.x + b.x, a.y + b.y, a.z + b.z, a.w + b.w);
		}
		inline cmp_math_fn ushort4 operator + (ushort4 a, ushort b)
		{
			return ushort4_make(a.x + b, a.y + b, a.z + b, a.w + b);
		}
		inline cmp_math_fn ushort4 operator + (ushort a, ushort4 b)
		{
			return b + a;
		}
		inline cmp_math_fn void operator += (ushort4& a, ushort4 b)
		{
			a.x += b.x; a.y += b.y; a.z += b.z; a.w += b.w;
		}
		inline cmp_math_fn void operator += (ushort4& a, ushort b)
		{
			a.x += b; a.y += b; a.z += b; a.w += b;
		}
		inline cmp_math_fn long4 operator + (long4 a, long4 b)
		{
			return long4_make(a.x + b.x, a.y + b.y, a.z + b.z, a.w + b.w);
		}
		inline cmp_math_fn long4 operator + (long4 a, long b)
		{
			return long4_make(a.x + b, a.y + b, a.z + b, a.w + b);
		}
		inline cmp_math_fn long4 operator + (long a, long4 b)
		{
			return b + a;
		}
		inline cmp_math_fn void operator += (long4& a, long4 b)
		{
			a.x += b.x; a.y += b.y; a.z += b.z; a.w += b.w;
		}
		inline cmp_math_fn void operator += (long4& a, long b)
		{
			a.x += b; a.y += b; a.z += b; a.w += b;
		}
		inline cmp_math_fn ulong4 operator + (ulong4 a, ulong4 b)
		{
			return ulong4_make(a.x + b.x, a.y + b.y, a.z + b.z, a.w + b.w);
		}
		inline cmp_math_fn ulong4 operator + (ulong4 a, ulong b)
		{
			return ulong4_make(a.x + b, a.y + b, a.z + b, a.w + b);
		}
		inline cmp_math_fn ulong4 operator + (ulong a, ulong4 b)
		{
			return b + a;
		}
		inline cmp_math_fn void operator += (ulong4& a, ulong4 b)
		{
			a.x += b.x; a.y += b.y; a.z += b.z; a.w += b.w;
		}
		inline cmp_math_fn void operator += (ulong4& a, ulong b)
		{
			a.x += b; a.y += b; a.z += b; a.w += b;
		}
	//$pycgen-end


	// Subtract operator -------------------------------------------------------------------------------
	/*$pycgen
		sub2 = """
		inline cmp_math_fn T2 operator - (T2 a, T2 b)
		{
			return T2_make(a.x - b.x, a.y - b.y);
		}
		inline cmp_math_fn T2 operator - (T2 a, T b)
		{
			return T2_make(a.x - b, a.y - b);
		}
		inline cmp_math_fn T2 operator - (T a, T2 b)
		{
			return T2_make(a - b.x, a - b.y);
		}
		inline cmp_math_fn void operator -= (T2& a, T2 b)
		{
			a.x -= b.x; a.y -= b.y;
		}
		inline cmp_math_fn void operator -= (T2& a, T b)
		{
			a.x -= b; a.y -= b;
		}
		"""

		sub3 = """
		inline cmp_math_fn T3 operator - (T3 a, T3 b)
		{
			return T3_make(a.x - b.x, a.y - b.y, a.z - b.z);
		}
		inline cmp_math_fn T3 operator - (T3 a, T b)
		{
			return T3_make(a.x - b, a.y - b, a.z - b);
		}
		inline cmp_math_fn T3 operator - (T a, T3 b)
		{
			return T3_make(a - b.x, a - b.y, a - b.z);
		}
		inline cmp_math_fn void operator -= (T3& a, T3 b)
		{
			a.x -= b.x; a.y -= b.y; a.z -= b.z;
		}
		inline cmp_math_fn void operator -= (T3& a, T b)
		{
			a.x -= b; a.y -= b; a.z -= b;
		}
		"""

		sub4 = """
		inline cmp_math_fn T4 operator - (T4 a, T4 b)
		{
			return T4_make(a.x - b.x, a.y - b.y, a.z - b.z, a.w - b.w);
		}
		inline cmp_math_fn T4 operator - (T4 a, T b)
		{
			return T4_make(a.x - b, a.y - b, a.z - b, a.w - b);
		}
		inline cmp_math_fn T4 operator - (T a, T4 b)
		{
			return T4_make(a - b.x, a - b.y, a - b.z, a - b.w);
		}
		inline cmp_math_fn void operator -= (T4& a, T4 b)
		{
			a.x -= b.x; a.y -= b.y; a.z -= b.z; a.w -= b.w;
		}
		inline cmp_math_fn void operator -= (T4& a, T b)
		{
			a.x -= b; a.y -= b; a.z -= b; a.w -= b;
		}
		"""

		EmitRepl(sub2, "T:" + all_types)
		EmitRepl(sub3, "T:" + all_types)
		EmitRepl(sub4, "T:" + all_types)
	*/
	//$pycgen-begin
		inline cmp_math_fn float2 operator - (float2 a, float2 b)
		{
			return float2_make(a.x - b.x, a.y - b.y);
		}
		inline cmp_math_fn float2 operator - (float2 a, float b)
		{
			return float2_make(a.x - b, a.y - b);
		}
		inline cmp_math_fn float2 operator - (float a, float2 b)
		{
			return float2_make(a - b.x, a - b.y);
		}
		inline cmp_math_fn void operator -= (float2& a, float2 b)
		{
			a.x -= b.x; a.y -= b.y;
		}
		inline cmp_math_fn void operator -= (float2& a, float b)
		{
			a.x -= b; a.y -= b;
		}
		inline cmp_math_fn double2 operator - (double2 a, double2 b)
		{
			return double2_make(a.x - b.x, a.y - b.y);
		}
		inline cmp_math_fn double2 operator - (double2 a, double b)
		{
			return double2_make(a.x - b, a.y - b);
		}
		inline cmp_math_fn double2 operator - (double a, double2 b)
		{
			return double2_make(a - b.x, a - b.y);
		}
		inline cmp_math_fn void operator -= (double2& a, double2 b)
		{
			a.x -= b.x; a.y -= b.y;
		}
		inline cmp_math_fn void operator -= (double2& a, double b)
		{
			a.x -= b; a.y -= b;
		}
		inline cmp_math_fn int2 operator - (int2 a, int2 b)
		{
			return int2_make(a.x - b.x, a.y - b.y);
		}
		inline cmp_math_fn int2 operator - (int2 a, int b)
		{
			return int2_make(a.x - b, a.y - b);
		}
		inline cmp_math_fn int2 operator - (int a, int2 b)
		{
			return int2_make(a - b.x, a - b.y);
		}
		inline cmp_math_fn void operator -= (int2& a, int2 b)
		{
			a.x -= b.x; a.y -= b.y;
		}
		inline cmp_math_fn void operator -= (int2& a, int b)
		{
			a.x -= b; a.y -= b;
		}
		inline cmp_math_fn uint2 operator - (uint2 a, uint2 b)
		{
			return uint2_make(a.x - b.x, a.y - b.y);
		}
		inline cmp_math_fn uint2 operator - (uint2 a, uint b)
		{
			return uint2_make(a.x - b, a.y - b);
		}
		inline cmp_math_fn uint2 operator - (uint a, uint2 b)
		{
			return uint2_make(a - b.x, a - b.y);
		}
		inline cmp_math_fn void operator -= (uint2& a, uint2 b)
		{
			a.x -= b.x; a.y -= b.y;
		}
		inline cmp_math_fn void operator -= (uint2& a, uint b)
		{
			a.x -= b; a.y -= b;
		}
		inline cmp_math_fn char2 operator - (char2 a, char2 b)
		{
			return char2_make(a.x - b.x, a.y - b.y);
		}
		inline cmp_math_fn char2 operator - (char2 a, char b)
		{
			return char2_make(a.x - b, a.y - b);
		}
		inline cmp_math_fn char2 operator - (char a, char2 b)
		{
			return char2_make(a - b.x, a - b.y);
		}
		inline cmp_math_fn void operator -= (char2& a, char2 b)
		{
			a.x -= b.x; a.y -= b.y;
		}
		inline cmp_math_fn void operator -= (char2& a, char b)
		{
			a.x -= b; a.y -= b;
		}
		inline cmp_math_fn uchar2 operator - (uchar2 a, uchar2 b)
		{
			return uchar2_make(a.x - b.x, a.y - b.y);
		}
		inline cmp_math_fn uchar2 operator - (uchar2 a, uchar b)
		{
			return uchar2_make(a.x - b, a.y - b);
		}
		inline cmp_math_fn uchar2 operator - (uchar a, uchar2 b)
		{
			return uchar2_make(a - b.x, a - b.y);
		}
		inline cmp_math_fn void operator -= (uchar2& a, uchar2 b)
		{
			a.x -= b.x; a.y -= b.y;
		}
		inline cmp_math_fn void operator -= (uchar2& a, uchar b)
		{
			a.x -= b; a.y -= b;
		}
		inline cmp_math_fn short2 operator - (short2 a, short2 b)
		{
			return short2_make(a.x - b.x, a.y - b.y);
		}
		inline cmp_math_fn short2 operator - (short2 a, short b)
		{
			return short2_make(a.x - b, a.y - b);
		}
		inline cmp_math_fn short2 operator - (short a, short2 b)
		{
			return short2_make(a - b.x, a - b.y);
		}
		inline cmp_math_fn void operator -= (short2& a, short2 b)
		{
			a.x -= b.x; a.y -= b.y;
		}
		inline cmp_math_fn void operator -= (short2& a, short b)
		{
			a.x -= b; a.y -= b;
		}
		inline cmp_math_fn ushort2 operator - (ushort2 a, ushort2 b)
		{
			return ushort2_make(a.x - b.x, a.y - b.y);
		}
		inline cmp_math_fn ushort2 operator - (ushort2 a, ushort b)
		{
			return ushort2_make(a.x - b, a.y - b);
		}
		inline cmp_math_fn ushort2 operator - (ushort a, ushort2 b)
		{
			return ushort2_make(a - b.x, a - b.y);
		}
		inline cmp_math_fn void operator -= (ushort2& a, ushort2 b)
		{
			a.x -= b.x; a.y -= b.y;
		}
		inline cmp_math_fn void operator -= (ushort2& a, ushort b)
		{
			a.x -= b; a.y -= b;
		}
		inline cmp_math_fn long2 operator - (long2 a, long2 b)
		{
			return long2_make(a.x - b.x, a.y - b.y);
		}
		inline cmp_math_fn long2 operator - (long2 a, long b)
		{
			return long2_make(a.x - b, a.y - b);
		}
		inline cmp_math_fn long2 operator - (long a, long2 b)
		{
			return long2_make(a - b.x, a - b.y);
		}
		inline cmp_math_fn void operator -= (long2& a, long2 b)
		{
			a.x -= b.x; a.y -= b.y;
		}
		inline cmp_math_fn void operator -= (long2& a, long b)
		{
			a.x -= b; a.y -= b;
		}
		inline cmp_math_fn ulong2 operator - (ulong2 a, ulong2 b)
		{
			return ulong2_make(a.x - b.x, a.y - b.y);
		}
		inline cmp_math_fn ulong2 operator - (ulong2 a, ulong b)
		{
			return ulong2_make(a.x - b, a.y - b);
		}
		inline cmp_math_fn ulong2 operator - (ulong a, ulong2 b)
		{
			return ulong2_make(a - b.x, a - b.y);
		}
		inline cmp_math_fn void operator -= (ulong2& a, ulong2 b)
		{
			a.x -= b.x; a.y -= b.y;
		}
		inline cmp_math_fn void operator -= (ulong2& a, ulong b)
		{
			a.x -= b; a.y -= b;
		}
		inline cmp_math_fn float3 operator - (float3 a, float3 b)
		{
			return float3_make(a.x - b.x, a.y - b.y, a.z - b.z);
		}
		inline cmp_math_fn float3 operator - (float3 a, float b)
		{
			return float3_make(a.x - b, a.y - b, a.z - b);
		}
		inline cmp_math_fn float3 operator - (float a, float3 b)
		{
			return float3_make(a - b.x, a - b.y, a - b.z);
		}
		inline cmp_math_fn void operator -= (float3& a, float3 b)
		{
			a.x -= b.x; a.y -= b.y; a.z -= b.z;
		}
		inline cmp_math_fn void operator -= (float3& a, float b)
		{
			a.x -= b; a.y -= b; a.z -= b;
		}
		inline cmp_math_fn double3 operator - (double3 a, double3 b)
		{
			return double3_make(a.x - b.x, a.y - b.y, a.z - b.z);
		}
		inline cmp_math_fn double3 operator - (double3 a, double b)
		{
			return double3_make(a.x - b, a.y - b, a.z - b);
		}
		inline cmp_math_fn double3 operator - (double a, double3 b)
		{
			return double3_make(a - b.x, a - b.y, a - b.z);
		}
		inline cmp_math_fn void operator -= (double3& a, double3 b)
		{
			a.x -= b.x; a.y -= b.y; a.z -= b.z;
		}
		inline cmp_math_fn void operator -= (double3& a, double b)
		{
			a.x -= b; a.y -= b; a.z -= b;
		}
		inline cmp_math_fn int3 operator - (int3 a, int3 b)
		{
			return int3_make(a.x - b.x, a.y - b.y, a.z - b.z);
		}
		inline cmp_math_fn int3 operator - (int3 a, int b)
		{
			return int3_make(a.x - b, a.y - b, a.z - b);
		}
		inline cmp_math_fn int3 operator - (int a, int3 b)
		{
			return int3_make(a - b.x, a - b.y, a - b.z);
		}
		inline cmp_math_fn void operator -= (int3& a, int3 b)
		{
			a.x -= b.x; a.y -= b.y; a.z -= b.z;
		}
		inline cmp_math_fn void operator -= (int3& a, int b)
		{
			a.x -= b; a.y -= b; a.z -= b;
		}
		inline cmp_math_fn uint3 operator - (uint3 a, uint3 b)
		{
			return uint3_make(a.x - b.x, a.y - b.y, a.z - b.z);
		}
		inline cmp_math_fn uint3 operator - (uint3 a, uint b)
		{
			return uint3_make(a.x - b, a.y - b, a.z - b);
		}
		inline cmp_math_fn uint3 operator - (uint a, uint3 b)
		{
			return uint3_make(a - b.x, a - b.y, a - b.z);
		}
		inline cmp_math_fn void operator -= (uint3& a, uint3 b)
		{
			a.x -= b.x; a.y -= b.y; a.z -= b.z;
		}
		inline cmp_math_fn void operator -= (uint3& a, uint b)
		{
			a.x -= b; a.y -= b; a.z -= b;
		}
		inline cmp_math_fn char3 operator - (char3 a, char3 b)
		{
			return char3_make(a.x - b.x, a.y - b.y, a.z - b.z);
		}
		inline cmp_math_fn char3 operator - (char3 a, char b)
		{
			return char3_make(a.x - b, a.y - b, a.z - b);
		}
		inline cmp_math_fn char3 operator - (char a, char3 b)
		{
			return char3_make(a - b.x, a - b.y, a - b.z);
		}
		inline cmp_math_fn void operator -= (char3& a, char3 b)
		{
			a.x -= b.x; a.y -= b.y; a.z -= b.z;
		}
		inline cmp_math_fn void operator -= (char3& a, char b)
		{
			a.x -= b; a.y -= b; a.z -= b;
		}
		inline cmp_math_fn uchar3 operator - (uchar3 a, uchar3 b)
		{
			return uchar3_make(a.x - b.x, a.y - b.y, a.z - b.z);
		}
		inline cmp_math_fn uchar3 operator - (uchar3 a, uchar b)
		{
			return uchar3_make(a.x - b, a.y - b, a.z - b);
		}
		inline cmp_math_fn uchar3 operator - (uchar a, uchar3 b)
		{
			return uchar3_make(a - b.x, a - b.y, a - b.z);
		}
		inline cmp_math_fn void operator -= (uchar3& a, uchar3 b)
		{
			a.x -= b.x; a.y -= b.y; a.z -= b.z;
		}
		inline cmp_math_fn void operator -= (uchar3& a, uchar b)
		{
			a.x -= b; a.y -= b; a.z -= b;
		}
		inline cmp_math_fn short3 operator - (short3 a, short3 b)
		{
			return short3_make(a.x - b.x, a.y - b.y, a.z - b.z);
		}
		inline cmp_math_fn short3 operator - (short3 a, short b)
		{
			return short3_make(a.x - b, a.y - b, a.z - b);
		}
		inline cmp_math_fn short3 operator - (short a, short3 b)
		{
			return short3_make(a - b.x, a - b.y, a - b.z);
		}
		inline cmp_math_fn void operator -= (short3& a, short3 b)
		{
			a.x -= b.x; a.y -= b.y; a.z -= b.z;
		}
		inline cmp_math_fn void operator -= (short3& a, short b)
		{
			a.x -= b; a.y -= b; a.z -= b;
		}
		inline cmp_math_fn ushort3 operator - (ushort3 a, ushort3 b)
		{
			return ushort3_make(a.x - b.x, a.y - b.y, a.z - b.z);
		}
		inline cmp_math_fn ushort3 operator - (ushort3 a, ushort b)
		{
			return ushort3_make(a.x - b, a.y - b, a.z - b);
		}
		inline cmp_math_fn ushort3 operator - (ushort a, ushort3 b)
		{
			return ushort3_make(a - b.x, a - b.y, a - b.z);
		}
		inline cmp_math_fn void operator -= (ushort3& a, ushort3 b)
		{
			a.x -= b.x; a.y -= b.y; a.z -= b.z;
		}
		inline cmp_math_fn void operator -= (ushort3& a, ushort b)
		{
			a.x -= b; a.y -= b; a.z -= b;
		}
		inline cmp_math_fn long3 operator - (long3 a, long3 b)
		{
			return long3_make(a.x - b.x, a.y - b.y, a.z - b.z);
		}
		inline cmp_math_fn long3 operator - (long3 a, long b)
		{
			return long3_make(a.x - b, a.y - b, a.z - b);
		}
		inline cmp_math_fn long3 operator - (long a, long3 b)
		{
			return long3_make(a - b.x, a - b.y, a - b.z);
		}
		inline cmp_math_fn void operator -= (long3& a, long3 b)
		{
			a.x -= b.x; a.y -= b.y; a.z -= b.z;
		}
		inline cmp_math_fn void operator -= (long3& a, long b)
		{
			a.x -= b; a.y -= b; a.z -= b;
		}
		inline cmp_math_fn ulong3 operator - (ulong3 a, ulong3 b)
		{
			return ulong3_make(a.x - b.x, a.y - b.y, a.z - b.z);
		}
		inline cmp_math_fn ulong3 operator - (ulong3 a, ulong b)
		{
			return ulong3_make(a.x - b, a.y - b, a.z - b);
		}
		inline cmp_math_fn ulong3 operator - (ulong a, ulong3 b)
		{
			return ulong3_make(a - b.x, a - b.y, a - b.z);
		}
		inline cmp_math_fn void operator -= (ulong3& a, ulong3 b)
		{
			a.x -= b.x; a.y -= b.y; a.z -= b.z;
		}
		inline cmp_math_fn void operator -= (ulong3& a, ulong b)
		{
			a.x -= b; a.y -= b; a.z -= b;
		}
		inline cmp_math_fn float4 operator - (float4 a, float4 b)
		{
			return float4_make(a.x - b.x, a.y - b.y, a.z - b.z, a.w - b.w);
		}
		inline cmp_math_fn float4 operator - (float4 a, float b)
		{
			return float4_make(a.x - b, a.y - b, a.z - b, a.w - b);
		}
		inline cmp_math_fn float4 operator - (float a, float4 b)
		{
			return float4_make(a - b.x, a - b.y, a - b.z, a - b.w);
		}
		inline cmp_math_fn void operator -= (float4& a, float4 b)
		{
			a.x -= b.x; a.y -= b.y; a.z -= b.z; a.w -= b.w;
		}
		inline cmp_math_fn void operator -= (float4& a, float b)
		{
			a.x -= b; a.y -= b; a.z -= b; a.w -= b;
		}
		inline cmp_math_fn double4 operator - (double4 a, double4 b)
		{
			return double4_make(a.x - b.x, a.y - b.y, a.z - b.z, a.w - b.w);
		}
		inline cmp_math_fn double4 operator - (double4 a, double b)
		{
			return double4_make(a.x - b, a.y - b, a.z - b, a.w - b);
		}
		inline cmp_math_fn double4 operator - (double a, double4 b)
		{
			return double4_make(a - b.x, a - b.y, a - b.z, a - b.w);
		}
		inline cmp_math_fn void operator -= (double4& a, double4 b)
		{
			a.x -= b.x; a.y -= b.y; a.z -= b.z; a.w -= b.w;
		}
		inline cmp_math_fn void operator -= (double4& a, double b)
		{
			a.x -= b; a.y -= b; a.z -= b; a.w -= b;
		}
		inline cmp_math_fn int4 operator - (int4 a, int4 b)
		{
			return int4_make(a.x - b.x, a.y - b.y, a.z - b.z, a.w - b.w);
		}
		inline cmp_math_fn int4 operator - (int4 a, int b)
		{
			return int4_make(a.x - b, a.y - b, a.z - b, a.w - b);
		}
		inline cmp_math_fn int4 operator - (int a, int4 b)
		{
			return int4_make(a - b.x, a - b.y, a - b.z, a - b.w);
		}
		inline cmp_math_fn void operator -= (int4& a, int4 b)
		{
			a.x -= b.x; a.y -= b.y; a.z -= b.z; a.w -= b.w;
		}
		inline cmp_math_fn void operator -= (int4& a, int b)
		{
			a.x -= b; a.y -= b; a.z -= b; a.w -= b;
		}
		inline cmp_math_fn uint4 operator - (uint4 a, uint4 b)
		{
			return uint4_make(a.x - b.x, a.y - b.y, a.z - b.z, a.w - b.w);
		}
		inline cmp_math_fn uint4 operator - (uint4 a, uint b)
		{
			return uint4_make(a.x - b, a.y - b, a.z - b, a.w - b);
		}
		inline cmp_math_fn uint4 operator - (uint a, uint4 b)
		{
			return uint4_make(a - b.x, a - b.y, a - b.z, a - b.w);
		}
		inline cmp_math_fn void operator -= (uint4& a, uint4 b)
		{
			a.x -= b.x; a.y -= b.y; a.z -= b.z; a.w -= b.w;
		}
		inline cmp_math_fn void operator -= (uint4& a, uint b)
		{
			a.x -= b; a.y -= b; a.z -= b; a.w -= b;
		}
		inline cmp_math_fn char4 operator - (char4 a, char4 b)
		{
			return char4_make(a.x - b.x, a.y - b.y, a.z - b.z, a.w - b.w);
		}
		inline cmp_math_fn char4 operator - (char4 a, char b)
		{
			return char4_make(a.x - b, a.y - b, a.z - b, a.w - b);
		}
		inline cmp_math_fn char4 operator - (char a, char4 b)
		{
			return char4_make(a - b.x, a - b.y, a - b.z, a - b.w);
		}
		inline cmp_math_fn void operator -= (char4& a, char4 b)
		{
			a.x -= b.x; a.y -= b.y; a.z -= b.z; a.w -= b.w;
		}
		inline cmp_math_fn void operator -= (char4& a, char b)
		{
			a.x -= b; a.y -= b; a.z -= b; a.w -= b;
		}
		inline cmp_math_fn uchar4 operator - (uchar4 a, uchar4 b)
		{
			return uchar4_make(a.x - b.x, a.y - b.y, a.z - b.z, a.w - b.w);
		}
		inline cmp_math_fn uchar4 operator - (uchar4 a, uchar b)
		{
			return uchar4_make(a.x - b, a.y - b, a.z - b, a.w - b);
		}
		inline cmp_math_fn uchar4 operator - (uchar a, uchar4 b)
		{
			return uchar4_make(a - b.x, a - b.y, a - b.z, a - b.w);
		}
		inline cmp_math_fn void operator -= (uchar4& a, uchar4 b)
		{
			a.x -= b.x; a.y -= b.y; a.z -= b.z; a.w -= b.w;
		}
		inline cmp_math_fn void operator -= (uchar4& a, uchar b)
		{
			a.x -= b; a.y -= b; a.z -= b; a.w -= b;
		}
		inline cmp_math_fn short4 operator - (short4 a, short4 b)
		{
			return short4_make(a.x - b.x, a.y - b.y, a.z - b.z, a.w - b.w);
		}
		inline cmp_math_fn short4 operator - (short4 a, short b)
		{
			return short4_make(a.x - b, a.y - b, a.z - b, a.w - b);
		}
		inline cmp_math_fn short4 operator - (short a, short4 b)
		{
			return short4_make(a - b.x, a - b.y, a - b.z, a - b.w);
		}
		inline cmp_math_fn void operator -= (short4& a, short4 b)
		{
			a.x -= b.x; a.y -= b.y; a.z -= b.z; a.w -= b.w;
		}
		inline cmp_math_fn void operator -= (short4& a, short b)
		{
			a.x -= b; a.y -= b; a.z -= b; a.w -= b;
		}
		inline cmp_math_fn ushort4 operator - (ushort4 a, ushort4 b)
		{
			return ushort4_make(a.x - b.x, a.y - b.y, a.z - b.z, a.w - b.w);
		}
		inline cmp_math_fn ushort4 operator - (ushort4 a, ushort b)
		{
			return ushort4_make(a.x - b, a.y - b, a.z - b, a.w - b);
		}
		inline cmp_math_fn ushort4 operator - (ushort a, ushort4 b)
		{
			return ushort4_make(a - b.x, a - b.y, a - b.z, a - b.w);
		}
		inline cmp_math_fn void operator -= (ushort4& a, ushort4 b)
		{
			a.x -= b.x; a.y -= b.y; a.z -= b.z; a.w -= b.w;
		}
		inline cmp_math_fn void operator -= (ushort4& a, ushort b)
		{
			a.x -= b; a.y -= b; a.z -= b; a.w -= b;
		}
		inline cmp_math_fn long4 operator - (long4 a, long4 b)
		{
			return long4_make(a.x - b.x, a.y - b.y, a.z - b.z, a.w - b.w);
		}
		inline cmp_math_fn long4 operator - (long4 a, long b)
		{
			return long4_make(a.x - b, a.y - b, a.z - b, a.w - b);
		}
		inline cmp_math_fn long4 operator - (long a, long4 b)
		{
			return long4_make(a - b.x, a - b.y, a - b.z, a - b.w);
		}
		inline cmp_math_fn void operator -= (long4& a, long4 b)
		{
			a.x -= b.x; a.y -= b.y; a.z -= b.z; a.w -= b.w;
		}
		inline cmp_math_fn void operator -= (long4& a, long b)
		{
			a.x -= b; a.y -= b; a.z -= b; a.w -= b;
		}
		inline cmp_math_fn ulong4 operator - (ulong4 a, ulong4 b)
		{
			return ulong4_make(a.x - b.x, a.y - b.y, a.z - b.z, a.w - b.w);
		}
		inline cmp_math_fn ulong4 operator - (ulong4 a, ulong b)
		{
			return ulong4_make(a.x - b, a.y - b, a.z - b, a.w - b);
		}
		inline cmp_math_fn ulong4 operator - (ulong a, ulong4 b)
		{
			return ulong4_make(a - b.x, a - b.y, a - b.z, a - b.w);
		}
		inline cmp_math_fn void operator -= (ulong4& a, ulong4 b)
		{
			a.x -= b.x; a.y -= b.y; a.z -= b.z; a.w -= b.w;
		}
		inline cmp_math_fn void operator -= (ulong4& a, ulong b)
		{
			a.x -= b; a.y -= b; a.z -= b; a.w -= b;
		}
	//$pycgen-end


	// Multiply operator -------------------------------------------------------------------------------
	/*$pycgen
		mul2 = """
		inline cmp_math_fn T2 operator * (T2 a, T2 b)
		{
			return T2_make(a.x * b.x, a.y * b.y);
		}
		inline cmp_math_fn T2 operator * (T2 a, T b)
		{
			return T2_make(a.x * b, a.y * b);
		}
		inline cmp_math_fn T2 operator * (T a, T2 b)
		{
			return b * a;
		}
		inline cmp_math_fn void operator *= (T2& a, T2 b)
		{
			a.x *= b.x; a.y *= b.y;
		}
		inline cmp_math_fn void operator *= (T2& a, T b)
		{
			a.x *= b; a.y *= b;
		}
		"""

		mul3 = """
		inline cmp_math_fn T3 operator * (T3 a, T3 b)
		{
			return T3_make(a.x * b.x, a.y * b.y, a.z * b.z);
		}
		inline cmp_math_fn T3 operator * (T3 a, T b)
		{
			return T3_make(a.x * b, a.y * b, a.z * b);
		}
		inline cmp_math_fn T3 operator * (T a, T3 b)
		{
			return b * a;
		}
		inline cmp_math_fn void operator *= (T3& a, T3 b)
		{
			a.x *= b.x; a.y *= b.y; a.z *= b.z;
		}
		inline cmp_math_fn void operator *= (T3& a, T b)
		{
			a.x *= b; a.y *= b; a.z *= b;
		}
		"""

		mul4 = """
		inline cmp_math_fn T4 operator * (T4 a, T4 b)
		{
			return T4_make(a.x * b.x, a.y * b.y, a.z * b.z, a.w * b.w);
		}
		inline cmp_math_fn T4 operator * (T4 a, T b)
		{
			return T4_make(a.x * b, a.y * b, a.z * b, a.w * b);
		}
		inline cmp_math_fn T4 operator * (T a, T4 b)
		{
			return b * a;
		}
		inline cmp_math_fn void operator *= (T4& a, T4 b)
		{
			a.x *= b.x; a.y *= b.y; a.z *= b.z; a.w *= b.w;
		}
		inline cmp_math_fn void operator *= (T4& a, T b)
		{
			a.x *= b; a.y *= b; a.z *= b; a.w *= b;
		}
		"""

		EmitRepl(mul2, "T:" + all_types)
		EmitRepl(mul3, "T:" + all_types)
		EmitRepl(mul4, "T:" + all_types)
	*/
	//$pycgen-begin
		inline cmp_math_fn float2 operator * (float2 a, float2 b)
		{
			return float2_make(a.x * b.x, a.y * b.y);
		}
		inline cmp_math_fn float2 operator * (float2 a, float b)
		{
			return float2_make(a.x * b, a.y * b);
		}
		inline cmp_math_fn float2 operator * (float a, float2 b)
		{
			return b * a;
		}
		inline cmp_math_fn void operator *= (float2& a, float2 b)
		{
			a.x *= b.x; a.y *= b.y;
		}
		inline cmp_math_fn void operator *= (float2& a, float b)
		{
			a.x *= b; a.y *= b;
		}
		inline cmp_math_fn double2 operator * (double2 a, double2 b)
		{
			return double2_make(a.x * b.x, a.y * b.y);
		}
		inline cmp_math_fn double2 operator * (double2 a, double b)
		{
			return double2_make(a.x * b, a.y * b);
		}
		inline cmp_math_fn double2 operator * (double a, double2 b)
		{
			return b * a;
		}
		inline cmp_math_fn void operator *= (double2& a, double2 b)
		{
			a.x *= b.x; a.y *= b.y;
		}
		inline cmp_math_fn void operator *= (double2& a, double b)
		{
			a.x *= b; a.y *= b;
		}
		inline cmp_math_fn int2 operator * (int2 a, int2 b)
		{
			return int2_make(a.x * b.x, a.y * b.y);
		}
		inline cmp_math_fn int2 operator * (int2 a, int b)
		{
			return int2_make(a.x * b, a.y * b);
		}
		inline cmp_math_fn int2 operator * (int a, int2 b)
		{
			return b * a;
		}
		inline cmp_math_fn void operator *= (int2& a, int2 b)
		{
			a.x *= b.x; a.y *= b.y;
		}
		inline cmp_math_fn void operator *= (int2& a, int b)
		{
			a.x *= b; a.y *= b;
		}
		inline cmp_math_fn uint2 operator * (uint2 a, uint2 b)
		{
			return uint2_make(a.x * b.x, a.y * b.y);
		}
		inline cmp_math_fn uint2 operator * (uint2 a, uint b)
		{
			return uint2_make(a.x * b, a.y * b);
		}
		inline cmp_math_fn uint2 operator * (uint a, uint2 b)
		{
			return b * a;
		}
		inline cmp_math_fn void operator *= (uint2& a, uint2 b)
		{
			a.x *= b.x; a.y *= b.y;
		}
		inline cmp_math_fn void operator *= (uint2& a, uint b)
		{
			a.x *= b; a.y *= b;
		}
		inline cmp_math_fn char2 operator * (char2 a, char2 b)
		{
			return char2_make(a.x * b.x, a.y * b.y);
		}
		inline cmp_math_fn char2 operator * (char2 a, char b)
		{
			return char2_make(a.x * b, a.y * b);
		}
		inline cmp_math_fn char2 operator * (char a, char2 b)
		{
			return b * a;
		}
		inline cmp_math_fn void operator *= (char2& a, char2 b)
		{
			a.x *= b.x; a.y *= b.y;
		}
		inline cmp_math_fn void operator *= (char2& a, char b)
		{
			a.x *= b; a.y *= b;
		}
		inline cmp_math_fn uchar2 operator * (uchar2 a, uchar2 b)
		{
			return uchar2_make(a.x * b.x, a.y * b.y);
		}
		inline cmp_math_fn uchar2 operator * (uchar2 a, uchar b)
		{
			return uchar2_make(a.x * b, a.y * b);
		}
		inline cmp_math_fn uchar2 operator * (uchar a, uchar2 b)
		{
			return b * a;
		}
		inline cmp_math_fn void operator *= (uchar2& a, uchar2 b)
		{
			a.x *= b.x; a.y *= b.y;
		}
		inline cmp_math_fn void operator *= (uchar2& a, uchar b)
		{
			a.x *= b; a.y *= b;
		}
		inline cmp_math_fn short2 operator * (short2 a, short2 b)
		{
			return short2_make(a.x * b.x, a.y * b.y);
		}
		inline cmp_math_fn short2 operator * (short2 a, short b)
		{
			return short2_make(a.x * b, a.y * b);
		}
		inline cmp_math_fn short2 operator * (short a, short2 b)
		{
			return b * a;
		}
		inline cmp_math_fn void operator *= (short2& a, short2 b)
		{
			a.x *= b.x; a.y *= b.y;
		}
		inline cmp_math_fn void operator *= (short2& a, short b)
		{
			a.x *= b; a.y *= b;
		}
		inline cmp_math_fn ushort2 operator * (ushort2 a, ushort2 b)
		{
			return ushort2_make(a.x * b.x, a.y * b.y);
		}
		inline cmp_math_fn ushort2 operator * (ushort2 a, ushort b)
		{
			return ushort2_make(a.x * b, a.y * b);
		}
		inline cmp_math_fn ushort2 operator * (ushort a, ushort2 b)
		{
			return b * a;
		}
		inline cmp_math_fn void operator *= (ushort2& a, ushort2 b)
		{
			a.x *= b.x; a.y *= b.y;
		}
		inline cmp_math_fn void operator *= (ushort2& a, ushort b)
		{
			a.x *= b; a.y *= b;
		}
		inline cmp_math_fn long2 operator * (long2 a, long2 b)
		{
			return long2_make(a.x * b.x, a.y * b.y);
		}
		inline cmp_math_fn long2 operator * (long2 a, long b)
		{
			return long2_make(a.x * b, a.y * b);
		}
		inline cmp_math_fn long2 operator * (long a, long2 b)
		{
			return b * a;
		}
		inline cmp_math_fn void operator *= (long2& a, long2 b)
		{
			a.x *= b.x; a.y *= b.y;
		}
		inline cmp_math_fn void operator *= (long2& a, long b)
		{
			a.x *= b; a.y *= b;
		}
		inline cmp_math_fn ulong2 operator * (ulong2 a, ulong2 b)
		{
			return ulong2_make(a.x * b.x, a.y * b.y);
		}
		inline cmp_math_fn ulong2 operator * (ulong2 a, ulong b)
		{
			return ulong2_make(a.x * b, a.y * b);
		}
		inline cmp_math_fn ulong2 operator * (ulong a, ulong2 b)
		{
			return b * a;
		}
		inline cmp_math_fn void operator *= (ulong2& a, ulong2 b)
		{
			a.x *= b.x; a.y *= b.y;
		}
		inline cmp_math_fn void operator *= (ulong2& a, ulong b)
		{
			a.x *= b; a.y *= b;
		}
		inline cmp_math_fn float3 operator * (float3 a, float3 b)
		{
			return float3_make(a.x * b.x, a.y * b.y, a.z * b.z);
		}
		inline cmp_math_fn float3 operator * (float3 a, float b)
		{
			return float3_make(a.x * b, a.y * b, a.z * b);
		}
		inline cmp_math_fn float3 operator * (float a, float3 b)
		{
			return b * a;
		}
		inline cmp_math_fn void operator *= (float3& a, float3 b)
		{
			a.x *= b.x; a.y *= b.y; a.z *= b.z;
		}
		inline cmp_math_fn void operator *= (float3& a, float b)
		{
			a.x *= b; a.y *= b; a.z *= b;
		}
		inline cmp_math_fn double3 operator * (double3 a, double3 b)
		{
			return double3_make(a.x * b.x, a.y * b.y, a.z * b.z);
		}
		inline cmp_math_fn double3 operator * (double3 a, double b)
		{
			return double3_make(a.x * b, a.y * b, a.z * b);
		}
		inline cmp_math_fn double3 operator * (double a, double3 b)
		{
			return b * a;
		}
		inline cmp_math_fn void operator *= (double3& a, double3 b)
		{
			a.x *= b.x; a.y *= b.y; a.z *= b.z;
		}
		inline cmp_math_fn void operator *= (double3& a, double b)
		{
			a.x *= b; a.y *= b; a.z *= b;
		}
		inline cmp_math_fn int3 operator * (int3 a, int3 b)
		{
			return int3_make(a.x * b.x, a.y * b.y, a.z * b.z);
		}
		inline cmp_math_fn int3 operator * (int3 a, int b)
		{
			return int3_make(a.x * b, a.y * b, a.z * b);
		}
		inline cmp_math_fn int3 operator * (int a, int3 b)
		{
			return b * a;
		}
		inline cmp_math_fn void operator *= (int3& a, int3 b)
		{
			a.x *= b.x; a.y *= b.y; a.z *= b.z;
		}
		inline cmp_math_fn void operator *= (int3& a, int b)
		{
			a.x *= b; a.y *= b; a.z *= b;
		}
		inline cmp_math_fn uint3 operator * (uint3 a, uint3 b)
		{
			return uint3_make(a.x * b.x, a.y * b.y, a.z * b.z);
		}
		inline cmp_math_fn uint3 operator * (uint3 a, uint b)
		{
			return uint3_make(a.x * b, a.y * b, a.z * b);
		}
		inline cmp_math_fn uint3 operator * (uint a, uint3 b)
		{
			return b * a;
		}
		inline cmp_math_fn void operator *= (uint3& a, uint3 b)
		{
			a.x *= b.x; a.y *= b.y; a.z *= b.z;
		}
		inline cmp_math_fn void operator *= (uint3& a, uint b)
		{
			a.x *= b; a.y *= b; a.z *= b;
		}
		inline cmp_math_fn char3 operator * (char3 a, char3 b)
		{
			return char3_make(a.x * b.x, a.y * b.y, a.z * b.z);
		}
		inline cmp_math_fn char3 operator * (char3 a, char b)
		{
			return char3_make(a.x * b, a.y * b, a.z * b);
		}
		inline cmp_math_fn char3 operator * (char a, char3 b)
		{
			return b * a;
		}
		inline cmp_math_fn void operator *= (char3& a, char3 b)
		{
			a.x *= b.x; a.y *= b.y; a.z *= b.z;
		}
		inline cmp_math_fn void operator *= (char3& a, char b)
		{
			a.x *= b; a.y *= b; a.z *= b;
		}
		inline cmp_math_fn uchar3 operator * (uchar3 a, uchar3 b)
		{
			return uchar3_make(a.x * b.x, a.y * b.y, a.z * b.z);
		}
		inline cmp_math_fn uchar3 operator * (uchar3 a, uchar b)
		{
			return uchar3_make(a.x * b, a.y * b, a.z * b);
		}
		inline cmp_math_fn uchar3 operator * (uchar a, uchar3 b)
		{
			return b * a;
		}
		inline cmp_math_fn void operator *= (uchar3& a, uchar3 b)
		{
			a.x *= b.x; a.y *= b.y; a.z *= b.z;
		}
		inline cmp_math_fn void operator *= (uchar3& a, uchar b)
		{
			a.x *= b; a.y *= b; a.z *= b;
		}
		inline cmp_math_fn short3 operator * (short3 a, short3 b)
		{
			return short3_make(a.x * b.x, a.y * b.y, a.z * b.z);
		}
		inline cmp_math_fn short3 operator * (short3 a, short b)
		{
			return short3_make(a.x * b, a.y * b, a.z * b);
		}
		inline cmp_math_fn short3 operator * (short a, short3 b)
		{
			return b * a;
		}
		inline cmp_math_fn void operator *= (short3& a, short3 b)
		{
			a.x *= b.x; a.y *= b.y; a.z *= b.z;
		}
		inline cmp_math_fn void operator *= (short3& a, short b)
		{
			a.x *= b; a.y *= b; a.z *= b;
		}
		inline cmp_math_fn ushort3 operator * (ushort3 a, ushort3 b)
		{
			return ushort3_make(a.x * b.x, a.y * b.y, a.z * b.z);
		}
		inline cmp_math_fn ushort3 operator * (ushort3 a, ushort b)
		{
			return ushort3_make(a.x * b, a.y * b, a.z * b);
		}
		inline cmp_math_fn ushort3 operator * (ushort a, ushort3 b)
		{
			return b * a;
		}
		inline cmp_math_fn void operator *= (ushort3& a, ushort3 b)
		{
			a.x *= b.x; a.y *= b.y; a.z *= b.z;
		}
		inline cmp_math_fn void operator *= (ushort3& a, ushort b)
		{
			a.x *= b; a.y *= b; a.z *= b;
		}
		inline cmp_math_fn long3 operator * (long3 a, long3 b)
		{
			return long3_make(a.x * b.x, a.y * b.y, a.z * b.z);
		}
		inline cmp_math_fn long3 operator * (long3 a, long b)
		{
			return long3_make(a.x * b, a.y * b, a.z * b);
		}
		inline cmp_math_fn long3 operator * (long a, long3 b)
		{
			return b * a;
		}
		inline cmp_math_fn void operator *= (long3& a, long3 b)
		{
			a.x *= b.x; a.y *= b.y; a.z *= b.z;
		}
		inline cmp_math_fn void operator *= (long3& a, long b)
		{
			a.x *= b; a.y *= b; a.z *= b;
		}
		inline cmp_math_fn ulong3 operator * (ulong3 a, ulong3 b)
		{
			return ulong3_make(a.x * b.x, a.y * b.y, a.z * b.z);
		}
		inline cmp_math_fn ulong3 operator * (ulong3 a, ulong b)
		{
			return ulong3_make(a.x * b, a.y * b, a.z * b);
		}
		inline cmp_math_fn ulong3 operator * (ulong a, ulong3 b)
		{
			return b * a;
		}
		inline cmp_math_fn void operator *= (ulong3& a, ulong3 b)
		{
			a.x *= b.x; a.y *= b.y; a.z *= b.z;
		}
		inline cmp_math_fn void operator *= (ulong3& a, ulong b)
		{
			a.x *= b; a.y *= b; a.z *= b;
		}
		inline cmp_math_fn float4 operator * (float4 a, float4 b)
		{
			return float4_make(a.x * b.x, a.y * b.y, a.z * b.z, a.w * b.w);
		}
		inline cmp_math_fn float4 operator * (float4 a, float b)
		{
			return float4_make(a.x * b, a.y * b, a.z * b, a.w * b);
		}
		inline cmp_math_fn float4 operator * (float a, float4 b)
		{
			return b * a;
		}
		inline cmp_math_fn void operator *= (float4& a, float4 b)
		{
			a.x *= b.x; a.y *= b.y; a.z *= b.z; a.w *= b.w;
		}
		inline cmp_math_fn void operator *= (float4& a, float b)
		{
			a.x *= b; a.y *= b; a.z *= b; a.w *= b;
		}
		inline cmp_math_fn double4 operator * (double4 a, double4 b)
		{
			return double4_make(a.x * b.x, a.y * b.y, a.z * b.z, a.w * b.w);
		}
		inline cmp_math_fn double4 operator * (double4 a, double b)
		{
			return double4_make(a.x * b, a.y * b, a.z * b, a.w * b);
		}
		inline cmp_math_fn double4 operator * (double a, double4 b)
		{
			return b * a;
		}
		inline cmp_math_fn void operator *= (double4& a, double4 b)
		{
			a.x *= b.x; a.y *= b.y; a.z *= b.z; a.w *= b.w;
		}
		inline cmp_math_fn void operator *= (double4& a, double b)
		{
			a.x *= b; a.y *= b; a.z *= b; a.w *= b;
		}
		inline cmp_math_fn int4 operator * (int4 a, int4 b)
		{
			return int4_make(a.x * b.x, a.y * b.y, a.z * b.z, a.w * b.w);
		}
		inline cmp_math_fn int4 operator * (int4 a, int b)
		{
			return int4_make(a.x * b, a.y * b, a.z * b, a.w * b);
		}
		inline cmp_math_fn int4 operator * (int a, int4 b)
		{
			return b * a;
		}
		inline cmp_math_fn void operator *= (int4& a, int4 b)
		{
			a.x *= b.x; a.y *= b.y; a.z *= b.z; a.w *= b.w;
		}
		inline cmp_math_fn void operator *= (int4& a, int b)
		{
			a.x *= b; a.y *= b; a.z *= b; a.w *= b;
		}
		inline cmp_math_fn uint4 operator * (uint4 a, uint4 b)
		{
			return uint4_make(a.x * b.x, a.y * b.y, a.z * b.z, a.w * b.w);
		}
		inline cmp_math_fn uint4 operator * (uint4 a, uint b)
		{
			return uint4_make(a.x * b, a.y * b, a.z * b, a.w * b);
		}
		inline cmp_math_fn uint4 operator * (uint a, uint4 b)
		{
			return b * a;
		}
		inline cmp_math_fn void operator *= (uint4& a, uint4 b)
		{
			a.x *= b.x; a.y *= b.y; a.z *= b.z; a.w *= b.w;
		}
		inline cmp_math_fn void operator *= (uint4& a, uint b)
		{
			a.x *= b; a.y *= b; a.z *= b; a.w *= b;
		}
		inline cmp_math_fn char4 operator * (char4 a, char4 b)
		{
			return char4_make(a.x * b.x, a.y * b.y, a.z * b.z, a.w * b.w);
		}
		inline cmp_math_fn char4 operator * (char4 a, char b)
		{
			return char4_make(a.x * b, a.y * b, a.z * b, a.w * b);
		}
		inline cmp_math_fn char4 operator * (char a, char4 b)
		{
			return b * a;
		}
		inline cmp_math_fn void operator *= (char4& a, char4 b)
		{
			a.x *= b.x; a.y *= b.y; a.z *= b.z; a.w *= b.w;
		}
		inline cmp_math_fn void operator *= (char4& a, char b)
		{
			a.x *= b; a.y *= b; a.z *= b; a.w *= b;
		}
		inline cmp_math_fn uchar4 operator * (uchar4 a, uchar4 b)
		{
			return uchar4_make(a.x * b.x, a.y * b.y, a.z * b.z, a.w * b.w);
		}
		inline cmp_math_fn uchar4 operator * (uchar4 a, uchar b)
		{
			return uchar4_make(a.x * b, a.y * b, a.z * b, a.w * b);
		}
		inline cmp_math_fn uchar4 operator * (uchar a, uchar4 b)
		{
			return b * a;
		}
		inline cmp_math_fn void operator *= (uchar4& a, uchar4 b)
		{
			a.x *= b.x; a.y *= b.y; a.z *= b.z; a.w *= b.w;
		}
		inline cmp_math_fn void operator *= (uchar4& a, uchar b)
		{
			a.x *= b; a.y *= b; a.z *= b; a.w *= b;
		}
		inline cmp_math_fn short4 operator * (short4 a, short4 b)
		{
			return short4_make(a.x * b.x, a.y * b.y, a.z * b.z, a.w * b.w);
		}
		inline cmp_math_fn short4 operator * (short4 a, short b)
		{
			return short4_make(a.x * b, a.y * b, a.z * b, a.w * b);
		}
		inline cmp_math_fn short4 operator * (short a, short4 b)
		{
			return b * a;
		}
		inline cmp_math_fn void operator *= (short4& a, short4 b)
		{
			a.x *= b.x; a.y *= b.y; a.z *= b.z; a.w *= b.w;
		}
		inline cmp_math_fn void operator *= (short4& a, short b)
		{
			a.x *= b; a.y *= b; a.z *= b; a.w *= b;
		}
		inline cmp_math_fn ushort4 operator * (ushort4 a, ushort4 b)
		{
			return ushort4_make(a.x * b.x, a.y * b.y, a.z * b.z, a.w * b.w);
		}
		inline cmp_math_fn ushort4 operator * (ushort4 a, ushort b)
		{
			return ushort4_make(a.x * b, a.y * b, a.z * b, a.w * b);
		}
		inline cmp_math_fn ushort4 operator * (ushort a, ushort4 b)
		{
			return b * a;
		}
		inline cmp_math_fn void operator *= (ushort4& a, ushort4 b)
		{
			a.x *= b.x; a.y *= b.y; a.z *= b.z; a.w *= b.w;
		}
		inline cmp_math_fn void operator *= (ushort4& a, ushort b)
		{
			a.x *= b; a.y *= b; a.z *= b; a.w *= b;
		}
		inline cmp_math_fn long4 operator * (long4 a, long4 b)
		{
			return long4_make(a.x * b.x, a.y * b.y, a.z * b.z, a.w * b.w);
		}
		inline cmp_math_fn long4 operator * (long4 a, long b)
		{
			return long4_make(a.x * b, a.y * b, a.z * b, a.w * b);
		}
		inline cmp_math_fn long4 operator * (long a, long4 b)
		{
			return b * a;
		}
		inline cmp_math_fn void operator *= (long4& a, long4 b)
		{
			a.x *= b.x; a.y *= b.y; a.z *= b.z; a.w *= b.w;
		}
		inline cmp_math_fn void operator *= (long4& a, long b)
		{
			a.x *= b; a.y *= b; a.z *= b; a.w *= b;
		}
		inline cmp_math_fn ulong4 operator * (ulong4 a, ulong4 b)
		{
			return ulong4_make(a.x * b.x, a.y * b.y, a.z * b.z, a.w * b.w);
		}
		inline cmp_math_fn ulong4 operator * (ulong4 a, ulong b)
		{
			return ulong4_make(a.x * b, a.y * b, a.z * b, a.w * b);
		}
		inline cmp_math_fn ulong4 operator * (ulong a, ulong4 b)
		{
			return b * a;
		}
		inline cmp_math_fn void operator *= (ulong4& a, ulong4 b)
		{
			a.x *= b.x; a.y *= b.y; a.z *= b.z; a.w *= b.w;
		}
		inline cmp_math_fn void operator *= (ulong4& a, ulong b)
		{
			a.x *= b; a.y *= b; a.z *= b; a.w *= b;
		}
	//$pycgen-end


	// Dot product -------------------------------------------------------------------------------------
	/*$pycgen
		dot2 = """
		inline cmp_math_fn T dot(T2 a, T2 b)
		{
			return a.x * b.x + a.y * b.y;
		}
		"""

		dot3 ="""
		inline cmp_math_fn T dot(T3 a, T3 b)
		{
			return a.x * b.x + a.y * b.y + a.z * b.z;
		}
		"""

		dot4 = """
		inline cmp_math_fn T dot(T4 a, T4 b)
		{
			return a.x * b.x + a.y * b.y + a.z * b.z + a.w * b.w;
		}
		"""

		EmitRepl(dot2, "T:" + all_types)
		EmitRepl(dot3, "T:" + all_types)
		EmitRepl(dot4, "T:" + all_types)
	*/
	//$pycgen-begin
		inline cmp_math_fn float dot(float2 a, float2 b)
		{
			return a.x * b.x + a.y * b.y;
		}
		inline cmp_math_fn double dot(double2 a, double2 b)
		{
			return a.x * b.x + a.y * b.y;
		}
		inline cmp_math_fn int dot(int2 a, int2 b)
		{
			return a.x * b.x + a.y * b.y;
		}
		inline cmp_math_fn uint dot(uint2 a, uint2 b)
		{
			return a.x * b.x + a.y * b.y;
		}
		inline cmp_math_fn char dot(char2 a, char2 b)
		{
			return a.x * b.x + a.y * b.y;
		}
		inline cmp_math_fn uchar dot(uchar2 a, uchar2 b)
		{
			return a.x * b.x + a.y * b.y;
		}
		inline cmp_math_fn short dot(short2 a, short2 b)
		{
			return a.x * b.x + a.y * b.y;
		}
		inline cmp_math_fn ushort dot(ushort2 a, ushort2 b)
		{
			return a.x * b.x + a.y * b.y;
		}
		inline cmp_math_fn long dot(long2 a, long2 b)
		{
			return a.x * b.x + a.y * b.y;
		}
		inline cmp_math_fn ulong dot(ulong2 a, ulong2 b)
		{
			return a.x * b.x + a.y * b.y;
		}
		inline cmp_math_fn float dot(float3 a, float3 b)
		{
			return a.x * b.x + a.y * b.y + a.z * b.z;
		}
		inline cmp_math_fn double dot(double3 a, double3 b)
		{
			return a.x * b.x + a.y * b.y + a.z * b.z;
		}
		inline cmp_math_fn int dot(int3 a, int3 b)
		{
			return a.x * b.x + a.y * b.y + a.z * b.z;
		}
		inline cmp_math_fn uint dot(uint3 a, uint3 b)
		{
			return a.x * b.x + a.y * b.y + a.z * b.z;
		}
		inline cmp_math_fn char dot(char3 a, char3 b)
		{
			return a.x * b.x + a.y * b.y + a.z * b.z;
		}
		inline cmp_math_fn uchar dot(uchar3 a, uchar3 b)
		{
			return a.x * b.x + a.y * b.y + a.z * b.z;
		}
		inline cmp_math_fn short dot(short3 a, short3 b)
		{
			return a.x * b.x + a.y * b.y + a.z * b.z;
		}
		inline cmp_math_fn ushort dot(ushort3 a, ushort3 b)
		{
			return a.x * b.x + a.y * b.y + a.z * b.z;
		}
		inline cmp_math_fn long dot(long3 a, long3 b)
		{
			return a.x * b.x + a.y * b.y + a.z * b.z;
		}
		inline cmp_math_fn ulong dot(ulong3 a, ulong3 b)
		{
			return a.x * b.x + a.y * b.y + a.z * b.z;
		}
		inline cmp_math_fn float dot(float4 a, float4 b)
		{
			return a.x * b.x + a.y * b.y + a.z * b.z + a.w * b.w;
		}
		inline cmp_math_fn double dot(double4 a, double4 b)
		{
			return a.x * b.x + a.y * b.y + a.z * b.z + a.w * b.w;
		}
		inline cmp_math_fn int dot(int4 a, int4 b)
		{
			return a.x * b.x + a.y * b.y + a.z * b.z + a.w * b.w;
		}
		inline cmp_math_fn uint dot(uint4 a, uint4 b)
		{
			return a.x * b.x + a.y * b.y + a.z * b.z + a.w * b.w;
		}
		inline cmp_math_fn char dot(char4 a, char4 b)
		{
			return a.x * b.x + a.y * b.y + a.z * b.z + a.w * b.w;
		}
		inline cmp_math_fn uchar dot(uchar4 a, uchar4 b)
		{
			return a.x * b.x + a.y * b.y + a.z * b.z + a.w * b.w;
		}
		inline cmp_math_fn short dot(short4 a, short4 b)
		{
			return a.x * b.x + a.y * b.y + a.z * b.z + a.w * b.w;
		}
		inline cmp_math_fn ushort dot(ushort4 a, ushort4 b)
		{
			return a.x * b.x + a.y * b.y + a.z * b.z + a.w * b.w;
		}
		inline cmp_math_fn long dot(long4 a, long4 b)
		{
			return a.x * b.x + a.y * b.y + a.z * b.z + a.w * b.w;
		}
		inline cmp_math_fn ulong dot(ulong4 a, ulong4 b)
		{
			return a.x * b.x + a.y * b.y + a.z * b.z + a.w * b.w;
		}
	//$pycgen-end


	// Length ------------------------------------------------------------------------------------------
	inline cmp_math_fn float length(float2 v)
	{
		return sqrtf(dot(v, v));
	}
	inline cmp_math_fn float length(float3 v)
	{
		return sqrtf(dot(v, v));
	}
	inline cmp_math_fn float length(float4 v)
	{
		return sqrtf(dot(v, v));
	}
	inline cmp_math_fn double length(double2 v)
	{
		return sqrt(dot(v, v));
	}
	inline cmp_math_fn double length(double3 v)
	{
		return sqrt(dot(v, v));
	}
	inline cmp_math_fn double length(double4 v)
	{
		return sqrt(dot(v, v));
	}


	// Floor -------------------------------------------------------------------------------------------
	inline cmp_math_fn float2 floorf(float2 v)
	{
		return float2_make(floorf(v.x), floorf(v.y));
	}
	inline cmp_math_fn float3 floorf(float3 v)
	{
		return float3_make(floorf(v.x), floorf(v.y), floorf(v.z));
	}
	inline cmp_math_fn float4 floorf(float4 v)
	{
		return float4_make(floorf(v.x), floorf(v.y), floorf(v.z), floorf(v.w));
	}
	inline cmp_math_fn double2 floor(double2 v)
	{
		return double2_make(floor(v.x), floor(v.y));
	}
	inline cmp_math_fn double3 floor(double3 v)
	{
		return double3_make(floor(v.x), floor(v.y), floor(v.z));
	}
	inline cmp_math_fn double4 floor(double4 v)
	{
		return double4_make(floor(v.x), floor(v.y), floor(v.z), floor(v.w));
	}


	// Normalise ---------------------------------------------------------------------------------------
	inline cmp_math_fn float2 normalize(float2 v)
	{
		return rsqrtf(dot(v, v)) * v;
	}
	inline cmp_math_fn float3 normalize(float3 v)
	{
		return rsqrtf(dot(v, v)) * v;
	}
	inline cmp_math_fn float4 normalize(float4 v)
	{
		return rsqrtf(dot(v, v)) * v;
	}
	inline cmp_math_fn double2 normalize(double2 v)
	{
		return rsqrt(dot(v, v)) * v;
	}
	inline cmp_math_fn double3 normalize(double3 v)
	{
		return rsqrt(dot(v, v)) * v;
	}
	inline cmp_math_fn double4 normalize(double4 v)
	{
		return rsqrt(dot(v, v)) * v;
	}


	// Min ---------------------------------------------------------------------------------------------
	/*$pycgen
		min = """
		inline cmp_math_fn T min(T a, T b)
		{
			return a < b ? a : b;
		}
		"""

		min2 = """
		inline cmp_math_fn T2 min(T2 a, T2 b)
		{
			return T2_make(min(a.x, b.x), min(a.y, b.y));
		}
		"""

		min3 ="""
		inline cmp_math_fn T3 min(T3 a, T3 b)
		{
			return T3_make(min(a.x, b.x), min(a.y, b.y), min(a.z, b.z));
		}
		"""

		min4 = """
		inline cmp_math_fn T4 min(T4 a, T4 b)
		{
			return T4_make(min(a.x, b.x), min(a.y, b.y), min(a.z, b.z), min(a.w, b.w));
		}
		"""

		EmitLn("#ifndef CMP_CUDA")
		EmitRepl(min, "T:" + "float,double,uint")
		EmitLn("#endif")
		EmitRepl(min, "T:" + "int,char,uchar,short,ushort,long,ulong")

		EmitRepl(min2, "T:" + all_types)
		EmitRepl(min3, "T:" + all_types)
		EmitRepl(min4, "T:" + all_types)
	*/
	//$pycgen-begin
		#ifndef CMP_CUDA
		inline cmp_math_fn float min(float a, float b)
		{
			return a < b ? a : b;
		}
		inline cmp_math_fn double min(double a, double b)
		{
			return a < b ? a : b;
		}
		inline cmp_math_fn uint min(uint a, uint b)
		{
			return a < b ? a : b;
		}
		#endif
		inline cmp_math_fn int min(int a, int b)
		{
			return a < b ? a : b;
		}
		inline cmp_math_fn char min(char a, char b)
		{
			return a < b ? a : b;
		}
		inline cmp_math_fn uchar min(uchar a, uchar b)
		{
			return a < b ? a : b;
		}
		inline cmp_math_fn short min(short a, short b)
		{
			return a < b ? a : b;
		}
		inline cmp_math_fn ushort min(ushort a, ushort b)
		{
			return a < b ? a : b;
		}
		inline cmp_math_fn long min(long a, long b)
		{
			return a < b ? a : b;
		}
		inline cmp_math_fn ulong min(ulong a, ulong b)
		{
			return a < b ? a : b;
		}
		inline cmp_math_fn float2 min(float2 a, float2 b)
		{
			return float2_make(min(a.x, b.x), min(a.y, b.y));
		}
		inline cmp_math_fn double2 min(double2 a, double2 b)
		{
			return double2_make(min(a.x, b.x), min(a.y, b.y));
		}
		inline cmp_math_fn int2 min(int2 a, int2 b)
		{
			return int2_make(min(a.x, b.x), min(a.y, b.y));
		}
		inline cmp_math_fn uint2 min(uint2 a, uint2 b)
		{
			return uint2_make(min(a.x, b.x), min(a.y, b.y));
		}
		inline cmp_math_fn char2 min(char2 a, char2 b)
		{
			return char2_make(min(a.x, b.x), min(a.y, b.y));
		}
		inline cmp_math_fn uchar2 min(uchar2 a, uchar2 b)
		{
			return uchar2_make(min(a.x, b.x), min(a.y, b.y));
		}
		inline cmp_math_fn short2 min(short2 a, short2 b)
		{
			return short2_make(min(a.x, b.x), min(a.y, b.y));
		}
		inline cmp_math_fn ushort2 min(ushort2 a, ushort2 b)
		{
			return ushort2_make(min(a.x, b.x), min(a.y, b.y));
		}
		inline cmp_math_fn long2 min(long2 a, long2 b)
		{
			return long2_make(min(a.x, b.x), min(a.y, b.y));
		}
		inline cmp_math_fn ulong2 min(ulong2 a, ulong2 b)
		{
			return ulong2_make(min(a.x, b.x), min(a.y, b.y));
		}
		inline cmp_math_fn float3 min(float3 a, float3 b)
		{
			return float3_make(min(a.x, b.x), min(a.y, b.y), min(a.z, b.z));
		}
		inline cmp_math_fn double3 min(double3 a, double3 b)
		{
			return double3_make(min(a.x, b.x), min(a.y, b.y), min(a.z, b.z));
		}
		inline cmp_math_fn int3 min(int3 a, int3 b)
		{
			return int3_make(min(a.x, b.x), min(a.y, b.y), min(a.z, b.z));
		}
		inline cmp_math_fn uint3 min(uint3 a, uint3 b)
		{
			return uint3_make(min(a.x, b.x), min(a.y, b.y), min(a.z, b.z));
		}
		inline cmp_math_fn char3 min(char3 a, char3 b)
		{
			return char3_make(min(a.x, b.x), min(a.y, b.y), min(a.z, b.z));
		}
		inline cmp_math_fn uchar3 min(uchar3 a, uchar3 b)
		{
			return uchar3_make(min(a.x, b.x), min(a.y, b.y), min(a.z, b.z));
		}
		inline cmp_math_fn short3 min(short3 a, short3 b)
		{
			return short3_make(min(a.x, b.x), min(a.y, b.y), min(a.z, b.z));
		}
		inline cmp_math_fn ushort3 min(ushort3 a, ushort3 b)
		{
			return ushort3_make(min(a.x, b.x), min(a.y, b.y), min(a.z, b.z));
		}
		inline cmp_math_fn long3 min(long3 a, long3 b)
		{
			return long3_make(min(a.x, b.x), min(a.y, b.y), min(a.z, b.z));
		}
		inline cmp_math_fn ulong3 min(ulong3 a, ulong3 b)
		{
			return ulong3_make(min(a.x, b.x), min(a.y, b.y), min(a.z, b.z));
		}
		inline cmp_math_fn float4 min(float4 a, float4 b)
		{
			return float4_make(min(a.x, b.x), min(a.y, b.y), min(a.z, b.z), min(a.w, b.w));
		}
		inline cmp_math_fn double4 min(double4 a, double4 b)
		{
			return double4_make(min(a.x, b.x), min(a.y, b.y), min(a.z, b.z), min(a.w, b.w));
		}
		inline cmp_math_fn int4 min(int4 a, int4 b)
		{
			return int4_make(min(a.x, b.x), min(a.y, b.y), min(a.z, b.z), min(a.w, b.w));
		}
		inline cmp_math_fn uint4 min(uint4 a, uint4 b)
		{
			return uint4_make(min(a.x, b.x), min(a.y, b.y), min(a.z, b.z), min(a.w, b.w));
		}
		inline cmp_math_fn char4 min(char4 a, char4 b)
		{
			return char4_make(min(a.x, b.x), min(a.y, b.y), min(a.z, b.z), min(a.w, b.w));
		}
		inline cmp_math_fn uchar4 min(uchar4 a, uchar4 b)
		{
			return uchar4_make(min(a.x, b.x), min(a.y, b.y), min(a.z, b.z), min(a.w, b.w));
		}
		inline cmp_math_fn short4 min(short4 a, short4 b)
		{
			return short4_make(min(a.x, b.x), min(a.y, b.y), min(a.z, b.z), min(a.w, b.w));
		}
		inline cmp_math_fn ushort4 min(ushort4 a, ushort4 b)
		{
			return ushort4_make(min(a.x, b.x), min(a.y, b.y), min(a.z, b.z), min(a.w, b.w));
		}
		inline cmp_math_fn long4 min(long4 a, long4 b)
		{
			return long4_make(min(a.x, b.x), min(a.y, b.y), min(a.z, b.z), min(a.w, b.w));
		}
		inline cmp_math_fn ulong4 min(ulong4 a, ulong4 b)
		{
			return ulong4_make(min(a.x, b.x), min(a.y, b.y), min(a.z, b.z), min(a.w, b.w));
		}
	//$pycgen-end


	// Max ---------------------------------------------------------------------------------------------
	/*$pycgen
		max = """
		inline cmp_math_fn T max(T a, T b)
		{
			return a > b ? a : b;
		}
		"""

		max2 = """
		inline cmp_math_fn T2 max(T2 a, T2 b)
		{
			return T2_make(max(a.x, b.x), max(a.y, b.y));
		}
		"""

		max3 ="""
		inline cmp_math_fn T3 max(T3 a, T3 b)
		{
			return T3_make(max(a.x, b.x), max(a.y, b.y), max(a.z, b.z));
		}
		"""

		max4 = """
		inline cmp_math_fn T4 max(T4 a, T4 b)
		{
			return T4_make(max(a.x, b.x), max(a.y, b.y), max(a.z, b.z), max(a.w, b.w));
		}
		"""

		EmitLn("#ifndef CMP_CUDA")
		EmitRepl(max, "T:" + "float,double,uint")
		EmitLn("#endif")
		EmitRepl(max, "T:" + "int,char,uchar,short,ushort,long,ulong")

		EmitRepl(max2, "T:" + all_types)
		EmitRepl(max3, "T:" + all_types)
		EmitRepl(max4, "T:" + all_types)
	*/
	//$pycgen-begin
		#ifndef CMP_CUDA
		inline cmp_math_fn float max(float a, float b)
		{
			return a > b ? a : b;
		}
		inline cmp_math_fn double max(double a, double b)
		{
			return a > b ? a : b;
		}
		inline cmp_math_fn uint max(uint a, uint b)
		{
			return a > b ? a : b;
		}
		#endif
		inline cmp_math_fn int max(int a, int b)
		{
			return a > b ? a : b;
		}
		inline cmp_math_fn char max(char a, char b)
		{
			return a > b ? a : b;
		}
		inline cmp_math_fn uchar max(uchar a, uchar b)
		{
			return a > b ? a : b;
		}
		inline cmp_math_fn short max(short a, short b)
		{
			return a > b ? a : b;
		}
		inline cmp_math_fn ushort max(ushort a, ushort b)
		{
			return a > b ? a : b;
		}
		inline cmp_math_fn long max(long a, long b)
		{
			return a > b ? a : b;
		}
		inline cmp_math_fn ulong max(ulong a, ulong b)
		{
			return a > b ? a : b;
		}
		inline cmp_math_fn float2 max(float2 a, float2 b)
		{
			return float2_make(max(a.x, b.x), max(a.y, b.y));
		}
		inline cmp_math_fn double2 max(double2 a, double2 b)
		{
			return double2_make(max(a.x, b.x), max(a.y, b.y));
		}
		inline cmp_math_fn int2 max(int2 a, int2 b)
		{
			return int2_make(max(a.x, b.x), max(a.y, b.y));
		}
		inline cmp_math_fn uint2 max(uint2 a, uint2 b)
		{
			return uint2_make(max(a.x, b.x), max(a.y, b.y));
		}
		inline cmp_math_fn char2 max(char2 a, char2 b)
		{
			return char2_make(max(a.x, b.x), max(a.y, b.y));
		}
		inline cmp_math_fn uchar2 max(uchar2 a, uchar2 b)
		{
			return uchar2_make(max(a.x, b.x), max(a.y, b.y));
		}
		inline cmp_math_fn short2 max(short2 a, short2 b)
		{
			return short2_make(max(a.x, b.x), max(a.y, b.y));
		}
		inline cmp_math_fn ushort2 max(ushort2 a, ushort2 b)
		{
			return ushort2_make(max(a.x, b.x), max(a.y, b.y));
		}
		inline cmp_math_fn long2 max(long2 a, long2 b)
		{
			return long2_make(max(a.x, b.x), max(a.y, b.y));
		}
		inline cmp_math_fn ulong2 max(ulong2 a, ulong2 b)
		{
			return ulong2_make(max(a.x, b.x), max(a.y, b.y));
		}
		inline cmp_math_fn float3 max(float3 a, float3 b)
		{
			return float3_make(max(a.x, b.x), max(a.y, b.y), max(a.z, b.z));
		}
		inline cmp_math_fn double3 max(double3 a, double3 b)
		{
			return double3_make(max(a.x, b.x), max(a.y, b.y), max(a.z, b.z));
		}
		inline cmp_math_fn int3 max(int3 a, int3 b)
		{
			return int3_make(max(a.x, b.x), max(a.y, b.y), max(a.z, b.z));
		}
		inline cmp_math_fn uint3 max(uint3 a, uint3 b)
		{
			return uint3_make(max(a.x, b.x), max(a.y, b.y), max(a.z, b.z));
		}
		inline cmp_math_fn char3 max(char3 a, char3 b)
		{
			return char3_make(max(a.x, b.x), max(a.y, b.y), max(a.z, b.z));
		}
		inline cmp_math_fn uchar3 max(uchar3 a, uchar3 b)
		{
			return uchar3_make(max(a.x, b.x), max(a.y, b.y), max(a.z, b.z));
		}
		inline cmp_math_fn short3 max(short3 a, short3 b)
		{
			return short3_make(max(a.x, b.x), max(a.y, b.y), max(a.z, b.z));
		}
		inline cmp_math_fn ushort3 max(ushort3 a, ushort3 b)
		{
			return ushort3_make(max(a.x, b.x), max(a.y, b.y), max(a.z, b.z));
		}
		inline cmp_math_fn long3 max(long3 a, long3 b)
		{
			return long3_make(max(a.x, b.x), max(a.y, b.y), max(a.z, b.z));
		}
		inline cmp_math_fn ulong3 max(ulong3 a, ulong3 b)
		{
			return ulong3_make(max(a.x, b.x), max(a.y, b.y), max(a.z, b.z));
		}
		inline cmp_math_fn float4 max(float4 a, float4 b)
		{
			return float4_make(max(a.x, b.x), max(a.y, b.y), max(a.z, b.z), max(a.w, b.w));
		}
		inline cmp_math_fn double4 max(double4 a, double4 b)
		{
			return double4_make(max(a.x, b.x), max(a.y, b.y), max(a.z, b.z), max(a.w, b.w));
		}
		inline cmp_math_fn int4 max(int4 a, int4 b)
		{
			return int4_make(max(a.x, b.x), max(a.y, b.y), max(a.z, b.z), max(a.w, b.w));
		}
		inline cmp_math_fn uint4 max(uint4 a, uint4 b)
		{
			return uint4_make(max(a.x, b.x), max(a.y, b.y), max(a.z, b.z), max(a.w, b.w));
		}
		inline cmp_math_fn char4 max(char4 a, char4 b)
		{
			return char4_make(max(a.x, b.x), max(a.y, b.y), max(a.z, b.z), max(a.w, b.w));
		}
		inline cmp_math_fn uchar4 max(uchar4 a, uchar4 b)
		{
			return uchar4_make(max(a.x, b.x), max(a.y, b.y), max(a.z, b.z), max(a.w, b.w));
		}
		inline cmp_math_fn short4 max(short4 a, short4 b)
		{
			return short4_make(max(a.x, b.x), max(a.y, b.y), max(a.z, b.z), max(a.w, b.w));
		}
		inline cmp_math_fn ushort4 max(ushort4 a, ushort4 b)
		{
			return ushort4_make(max(a.x, b.x), max(a.y, b.y), max(a.z, b.z), max(a.w, b.w));
		}
		inline cmp_math_fn long4 max(long4 a, long4 b)
		{
			return long4_make(max(a.x, b.x), max(a.y, b.y), max(a.z, b.z), max(a.w, b.w));
		}
		inline cmp_math_fn ulong4 max(ulong4 a, ulong4 b)
		{
			return ulong4_make(max(a.x, b.x), max(a.y, b.y), max(a.z, b.z), max(a.w, b.w));
		}
	//$pycgen-end
#endif


#ifdef CMP_OPENCL
	#define floorf floor
#endif


#endif	// Prologue check


#endif	// INCLUDED_CBPP_MATH_H
