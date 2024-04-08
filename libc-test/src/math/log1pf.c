#include <stdint.h>
#include <stdio.h>
#include "mtest.h"

static struct f_f t[] = {
#include "sanity/log1pf.h"
#include "special/log1pf.h"

};

int main(void)
{
	#pragma STDC FENV_ACCESS ON
	float y;
	float d;
	int e, i, err = 0;
	struct f_f *p;

	for (i = 0; i < sizeof t/sizeof *t; i++) {
		p = t + i;

		if (p->r < 0)
			continue;
		fesetround(p->r);
		feclearexcept(FE_ALL_EXCEPT);
		y = log1pf(p->x);
		e = fetestexcept(INEXACT|INVALID|DIVBYZERO|UNDERFLOW|OVERFLOW);

#if !defined(__aarch64__) && !(defined(__riscv) && __riscv_xlen == 64)
		if (!checkexcept(e, p->e, p->r)) {
			printf("%s:%d: bad fp exception: %s log1pf(%a)=%a, want %s",
				p->file, p->line, rstr(p->r), p->x, p->y, estr(p->e));
			printf(" got %s\n", estr(e));
			err++;
		}
#endif
		d = ulperrf(y, p->y, p->dy);
		if (!checkulp(d, p->r)) {
			printf("%s:%d: %s log1pf(%a) want %a got %a ulperr %.3f = %a + %a\n",
				p->file, p->line, rstr(p->r), p->x, p->y, y, d, d-p->dy, p->dy);
			err++;
		}
	}
	return !!err;
}
