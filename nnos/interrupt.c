#include "bootpack.h"
#include <stdio.h>

extern struct BUFFER buffer;

void init_pic(void)
{
	/* äÑÇËçûÇ›ã÷é~ */
	io_out8(PIC0_IMR, 0xff);
	io_out8(PIC1_IMR, 0xff);

	io_out8(PIC0_ICW1, 0x11);
	io_out8(PIC0_ICW2, 0x20);
	io_out8(PIC0_ICW3, 0x04);
	io_out8(PIC0_ICW4, 0x01);

	io_out8(PIC1_ICW1, 0x11);
	io_out8(PIC1_ICW2, 0x28);
	io_out8(PIC1_ICW3, 0x02);
	io_out8(PIC1_ICW4, 0x01);

	io_out8(PIC0_IMR, 0xfb);
	io_out8(PIC1_IMR, 0xff);

	return;
}

int *inthandler07(int *esp)
{
	struct TASK *task = task_now();

	io_cli();
	clts();
	if(taskctl->fpu_task != task){
		if(taskctl->fpu_task != 0) fnsave(taskctl->fpu_task->fpu_register);
		frstor(task->fpu_register);
		taskctl->fpu_task = task;
	}
	io_sti();

	return 0;
}

int *inthandler0c(int *esp)
{
	struct TASK *task = task_now();
	struct CONSOLE *cons = task->cons;
	char s[30];
	cons_putstr0(cons, "\nINT 0c :\n Stack Exception.\n");
	sprintf(s, "EIP = %08X\n", esp[11]);
	cons_putstr0(cons, s);
	return &(task->tss.esp0);
}

int *inthandler0d(int *esp)
{
	struct TASK *task = task_now();
	struct CONSOLE *cons = task->cons;
	char s[30];
	cons_putstr0(cons, "\nINT 0d :\n General Protected Exception.\n");
	sprintf(s, "EIP = %08X\n", esp[11]);
	cons_putstr0(cons, s);
	return &(task->tss.esp0);
}

/* timer */
void inthandler20(int *esp)
{
	struct TIMER *timer;
	char ts = 0;

	io_out8(PIC0_OCW2, 0x60);

	timerctl.count++;
	if(timerctl.next > timerctl.count) return;

	timer = timerctl.t0;
	for(;;){
		if(timer->timeout > timerctl.count) break;
		timer->flags = TIMER_FLAGS_ALLOC;
		if(timer != task_timer) buffer_put(timer->buf, TAG_TIMER, timer->data);
		else ts = 1;
		timer = timer->next;
	}
	timerctl.t0 = timer;
	timerctl.next = timer->timeout;
	if(ts != 0) task_switch();

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
