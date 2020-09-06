#include <stdio.h>
#include "nnos.h"

void HariMain(void)
{
	struct color black = {0x00, 0x00, 0x00, 0xff};
	struct color yellow = {0xff, 0xff, 0x00, 0xff};

	char *array[7] = {"Sun.", "Mon.", "Tue.", "Wed.", "Thu.", "Fri.", "Sat."};

	char buf[150 * 66 * 4], s[12];
	int win;
	struct time time;

	win = api_openwin(buf, 150, 66, "calendar");
	api_getclock(&time);

	sprintf(s, " %02d%02d %02d/%02d", time.y0, time.y1, time.month, time.day);
	api_putstrwin(win, 28, 27, &black, s);

	int year = (time.y0 * 100 + time.y1);
	long d = (5 * year / 4 - year / 100 + year / 400 + ((26 * time.month + 16) / 10) + time.day);
	api_putstrwin(win, 60, 46, &yellow, array[d % 7]);

	for(;;){
		if(api_getkey(0) != -1) break;
	}

	api_end();
}
