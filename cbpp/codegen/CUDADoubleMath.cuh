
#ifndef INCLUDED_CUDA_DOUBLE_MATH_H
#define INCLUDED_CUDA_DOUBLE_MATH_H


////////////////////////////////////////////////////////////////////////////////
// dot
////////////////////////////////////////////////////////////////////////////////


__device__ double dot(double2 a, double2 b)
{
	return a.x * b.x + a.y * b.y;
}
__device__ double dot(double3 a, double3 b)
{
	return a.x * b.x + a.y * b.y + a.z * b.z;
}
__device__ double dot(double4 a, double4 b)
{
	return a.x * b.x + a.y * b.y + a.z * b.z * a.w * b.w;
}


////////////////////////////////////////////////////////////////////////////////
// length
////////////////////////////////////////////////////////////////////////////////


__device__ double length(double2 v)
{
	return sqrt(dot(v, v));
}
__device__ double length(double3 v)
{
	return sqrt(dot(v, v));
}
__device__ double length(double4 v)
{
	return sqrt(dot(v, v));
}


////////////////////////////////////////////////////////////////////////////////
// floor
////////////////////////////////////////////////////////////////////////////////


__device__ double2 floor(double2 v)
{
	return make_double2(floor(v.x), floor(v.y));
}
__device__ double3 floor(double3 v)
{
	return make_double3(floor(v.x), floor(v.y), floor(v.z));
}
__device__ double4 floor(double4 v)
{
	return make_double4(floor(v.x), floor(v.y), floor(v.z), floor(v.w));
}


////////////////////////////////////////////////////////////////////////////////
// add
////////////////////////////////////////////////////////////////////////////////


__device__ double2 operator + (double2 a, double2 b)
{
	return make_double2(a.x + b.x, a.y + b.y);
}
__device__ void operator += (double2 &a, double2 b)
{
	a.x += b.x;
	a.y += b.y;
}
__device__ double2 operator + (double2 a, float b)
{
	return make_double2(a.x + b, a.y + b);
}
__device__ double2 operator + (float b, double2 a)
{
	return make_double2(b + a.x, b + a.y);
}
__device__ void operator += (double2 &a, float b)
{
	a.x += b;
	a.y += b;
}


__device__ double3 operator + (double3 a, double3 b)
{
	return make_double3(a.x + b.x, a.y + b.y, a.z + b.z);
}
__device__ void operator += (double3 &a, double3 b)
{
	a.x += b.x;
	a.y += b.y;
	a.z += b.z;
}
__device__ double3 operator + (double3 a, float b)
{
	return make_double3(a.x + b, a.y + b, a.z + b);
}
__device__ double3 operator + (float b, double3 a)
{
	return make_double3(b + a.x, b + a.y, b + a.z);
}
__device__ void operator += (double3 &a, float b)
{
	a.x += b;
	a.y += b;
	a.z += b;
}


__device__ double4 operator + (double4 a, double4 b)
{
	return make_double4(a.x + b.x, a.y + b.y, a.z + b.z, a.w + b.w);
}
__device__ void operator += (double4 &a, double4 b)
{
	a.x += b.x;
	a.y += b.y;
	a.z += b.z;
	a.w += b.w;
}
__device__ double4 operator + (double4 a, float b)
{
	return make_double4(a.x + b, a.y + b, a.z + b, a.w + b);
}
__device__ double4 operator + (float b, double4 a)
{
	return make_double4(b + a.x, b + a.y, b + a.z, b + a.w);
}
__device__ void operator += (double4 &a, float b)
{
	a.x += b;
	a.y += b;
	a.z += b;
	a.w += b;
}


////////////////////////////////////////////////////////////////////////////////
// subtract
////////////////////////////////////////////////////////////////////////////////


__device__ double2 operator - (double2 a, double2 b)
{
	return make_double2(a.x - b.x, a.y - b.y);
}
__device__ void operator -= (double2 &a, double2 b)
{
	a.x -= b.x;
	a.y -= b.y;
}
__device__ double2 operator - (double2 a, float b)
{
	return make_double2(a.x - b, a.y - b);
}
__device__ double2 operator - (float b, double2 a)
{
	return make_double2(b - a.x, b - a.y);
}
__device__ void operator -= (double2 &a, float b)
{
	a.x -= b;
	a.y -= b;
}


__device__ double3 operator - (double3 a, double3 b)
{
	return make_double3(a.x - b.x, a.y - b.y, a.z - b.z);
}
__device__ void operator -= (double3 &a, double3 b)
{
	a.x -= b.x;
	a.y -= b.y;
	a.z -= b.z;
}
__device__ double3 operator - (double3 a, float b)
{
	return make_double3(a.x - b, a.y - b, a.z - b);
}
__device__ double3 operator - (float b, double3 a)
{
	return make_double3(b - a.x, b - a.y, b - a.z);
}
__device__ void operator -= (double3 &a, float b)
{
	a.x -= b;
	a.y -= b;
	a.z -= b;
}


__device__ double4 operator - (double4 a, double4 b)
{
	return make_double4(a.x - b.x, a.y - b.y, a.z - b.z, a.w - b.w);
}
__device__ void operator -= (double4 &a, double4 b)
{
	a.x -= b.x;
	a.y -= b.y;
	a.z -= b.z;
	a.w -= b.w;
}
__device__ double4 operator - (double4 a, float b)
{
	return make_double4(a.x - b, a.y - b, a.z - b, a.w - b);
}
__device__ double4 operator - (float b, double4 a)
{
	return make_double4(b - a.x, b - a.y, b - a.z, b - a.w);
}
__device__ void operator -= (double4 &a, float b)
{
	a.x -= b;
	a.y -= b;
	a.z -= b;
	a.w -= b;
}


////////////////////////////////////////////////////////////////////////////////
// multiply
////////////////////////////////////////////////////////////////////////////////


__device__ double2 operator * (double2 a, double2 b)
{
	return make_double2(a.x * b.x, a.y * b.y);
}
__device__ void operator *= (double2 &a, double2 b)
{
	a.x *= b.x;
	a.y *= b.y;
}
__device__ double2 operator * (double2 a, double b)
{
	return make_double2(a.x * b, a.y * b);
}
__device__ double2 operator * (double b, double2 a)
{
	return make_double2(b * a.x, b * a.y);
}
__device__ void operator *= (double2 &a, double b)
{
	a.x *= b;
	a.y *= b;
}


__device__ double3 operator * (double3 a, double3 b)
{
	return make_double3(a.x * b.x, a.y * b.y, a.z * b.z);
}
__device__ void operator *= (double3 &a, double3 b)
{
	a.x *= b.x;
	a.y *= b.y;
	a.z *= b.z;
}
__device__ double3 operator * (double3 a, double b)
{
	return make_double3(a.x * b, a.y * b, a.z * b);
}
__device__ double3 operator * (double b, double3 a)
{
	return make_double3(b * a.x, b * a.y, b * a.z);
}
__device__ void operator *= (double3 &a, double b)
{
	a.x *= b;
	a.y *= b;
	a.z *= b;
}


__device__ double4 operator * (double4 a, double4 b)
{
	return make_double4(a.x * b.x, a.y * b.y, a.z * b.z, a.w * b.w);
}
__device__ void operator *= (double4 &a, double4 b)
{
	a.x *= b.x;
	a.y *= b.y;
	a.z *= b.z;
	a.w *= b.w;
}
__device__ double4 operator * (double4 a, double b)
{
	return make_double4(a.x * b, a.y * b, a.z * b, a.w * b);
}
__device__ double4 operator * (double b, double4 a)
{
	return make_double4(b * a.x, b * a.y, b * a.z, b * a.w);
}
__device__ void operator *= (double4 &a, double b)
{
	a.x *= b;
	a.y *= b;
	a.z *= b;
	a.w *= b;
}


#endif
