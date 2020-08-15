#include "bootpack.h"

int readcmos(unsigned char addr)
{
	io_out8(0x70, addr);
	return io_in8(0x71);
}

struct localtime readrtc()
{
	int i = 0, k;
	struct localtime lt;

	lt.sec = 0xff;
	/* ‚·‚®‚É‚ÍXV‚³‚ê‚È‚¢ */
	if((readcmos(0x0a) & 0x80) == 0){
		/* •b‚ğæ“¾ */
		i = readcmos(0x00);
		k = (i >> 4) * 10 + (i & 0x0f);
		lt.sec = i;
		if(lt.sec > 0x60) lt.sec = 0xff;

		/* •ª‚ğæ“¾ */
		i = readcmos(0x02);
		k = (i >> 4) * 10 + (i & 0x0f);
		lt.min = i;
		if(lt.sec > 0x60) lt.sec = 0xff;

		/* ‚ğæ“¾ */
		i = readcmos(0x04);
		k = (i >> 4) * 10 + (i & 0x0f);
		lt.hour = i;
		if(lt.sec > 0x12) lt.sec = 0xff;
	}

	return lt;
}
