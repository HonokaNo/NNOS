#include <string.h>
#include "nnos.h"

int sprintf(char *s, char *format, ...);

void HariMain(void)
{
	int win;
	char buf[150 * 50 * 4], s[30];
	struct ret ret = {0x0000, 0x0000};
	struct color black = {0x00, 0x00, 0x00, 0xff};

	win = api_openwin(buf, 150, 50, "winsize", 1);

	api_getwindata(win, 0, &ret);
	sprintf(s, "%d %d", ret.v0, ret.v1);
	api_putstrwin(win, ret.v0 / 2 - strlen(s) * 8 / 2, 30, &black, s);

	for(;;){
		int i = api_getkey(0);
		if(i == 1){
			api_getwindata(win, 0, &ret);
			sprintf(s, "%d %d", ret.v0, ret.v1);
			api_putstrwin(win, ret.v0 / 2 - strlen(s) * 8 / 2, 30, &black, s);
		}
		if(i == 0x0a) break;
	}

	api_end();
}
