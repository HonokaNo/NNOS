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
	win = api_openwin(buf, 150, 100, "star2");
	api_boxfilwin(win + 1, 6, 26, 143, 93, &black);

	for(i = 0; i < 50; i++){
		y = (rand() %  67) + 26;
		x = (rand() % 137) +  6;
		api_point(win + 1, x, y, &yellow);
	}
	api_refreshwin(win, 6, 26, 144, 94);

	api_end();
}
