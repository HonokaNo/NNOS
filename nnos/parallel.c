#include <stdio.h>
#include "bootpack.h"

/* 10ƒNƒƒbƒN‘Ò‚Â */
//void wait10(void);

void wait_receptive(void)
{
//	while((io_in8(0x379) & 0x80) != 0) wait10();
	while(!(io_in8(0x379) & 0x80));
	return;
}

void send_data(unsigned char data)
{
	unsigned char ctrl;

	wait_receptive();

	io_out8(0x378, data);

	ctrl = io_in8(0x37a);
	io_out8(0x37a, ctrl | 1);
	io_out8(0x37a, ctrl);
//	wait10();
	io_out8(0x37a, ctrl);

	wait_receptive();

	return;
}

void send_string(char *string)
{
	for(; *string != 0x00; string++) send_data(*string);
	return;
}

void printlog(char *format, ...)
{
	va_list va;
	char s[1000];

	va_start(va, format);
	vsprintf(s, format, va);
	send_string(s);
	va_end(va);

	return;
}
