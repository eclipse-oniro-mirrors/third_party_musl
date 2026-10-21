#define _GNU_SOURCE
#include <unistd.h>
#include "libc.h"

int __getpagesize(void)
{
	return PAGE_SIZE;
}

weak_alias(__getpagesize, getpagesize);
