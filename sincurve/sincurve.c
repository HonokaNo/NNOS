#include <math.h>
#include "nnos.h"

void HariMain(void)
{
	struct color black = {0x00, 0x00, 0x00, 0xff};
	char buf[160 * 100 * 4];
	int win, i, x, y;
	win = api_openwin(buf, 160, 100, "sincurve");

	for(i = 0; i < 144; i++){
		x = i + 8;
		y = sin(i * 0.05) * 30 + 60;

		api_point(win, x, y, &black);
	}

	api_getkey(1);
	api_end();
}
