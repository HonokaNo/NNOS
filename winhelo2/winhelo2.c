#include "nnos.h"

char buf[150 * 50 * 4];

void HariMain(void)
{
	int win;
	struct color yellow = {0xff, 0xff, 0x00, 0xff};
	struct color  black = {0x00, 0x00, 0x00, 0xff};

	win = api_openwin(buf, 150, 50, "hello2");
	api_boxfilwin(win,  8, 36, 141, 43, &yellow);
	api_putstrwin(win, 28, 28, &black, "hello, world");

	for(;;){
		if(api_getkey(1) == 0x0a) break;
	}

	api_end();
}
