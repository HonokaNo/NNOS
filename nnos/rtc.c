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
		lt.sec = k;
		if(lt.sec > 60) lt.sec = 0xff;

		/* •ª‚ğæ“¾ */
		i = readcmos(0x02);
		k = (i >> 4) * 10 + (i & 0x0f);
		lt.min = k;
		if(lt.min > 60) lt.sec = 0xff;

		/* ‚ğæ“¾ */
		i = readcmos(0x04);
		k = (i >> 4) * 10 + (i & 0x0f);
		lt.hour = k;
		if(lt.hour > 12) lt.sec = 0xff;

		/* “ú‚ğæ“¾ */
		i = readcmos(0x07);
		k = (i >> 4) * 10 + (i & 0x0f);
		lt.day = k;
		if(lt.day > 31) lt.sec = 0xff;

		/* Œ‚ğæ“¾ */
		i = readcmos(0x08);
		k = (i >> 4) * 10 + (i & 0x0f);
		lt.month = k;
		if(lt.month > 12) lt.sec = 0xff;

		/* ”N‰º2Œ…‚ğæ“¾ */
		i = readcmos(0x09);
		lt.y1 = (i >> 4) * 10 + (i & 0x0f);

		i = readcmos(0x32);
		lt.y0 = (i >> 4) * 10 + (i & 0x0f);
	}

	return lt;
}
