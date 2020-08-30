#include "nnos.h"

void HariMain(void)
{
	struct color black  = {0x00, 0x00, 0x00, 0xff};
	struct color yellow = {0xff, 0xff, 0x00, 0xff};

	char *buf;
	int win, i, x, y;

	api_initmalloc();
	buf = api_malloc(160 * 100 * 4);
	win = api_openwin(buf, 160, 100, "walk");
	api_boxfilwin(win, 4, 24, 155, 95, &black);
	x = 76;
	y = 56;

	api_putstrwin(win, x, y, &yellow, "*");

	for(;;){
		i = api_getkey(0);
		api_putstrwin(win, x, y, &black, "*");
		if(i == '4' && x >   4) x -= 8;
		if(i == '6' && x < 148) x += 8;
		if(i == '8' && y >  24) y -= 8;
		if(i == '2' && y <  80) y += 8;
		if(i == 0x0a) break;

		api_putstrwin(win, x, y, &yellow, "*");
	}

	api_closewin(win);
	api_end();
}
