#include "nnos.h"

char buf[150 * 100 * 4];

void HariMain(void)
{
	int win;
	win = api_openwin(buf, 150, 50, "hello");
	api_end();
}
