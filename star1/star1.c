#include "nnos.h"

void HariMain(void)
{
	char *buf;
	int win;
	struct color yellow = {0xff, 0xff, 0x00, 0xff};
	struct color  black = {0x00, 0x00, 0x00, 0xff};

	api_initmalloc();
	buf = api_malloc(150 * 100 * 4);
	win = api_openwin(buf, 150, 100, "star1");
	api_boxfilwin(win,  6, 26, 143, 93, &black);
	api_point(win, 75, 59, &yellow);

	for(;;){
		if(api_getkey(1) == 0x0a) break;
	}

	api_end();
}
