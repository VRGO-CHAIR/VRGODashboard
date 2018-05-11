#pragma once
/*! \file x86timer.hpp
\brief Timing based on elapsed CPU cycles on X86 or compatible CPUs using rdtsc register
*/

#if defined(__GNUC__) || defined(__ICC)
#ifndef __GNUC__
#define __GNUC__
#endif
#elif defined(_MSC_VER)
#else
#error "Compiler not supported"
#endif
/*
#if defined(__i386__) || defined(__x86_64__)
#else
#error "Platform not supported"
#endif
*/

#if defined(__x86_64__) || defined(_M_X64)
/* x86 64-bit ----------------------------------------------- */

#elif defined(__i386) || defined(_M_IX86)
/* x86 32-bit ----------------------------------------------- */

#else
#error "Platform not supported"
#endif


#ifdef _MSC_VER
typedef unsigned __int64 uint64_t;
typedef __int64 int64_t;
#endif

#ifdef __GNUC__
/** CPU cycles since processor startup */
inline uint64_t rdtsc() {
	uint32_t lo, hi;
	/* We cannot use "=A", since this would use %rax on x86_64 */
	__asm__ __volatile__("rdtsc" : "=a" (lo), "=d" (hi));
	return (uint64_t)hi << 32 | lo;
}
#endif

#ifdef _MSC_VER
/*__declspec(naked)
unsigned __int64 __cdecl rdtsc(void)
{
	__asm
	{
		rdtsc
		ret; return value at EDX : EAX
	}
}*/

#include <intrin.h>
/*inline unsigned __int64 rdtsc(void)
{
	return __rdtsc();
}*/
#define rdtsc __rdtsc

#endif

/** Accurate timing and sleeping on pentium or newer x86 computers */
class x86timer {
private:

	uint64_t fstart, fend;
	double clocks_per_nanosecond;
	uint64_t start_, end_;
	double sum;
	int times;

public:

	/** Nanosecond sleep */
	void nanosleep(uint64_t nanoseconds)
	{
		uint64_t begin = rdtsc();
		uint64_t now;
		uint64_t dtime = (uint64_t)(nanoseconds*clocks_per_nanosecond);
		do
		{
			now = rdtsc();
		} while ((now - begin) < dtime);
	}

	void start()
	{
		start_ = rdtsc();
	}

	/** Returns elapsed nanoseconds */
	uint64_t stop()
	{
		end_ = rdtsc();
		return (uint64_t)(double(end_ - start_) / clocks_per_nanosecond);
	}

	x86timer() {

		sum = 0;
		times = 0;
		uint64_t bench1 = 0;
		uint64_t bench2 = 0;
		clocks_per_nanosecond = 0;

		bench1 = rdtsc();

#ifdef __GNUC__
		usleep(250000); // 1/4 second
#else
		Sleep(250);
#endif

		bench2 = rdtsc();

		clocks_per_nanosecond = (double)(bench2 - bench1);
		clocks_per_nanosecond *= 4.0e-9;
	}

};