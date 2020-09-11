#include "nnos.h"

int sprintf(char *s, char *format, ...);

void HariMain(void)
{
	struct color black = {0x00, 0x00, 0x00, 0xff};
	struct color yellow = {0xff, 0xff, 0x00, 0xff};

	char *array[7] = {"Sun.", "Mon.", "Tue.", "Wed.", "Thu.", "Fri.", "Sat."};

	char buf[150 * 66 * 4], s[30];
	int win, year, y0, y1, month, day;
	struct time time;

	win = api_openwin(buf, 150, 66, "calendar", 0);
	api_getclock(&time);

	year = (time.y0 * 100 + time.y1);
	y0 = time.y0;
	y1 = time.y1;
	month = time.month;
	day = time.day;

	sprintf(s, " %04d %02d/%02d", year, month, time.day);
	api_putstrwin(win, 28, 27, &black, s);

	if(month < 3){
		if(y1 != 0) y1--;
		else{
			y1 = 99;
			y0--;
		}
		month += 12;
	}

	int yy = (y0 * 100 + y1);
	int h = (yy + (yy / 4) - (yy / 100) + (yy / 400) + ((13 * month + 8) / 5) + day) % 7;
	api_putstrwin(win, 60, 46, &yellow, array[h]);

	for(;;){
		if(api_getkey(0) != -1) break;
	}

	api_end();
}
