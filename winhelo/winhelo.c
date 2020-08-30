#include "nnos.h"

void HariMain(void)
{
	char buf[150 * 100 * 4];
	int win;
	win = api_openwin(buf, 150, 50, "hello");

	for(;;){
		if(api_getkey(1) == 0x0a) break;
	}

	api_end();
}
