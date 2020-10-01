#include "nnos.h"

void HariMain(void)
{
	char s[20];
	sprintf(s, "0");
/*	struct color white = {0xff, 0xff, 0xff, 0xff};
	struct color black = {0x00, 0x00, 0x00, 0xff};

	char s[20], *buf;
	int win, timer, sec = 0, min = 0, hour = 0, j;

	api_initmalloc();
	buf = api_malloc(150 * 50 * 4);
	win = api_openwin(buf, 150, 50, "noodle", 0);

	api_putstr0("1\n");

	api_boxfilwin(win, 28, 27, 115, 41, &white);
	sprintf(s, "%5d:%02d:%02d", hour, min, sec);
	api_putstrwin(win, 28, 27, &black, s);

	api_putstr0("2\n");

	timer = api_alloctimer();
	api_inittimer(timer, 128);
	api_settime(timer, 100);

	api_putstr0("3\n");

	for(;;){
		if(api_getkey(0) != -1) break;

		j = api_gettimer(0);

		if(j == 128){
			sec++;
			if(sec == 60){
				sec = 0;
				min++;
				if(min == 60){
					min = 0;
					hour++;
				}
			}

			api_boxfilwin(win, 28, 27, 115, 41, &white);
			sprintf(s, "%5d:%02d:%02d", hour, min, sec);
			api_putstrwin(win, 28, 27, &black, s);
			api_settime(timer, 100);
		}
	}*/

	api_end();
}
