#define _XOPEN_SOURCE 700
#include <stdint.h>
#include <stdio.h>
#include "mtest.h"

static struct d_d t[] = {
#include "sanity/y0.h"
#include "special/y0.h"

};

int main(void)
{
	#pragma STDC FENV_ACCESS ON
	double y;
	float d;
	int e, i, bad, err = 0;
	struct d_d *p;

	for (i = 0; i < sizeof t/sizeof *t; i++) {
		p = t + i;

		if (p->r < 0)
			continue;
		fesetround(p->r);
		feclearexcept(FE_ALL_EXCEPT);
		y = y0(p->x);
		e = fetestexcept(INEXACT|INVALID|DIVBYZERO|UNDERFLOW|OVERFLOW);

#if !defined(__aarch64__) && !(defined(__riscv) && __riscv_xlen == 64)
		if (!checkexcept(e, p->e, p->r)) {
			printf("%s:%d: bad fp exception: %s y0(%a)=%a, want %s",
				p->file, p->line, rstr(p->r), p->x, p->y, estr(p->e));
			printf(" got %s\n", estr(e));
			err++;
		}
#endif
		d = ulperr(y, p->y, p->dy);
		bad = p->x < 0 && !isnan(y) && y != -inf;
		if (bad || (!(p->x < 0) && !checkulp(d, p->r))) {
			if (!bad && fabsf(d) < 0x1p52f)
				printf("X ");
			else
				err++;
			printf("%s:%d: %s y0(%a) want %a got %a ulperr %.3f = %a + %a\n",
				p->file, p->line, rstr(p->r), p->x, p->y, y, d, d-p->dy, p->dy);
		}
	}
	return !!err;
}
