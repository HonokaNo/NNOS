#include <math.h>
#include "nnos.h"

void HariMain(void)
{
	struct color white = {0x00, 0x00, 0x00, 0xff};
	char buf[160 * 100 * 4];
	int win, i;
	win = api_openwin(buf, 160, 100, "sincurve");

	for(i = 0; i < 144; i++) api_point(win, i + 8, sin(i * 0.05) * 30 + 60, &white);

	api_getkey(1);
	api_end();
}
