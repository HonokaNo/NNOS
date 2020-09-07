#include "nnos.h"
#include "d_stdio.h"

void HariMain(void)
{
	int win;
	char buf[150 * 50 * 4];
	struct ret ret = {0x0000, 0x0000};

	win = api_openwin(buf, 150, 50, "winsize", 1);

	api_getwindata(win, 0, &ret);
	printf("%d %d\n", ret.v0, ret.v1);

	for(;;){
		int i = api_getkey(0);
		if(i == 1){
			api_getwindata(win, 0, &ret);
			printf("%d %d\n", ret.v0, ret.v1);
		}
		if(i == 0x0a) break;
	}

	api_end();
}
