#include "d_stdio.h"

int fflush(FILE *stream)
{
	/* バッファはないので何もしない */
	return 0;
}
