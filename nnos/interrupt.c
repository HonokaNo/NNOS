#include "bootpack.h"

extern struct BUFFER buffer;

/* timer */
void inthandler20(int *esp)
{
	struct TIMER *timer;

	io_out8(PIC0_OCW2, 0x60);

	timerctl.count++;
	if(timerctl.next > timerctl.count) return;

	timer = timerctl.t0;
	for(;;){
		if(timer->timeout > timerctl.count) break;
		timer->flags = TIMER_FLAGS_ALLOC;
		buffer_put(timer->buf, TAG_TIMER, timer->data);
		timer = timer->next;
	}
	timerctl.t0 = timer;
	timerctl.next = timer->timeout;

	return;
}

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
