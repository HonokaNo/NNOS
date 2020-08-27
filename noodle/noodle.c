#include <stdio.h>
#include "nnos.h"

void HariMain(void)
{
	struct color white = {0xff, 0xff, 0xff, 0xff};
	struct color black = {0x00, 0x00, 0x00, 0xff};

	char *buf, s[12];
	int win, timer, sec = 0, min = 0, hour = 0;

	api_initmalloc();
	buf = api_malloc(150 * 50 * 4);
	win = api_openwin(buf, 150, 50, "noodle");
	timer = api_alloctimer();
	api_inittimer(timer, 128);

	for(;;){
		sprintf(s, "%5d:%02d:%02d", hour, min, sec);
		api_boxfilwin(win, 28, 27, 115, 41, &white);
		api_putstrwin(win, 28, 27, &black, s);
		api_settimer(timer, 100);

		if(api_getkey(0) != -1) break;

		if(api_gettimer(1) == 128){
			sec++;
			if(sec == 60){
				sec = 0;
				min++;
				if(min == 60){
					min = 0;
					hour++;
				}
			}
		}
	}

	api_end();
}
