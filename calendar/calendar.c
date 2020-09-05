#include <stdio.h>
#include "nnos.h"

int day_of_week(struct time time);

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
	api_putstrwin(win, 60, 46, &yellow, array[day_of_week(time)]);

	for(;;){
		if(api_getkey(0) != -1) break;
	}

	api_end();
}

int day_of_week(struct time time)
{
	int a = time.month, b = time.y0 * 100 + time.y1, t;

	if(a < 3){
		a += 12;
		b--;
	}
	t = time.day + (13 * a - 27) / 5 + b + b/4 - b/100 + b/400;

	return (t + 4) % 7;
}
