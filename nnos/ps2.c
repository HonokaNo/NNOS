#include "bootpack.h"

#define PORT_KEYSTA				0x64
#define KEYSTA_SEND_NOTREADY	0x02
#define KEYCMD_WRITE_MODE		0x60
#define KBC_MODE				0x47

#define KEYCMD_SENDTO_MOUSE		0xd4
#define MOUSECMD_ENABLE			0xf4

void wait_KBC_sendready(void)
{
	for(;;){
		if((io_in8(PORT_KEYSTA) & KEYSTA_SEND_NOTREADY) == 0) break;
	}
	return;
}

void init_keyboard(void)
{
	wait_KBC_sendready();
	io_out8(PORT_KEYCMD, KEYCMD_WRITE_MODE);
	wait_KBC_sendready();
	io_out8(PORT_KEYDAT, KBC_MODE);
	wait_KBC_sendready();
	return;
}

void enable_mouse(struct MOUSE_DEC *mdec)
{
	wait_KBC_sendready();
	io_out8(PORT_KEYCMD, KEYCMD_SENDTO_MOUSE);
	wait_KBC_sendready();
	io_out8(PORT_KEYDAT, MOUSECMD_ENABLE);
	wait_KBC_sendready();
	mdec->phase = 0;
	return;
}

int mouse_decode(struct MOUSE_DEC *mdec, unsigned char dat)
{
	if(mdec->phase == 0){
		if(dat == 0xfa) mdec->phase = 1;
		return 0;
	}
	if(mdec->phase == 1){
		if((dat & 0xc8) == 0x08){
			mdec->buf[0] = dat;
			mdec->phase = 2;
		}
		return 0;
	}
	if(mdec->phase == 2){
		mdec->buf[1] = dat;
		mdec->phase = 3;
		return 0;
	}
	if(mdec->phase == 3){
		mdec->buf[2] = dat;
		mdec->phase = 1;
		mdec->btn = mdec->buf[0] & 0x07;
		mdec->x = mdec->buf[1];
		mdec->y = mdec->buf[2];
		if((mdec->buf[0] & 0x10) != 0) mdec->x |= 0xffffff00;
		if((mdec->buf[0] & 0x20) != 0) mdec->y |= 0xffffff00;
		mdec->y = - mdec->y;

		return 1;
	}

	return -1;
}
