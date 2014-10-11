
#ifndef INCLUDED_CBPP_ATOMICS_H
#define INCLUDED_CBPP_ATOMICS_H


// Prologue check
#if defined(CMP_CPP) || defined(CMP_HLSL) || defined(CMP_WITHIN_PROLOGUE)


#ifdef CMP_CUDA

	cmp_device_fn uint cmp_atomic_inc(uint* p)
	{
		return atomicAdd(p, 1);
	}
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

#elif CMP_OPENCL

	cmp_device_fn uint cmp_atomic_inc(cmp_global uint* p)
	{
		return atomic_inc(p);
	}
	cmp_device_fn uint cmp_atomic_or(cmp_global uint* p, uint val)
	{
		return atomic_or(p, val);
	}
	cmp_device_fn uint cmp_atomic_min(cmp_global uint* p, uint val)
	{
		return atomic_min(p, val);
	}
	cmp_device_fn uint cmp_atomic_max(cmp_global uint* p, uint val)
	{
		return atomic_max(p, val);
	}

#elif CMP_CPP

	// TODO: NOT atomic in C++ land - is this necessary?
	cmp_device_fn uint cmp_atomic_inc(uint* p)
	{
		uint old = *p;
		*p += 1;
		return old;
	}
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

#endif


#endif	// Prologue check


#endif	// INCLUDED_CBPP_ATOMICS_H