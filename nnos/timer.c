#include "bootpack.h"

#define PIT_CTRL	0x43
#define PIT_CNT0	0x40

struct TIMERCTL timerctl;

#define TIMER_FLAGS_ALLOC	1
#define TIMER_FLAGS_USING	2

void init_pit(void)
{
	io_out8(PIT_CTRL, 0x34);
	io_out8(PIT_CNT0, 0x9c);
	io_out8(PIT_CNT0, 0x2e);
	timerctl.count = 0;
	timerctl.next = 0xffffffff;
	return;
}

void init_timerctl(void)
{
	struct MEMMAN *memman = (struct MEMMAN *)MEMMAN_ADDR;
	struct TIMER *t;
	int i;

	timerctl.timers0 = (struct TIMER *)memman_alloc_4k(memman, MAX_TIMER * sizeof(struct TIMER));
	for(i = 0; i < MAX_TIMER; i++){
		timerctl.timers0[i].flags = 0;
	}
	t = timer_alloc();
	t->timeout = 0xffffffff;
	t->flags = TIMER_FLAGS_USING;
	t->next = 0;
	timerctl.t0 = t;

	return;
}

struct TIMER *timer_alloc(void)
{
	int i;

	for(i = 0; i < MAX_TIMER; i++){
		if(timerctl.timers0[i].flags == 0){
			timerctl.timers0[i].flags = TIMER_FLAGS_ALLOC;
			timerctl.timers0[i].flags2 = 0;
			return &timerctl.timers0[i];
		}
	}
	return 0;
}

void timer_free(struct TIMER *timer)
{
	if(timer->flags == TIMER_FLAGS_USING) timer_cancel(timer);

	timer->flags = 0;
	return;
}

void timer_init(struct TIMER *timer, struct BUFFER *buf, unsigned char data)
{
	timer->buf = buf;
	timer->data = data;
	return;
}

void timer_settime(struct TIMER *timer, unsigned int timeout)
{
	int e;
	struct TIMER *t, *s;

	timer->timeout = timeout + timerctl.count;
	timer->flags = TIMER_FLAGS_USING;
	e = io_load_eflags();
	io_cli();

	t = timerctl.t0;
	if(timer->timeout <= t->timeout){
		timerctl.t0 = timer;
		timer->next = t;
		timerctl.next = timer->timeout;
		io_store_eflags(e);
		return;
	}

	for(;;){
		s = t;
		t = t->next;
		if(timer->timeout <= t->timeout){
			s->next = timer;
			timer->next = t;
			io_store_eflags(e);
			return;
		}
	}
}

int timer_cancel(struct TIMER *timer)
{
	int e;
	struct TIMER *t;

	e = io_load_eflags();
	io_cli();
	if(timer->flags == TIMER_FLAGS_USING){
		if(timer == timerctl.t0){
			t = timer->next;
			timerctl.t0 = t;
			timerctl.next = t->timeout;
		}else{
			t = timerctl.t0;
			for(;;){
				if(t->next == timer) break;
				t = t->next;
			}
			t->next = timer->next;
		}
		timer->flags = TIMER_FLAGS_ALLOC;
		io_store_eflags(e);
		return 1;
	}
	io_store_eflags(e);
	return 0;
}

void timer_cancelall(struct BUFFER *buf)
{
	int e, i;
	struct TIMER *t;

	e = io_load_eflags();
	io_cli();
	for(i = 0; i < MAX_TIMER; i++){
		t = &timerctl.timers0[i];
		if(t->flags != 0 && t->flags2 != 0 && t->buf == buf){
			timer_cancel(t);
			timer_free(t);
		}
	}
	io_store_eflags(e);
	return;
}
