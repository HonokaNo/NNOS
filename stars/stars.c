#include "nnos.h"

int rand(void);

void HariMain(void)
{
	char *buf;
	int win, i, x, y;
	struct color yellow = {0xff, 0xff, 0x00, 0xff};
	struct color  black = {0x00, 0x00, 0x00, 0xff};

	api_initmalloc();
	buf = api_malloc(150 * 100 * 4);
	win = api_openwin(buf, 150, 100, "stars");
	api_boxfilwin(win,  6, 26, 143, 93, &black);

	for(i = 0; i < 50; i++){
		y = (rand() %  67) + 26;
		x = (rand() % 137) +  6;
		api_point(win, x, y, &yellow);
	}

	for(;;){
		if(api_getkey(1) == 0x0a) break;
	}

	api_end();
}
