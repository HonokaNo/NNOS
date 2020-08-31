#include "nnos.h"

void HariMain(void)
{
	struct color invisible = {0x00, 0x00, 0x00, 0x00};
	int win;
	char buf[150 * 70 * 4];

	win = api_openwin(buf, 150, 70, "notrec");
	api_boxfilwin(win,   0, 50,  34, 69, &invisible);
	api_boxfilwin(win, 115, 50, 149, 69, &invisible);
	api_boxfilwin(win,  50, 30,  99, 49, &invisible);

	for(;;){
		if(api_getkey(1) == 0x0a) break;
	}

	api_end();
}
