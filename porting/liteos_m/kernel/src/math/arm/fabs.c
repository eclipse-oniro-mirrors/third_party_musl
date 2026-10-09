#include <math.h>

#if defined(__aarch64__)

/* Soft-float implementation: clear the sign bit with integer ops. Avoids
 * FP registers entirely so the kernel can be built with
 * -mgeneral-regs-only (required when running MMU-off: Device memory
 * alignment rules make compiler-generated FP reg save/restore unsafe). */
double fabs(double x)
{
	union { double d; unsigned long long u; } u;
	u.d = x;
	u.u &= 0x7fffffffffffffffULL;
	return u.d;
}

#elif __ARM_PCS_VFP

double fabs(double x)
{
	__asm__ ("vabs.f64 %P0, %P1" : "=w"(x) : "w"(x));
	return x;
}

#else

#include "../fabs.c"

#endif
