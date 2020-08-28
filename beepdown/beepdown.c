#include "nnos.h"

void HariMain(void)
{
	int i, timer;

	timer = api_alloctimer();
	api_inittimer(timer, 128);
	api_settimer(timer, 1);

	for(i = 20000000; i >= 20000; i -= i / 100){
		if(api_gettimer(1) == 128){
			api_beep(i);
			api_settimer(timer, 1);
		}
	}

	api_beep(0);
	api_end();
}
