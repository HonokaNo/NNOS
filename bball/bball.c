#include "nnos.h"

void HariMain(void)
{
	char buf[216 * 237 * 4];
	int win, i, j, dis;
	struct POINT{
		int x, y;
	};

	static struct POINT table[16] = {
		{204, 129}, {195,  90}, {172,  58}, {137,  38},
		{ 98,  34}, { 61,  46}, { 31,  73}, { 15, 110},
		{ 15, 148}, { 31, 185}, { 61, 212}, { 98, 224},
		{137, 220}, {172, 200}, {195, 168}, {204, 129}
	};

	struct color c, colors[] = {
		{0x00, 0x00, 0x00, 0xff},
		{0xff, 0x00, 0x00, 0xff},
		{0x00, 0xff, 0x00, 0xff},
		{0xff, 0xff, 0x00, 0xff},
		{0x00, 0x00, 0xff, 0xff},
		{0xff, 0x00, 0xff, 0xff},
		{0x00, 0xff, 0xff, 0xff},
		{0xff, 0xff, 0xff, 0xff}
	};
	struct color black = {0x00, 0x00, 0x00, 0xff};

	win = api_openwin(buf, 216, 237, "bball");
	api_boxfilwin(win, 8, 29, 207, 228, &black);

	for(i = 0; i <= 14; i++){
		for(j = i + 1; j <= 15; j++){
			dis = j - i;
			if(dis >= 8) dis = 15 - dis;
			if(dis != 0){
				c = colors[8 - dis];

				api_linewin(win, table[i].x, table[i].y, table[j].x, table[j].y, &c);
			}
		}
	}

	for(;;){
		if(api_getkey(1) == 0x0a) break;
	}

	api_end();
}
