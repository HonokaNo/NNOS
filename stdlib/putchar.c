#include "d_stdio.h"

int putchar(int c)
{
	api_putchar(c);

	return c;
}
