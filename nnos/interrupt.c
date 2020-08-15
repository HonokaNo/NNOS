#include "bootpack.h"

extern struct BUFFER buffer;

/* PS/2 keyboard */
void inthandler21(int *esp)
{
	io_out8(PIC0_OCW2, 0x61);

	buffer_put(&buffer, TAG_KEYBOARD, io_in8(PORT_KEYDAT));
	return;
}

/* ïsäÆëSäÑÇËçûÇ›ñhé~ */
void inthandler27(int *esp)
{
	io_out8(PIC0_OCW2, 0x67);

	return;
}

/* RTC */
void inthandler28(int *esp)
{
	io_out8(PIC1_OCW2, 0x60);
	io_out8(PIC0_OCW2, 0x62);

	/* RTC update */
	buffer_put(&buffer, TAG_RTC, 1);

	io_out8(0x70, 0x0c);
	io_in8(0x71);

	return;
}

void inthandler2c(int *esp)
{
	io_out8(PIC1_OCW2, 0x64);
	io_out8(PIC0_OCW2, 0x62);

	buffer_put(&buffer, TAG_MOUSE, io_in8(PORT_KEYDAT));

	return;
}
