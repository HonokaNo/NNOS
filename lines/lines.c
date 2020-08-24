#include "nnos.h"

void HariMain(void)
{
	char *buf;
	int win, i;

	struct color colors[] = {
		{0x00, 0x00, 0x00, 0xff},
		{0xff, 0x00, 0x00, 0xff},
		{0x00, 0xff, 0x00, 0xff},
		{0xff, 0xff, 0x00, 0xff},
		{0x00, 0x00, 0xff, 0xff},
		{0xff, 0x00, 0xff, 0xff},
		{0x00, 0xff, 0xff, 0xff},
		{0xff, 0xff, 0xff, 0xff},
	};
	struct color c;

	api_initmalloc();
	buf = api_malloc(160 * 100 * 4);
	win = api_openwin(buf, 160, 100, "lines");

	for(i = 0; i < 8; i++){
		c = colors[i];

		api_linewin(win + 1,  8, 26, 77, i * 9 + 26, &c);
		api_linewin(win + 1, 88, 26, i * 9 + 88, 89, &c);
	}
	api_refreshwin(win, 6, 26, 154, 90);

	for(;;){
		if(api_getkey(1) == 0x0a) break;
	}

	api_closewin(win);
	api_end();
}
