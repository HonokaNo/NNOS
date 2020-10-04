#include <stdio.h>
#include "nnos.h"

int printf(const char *format, ...)
{
	va_list va;
	char s[1000];
	int i;

	va_start(va, format);
	i = vsprintf(s, format, va);
	api_putstr0(s);
	va_end(va);
	return i;
}
