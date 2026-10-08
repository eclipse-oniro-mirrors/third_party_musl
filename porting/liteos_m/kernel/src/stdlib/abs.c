#include <stdlib.h>

int abs(int a)
{
	/* 0U - (unsigned)a keeps the wrap-around result of -a but avoids the
	 * signed-overflow UB for INT_MIN, which -Oz -flto -fno-builtin
	 * constant-folds to poison (miscompiled to 0). */
	return a > 0 ? a : (int)(0U - (unsigned int)a);
}
