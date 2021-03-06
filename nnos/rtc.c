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
	/* すぐには更新されない */
	if((readcmos(0x0a) & 0x80) == 0){
		/* 秒を取得 */
		i = readcmos(0x00);
		k = (i >> 4) * 10 + (i & 0x0f);
		lt.sec = k;
		if(lt.sec > 60) lt.sec = 0xff;

		/* 分を取得 */
		i = readcmos(0x02);
		k = (i >> 4) * 10 + (i & 0x0f);
		lt.min = k;
		if(lt.min > 60) lt.sec = 0xff;

		/* 時を取得 */
		i = readcmos(0x04);
		k = (i >> 4) * 10 + (i & 0x0f);
		lt.hour = k;
		if(lt.hour > 12) lt.sec = 0xff;

		/* 日を取得 */
		i = readcmos(0x07);
		k = (i >> 4) * 10 + (i & 0x0f);
		lt.day = k;
		if(lt.day > 31) lt.sec = 0xff;

		/* 月を取得 */
		i = readcmos(0x08);
		k = (i >> 4) * 10 + (i & 0x0f);
		lt.month = k;
		if(lt.month > 12) lt.sec = 0xff;

		/* 年下2桁を取得 */
		i = readcmos(0x09);
		k = (i >> 4) * 10 + (i & 0x0f);
		lt.y1 = k;

		i = readcmos(0x32);
		k = (i >> 4) * 10 + (i & 0x0f);
		lt.y0 = k;
	}

	return lt;
}
