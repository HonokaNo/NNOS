#include <stdio.h>
#include <string.h>
#include "bootpack.h"

struct BUFFER buffer;

#define KEYCMD_LED	0xed

void HariMain(void)
{
	struct color white = {0xff, 0xff, 0xff, 0xff};
	struct color black = {0x00, 0x00, 0x00, 0xff};
	struct color light_gray = {0xc6, 0xc6, 0xc6, 0xff};
	struct color invisible_cursor = {0x00, 0x00, 0x00, 0x00};
	struct color back24  = {0x99, 0xd9, 0xea, 0xff};
	struct color back8 = {0x00, 0x84, 0x84, 0xff};
	struct color back, cursor_c;

	struct BOOTINFO *binfo = (struct BOOTINFO *)ADR_BOOTINFO;
	struct localtime lt;
	char s[35];
	unsigned int memtotal;
	struct MEMMAN *memman = (struct MEMMAN *)MEMMAN_ADDR;;
	struct BUFDATA dat;
	struct SHTCTL *shtctl;
	struct SHEET *sht_back, *sht_win, *sht_mouse, *sht_win2, *sht_win3, *sht_cons;
	unsigned char *buf_back, *buf_win, buf_mouse[256 * 4], *buf_win2, *buf_win3, *buf_cons;
	struct MOUSE_DEC mdec;
	int mx, my, cursor_x;
	struct TIMER *timer;
	struct TASK *task_a, *task_cons;
	struct BUFFER keycmd;
	struct CONSOLE *cons;
	int key_to = 0, key_shift = 0, key_leds = (binfo->leds >> 4) & 7, keycmd_wait = -1;

	static char keytable0[0x80] = {
		0,   0,   '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '-', '^', 0,   0,
		'Q', 'W', 'E', 'R', 'T', 'Y', 'U', 'I', 'O', 'P', '@', '[', 0,   0,   'A', 'S',
		'D', 'F', 'G', 'H', 'J', 'K', 'L', ';', ':', 0,   0,   ']', 'Z', 'X', 'C', 'V',
		'B', 'N', 'M', ',', '.', '/', 0,   '*', 0,   ' ', 0,   0,   0,   0,   0,   0,
		0,   0,   0,   0,   0,   0,   0,   '7', '8', '9', '-', '4', '5', '6', '+', '1',
		'2', '3', '0', '.', 0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
		0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
		0,   0,   0,   0x5c, 0,  0,   0,   0,   0,   0,   0,   0,   0,   0x5c, 0,  0
	};

	static char keytable1[0x80] = {
		0,   0,   '!', 0x22, '#', '$', '%', '&', 0x27, '(', ')', '~', '=', '~', 0,   0,
		'Q', 'W', 'E', 'R', 'T', 'Y', 'U', 'I', 'O', 'P', '`', '{', 0,   0,   'A', 'S',
		'D', 'F', 'G', 'H', 'J', 'K', 'L', '+', '*', 0,   0,   '}', 'Z', 'X', 'C', 'V',
		'B', 'N', 'M', '<', '>', '?', 0,   '*', 0,   ' ', 0,   0,   0,   0,   0,   0,
		0,   0,   0,   0,   0,   0,   0,   '7', '8', '9', '-', '4', '5', '6', '+', '1',
		'2', '3', '0', '.', 0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
		0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
		0,   0,   0,   '_', 0,   0,   0,   0,   0,   0,   0,   0,   0,   '|', 0,   0
	};

	init_gdtidt();
	init_pic();

	/* rtc初期化 */
	io_out8(0x70, 0x0b);
	char prev = io_in8(0x71) | 0x10;
	io_out8(0x71, prev);

	io_sti();

	/* 割り込み有効化 11111000 */
	io_out8(PIC0_IMR, 0xf8);
	/* 割り込み有効化 11101110 */
	io_out8(PIC1_IMR, 0xee);

	buffer_init(&buffer, 250, 0);
	buffer_init(&keycmd,  32, 0);

	init_pit();

	init_keyboard();
	enable_mouse(&mdec);

	/* メモリ量取得 */
	memtotal = memtest(ADR_BOTPAK, 0xffffffff);
	memman_init(memman);
	memman_free(memman, 0x00001000, 0x0009e000);
	memman_free(memman, 0x00400000, memtotal - 0x00400000);

	timer = timer_alloc();
	timer_init(timer, &buffer, 1);
	timer_settime(timer, 50);

	if(binfo->vmode == 8){
		setPalette();
		back = back8;
	}else back = back24;

	shtctl = shtctl_init(memman, binfo->vram, binfo->scrnx, binfo->scrny);

	/* 背景 */
	sht_back = sheet_alloc(shtctl);
	buf_back = (unsigned char *)memman_alloc_4k(memman, binfo->scrnx * binfo->scrny * VMODE_WINDOW / 8);
	sheet_setbuf(sht_back, buf_back, binfo->scrnx, binfo->scrny);
	init_screen(VMODE_WINDOW, buf_back, WINDOW_SCLINE(sht_back), binfo->scrnx, binfo->scrny, binfo->vmode == 8 ? back8 : back);

	task_a = task_init(memman);
	buffer.task = task_a;
	task_run(task_a, 1, 2);

	/* ウィンドウ */
	sht_win = sheet_alloc(shtctl);
	buf_win = (unsigned char *)memman_alloc_4k(memman, 160 * 68 * VMODE_WINDOW / 8);
	sheet_setbuf(sht_win, buf_win, 160, 68);
	make_window(sht_win, "window", 1);
	putfontstr(VMODE_WINDOW, buf_win, WINDOW_SCLINE(sht_win), 24, 28, black, "Welcome to");
	putfontstr(VMODE_WINDOW, buf_win, WINDOW_SCLINE(sht_win), 24, 44, black, "  No Name OS!");

	sht_win2 = sheet_alloc(shtctl);
	buf_win2 = (unsigned char *)memman_alloc_4k(memman, 160 * 68 * VMODE_WINDOW / 8);
	sheet_setbuf(sht_win2, buf_win2, 160, 68);
	make_window(sht_win2, "window2", 1);
	putfontstr(VMODE_WINDOW, buf_win2, WINDOW_SCLINE(sht_win2), 24, 28, black, "Welcome to");
	putfontstr(VMODE_WINDOW, buf_win2, WINDOW_SCLINE(sht_win2), 24, 44, black, "  No Name OS!");

	sht_win3 = sheet_alloc(shtctl);
	buf_win3 = (unsigned char *)memman_alloc_4k(memman, 160 * 52 * VMODE_WINDOW / 8);
	sheet_setbuf(sht_win3, buf_win3, 160, 52);
	make_window(sht_win3, "textbox", 1);
	make_textbox(sht_win3, 8, 28, 144, 16, white);

	cursor_x = 8;
	cursor_c = white;

	sht_cons = sheet_alloc(shtctl);
	buf_cons = (unsigned char *)memman_alloc_4k(memman, 256 * 165 * VMODE_WINDOW / 8);
	sheet_setbuf(sht_cons, buf_cons, 256, 165);
	make_window(sht_cons, "console", 0);
	make_textbox(sht_cons, 8, 28, 240, 128, black);

	task_cons = task_alloc();
	task_cons->tss.esp = memman_alloc_4k(memman, 64 * 1024) + 64 * 1024 - 12;
	task_cons->tss.eip = (int)&console_task;
	task_cons->tss.es = 1 * 8;
	task_cons->tss.cs = 2 * 8;
	task_cons->tss.ss = 1 * 8;
	task_cons->tss.ds = 1 * 8;
	task_cons->tss.fs = 1 * 8;
	task_cons->tss.gs = 1 * 8;
	*((int *)(task_cons->tss.esp + 4)) = (int)sht_cons;
	*((int *)(task_cons->tss.esp + 8)) = memtotal;
	task_run(task_cons, 2, 2);

	/* マウス */
	sht_mouse = sheet_alloc(shtctl);
	sheet_setbuf(sht_mouse, buf_mouse, 16, 16);

	init_mouse_cursor(sht_mouse);
	buf_mouse[1 * WINDOW_SCLINE(sht_mouse) + 10 * VMODE_WINDOW / 8 + 3] = 0xff;
	mx = (binfo->scrnx - 16) / 2;
	my = (binfo->scrny - 28 - 16) / 2;

	sheet_slide(sht_back, 0, 0);
	sheet_slide(sht_win, 200, 80);
	sheet_slide(sht_win2, 300, 300);
	sheet_slide(sht_win3, 500, 200);
	sheet_slide(sht_cons, 400, 10);
	sheet_slide(sht_mouse, mx, my);

	sheet_updown(sht_back,  0);
	sheet_updown(sht_win,   1);
	sheet_updown(sht_win2,  2);
	sheet_updown(sht_cons,  3);
	sheet_updown(sht_win3,  4);
	sheet_updown(sht_mouse, 5);

	/* 画面初期化 */
	putfontstr_sht(sht_back,  8,  8, white, back, "ABC 123");
	putfontstr(VMODE_WINDOW, buf_back, WINDOW_SCLINE(sht_back), 31, 31, black, "No Name OS.");
	putfontstr(VMODE_WINDOW, buf_back, WINDOW_SCLINE(sht_back), 30, 30, white, "No Name OS.");

	/* 時刻取得 */
	lt = readrtc();
	/* エラーがおきたらもう一度読む */
	if(lt.sec == 0xff) lt = readrtc();

	sprintf(s, "%02X:%02X", lt.hour, lt.min);
	putfontstr_sht(sht_back, binfo->scrnx - 45, binfo->scrny - 21, white, light_gray, s);

	if(memtotal / (1024 * 1024) >= 1024){
		sprintf(s, "memory %dMB / %dGB", memtotal / (1024 * 1024), memtotal / (1024 * 1024 * 1024));
	}else sprintf(s, "memory %dMB", memtotal / (1024 * 1024));
	putfontstr_sht(sht_back, 8, 48, white, back, s);

	putfontstr_sht(sht_back, 8, 80, white, back, "keydata: not found");

	sheet_refresh(sht_back, 0, 0, binfo->scrnx, 120);
	sheet_refresh(sht_back, binfo->scrnx - 45, binfo->scrny - 21, binfo->scrnx - 5, binfo->scrny - 5);

	buffer_put(&keycmd, TAG_KEYBOARD, KEYCMD_LED);
	buffer_put(&keycmd, TAG_KEYBOARD, key_leds);

	for(;;){
		if(buffer_status(&keycmd) > 0 && keycmd_wait < 0){
			keycmd_wait = buffer_get(&keycmd).data;
			wait_KBC_sendready();
			io_out8(PORT_KEYDAT, keycmd_wait);
		}
		io_cli();
		if(buffer_status(&buffer) == 0){
			task_sleep(task_a);
			io_sti();
		}else{
			/* バッファリード + 処理 */
			dat = buffer_get(&buffer);
			io_sti();

			if(dat.tag == TAG_KEYBOARD){
				boxfill(VMODE_WINDOW, buf_back, WINDOW_SCLINE(sht_back), back, 8, 80, 240, 80 + 16);
				sprintf(s, "keydata: %02X", dat.data);
				putfontstr(VMODE_WINDOW, buf_back, WINDOW_SCLINE(sht_back), 8, 80, white, s);
				/* keydata: not foundよりも短い文字列で書き換えるのでrefresh範囲を広めにとってrefresh */
				sheet_refresh(sht_back, 8, 80, 240, 96);
				if(dat.data < 0x80){
					if(key_shift == 0) s[0] = keytable0[dat.data];
					else s[0] = keytable1[dat.data];
				}else s[0] = 0;
				if('A' <= s[0] && s[0] <= 'Z'){
					if(((key_leds & 4) == 0 && key_shift == 0) || ((key_leds & 4) != 0 && key_shift != 0)){
						s[0] += 0x20;
					}
				}
				if(s[0] != 0){
					if(key_to == 0){
						if(cursor_x < 144){
							s[1] = 0;
							putfontstr_sht_ref(sht_win3, cursor_x, 28, black, white, s);
							cursor_x += 8;
						}
					}else buffer_put(&task_cons->buf, TAG_KEYBOARD, s[0]);
				}
				/* Back space */
				if(dat.data == 0x0e){
					if(key_to == 0){
						if(cursor_x > 8){
							putfontstr_sht_ref(sht_win3, cursor_x, 28, black, white, " ");
							cursor_x -= 8;
						}
					}else buffer_put(&task_cons->buf, TAG_KEYBOARD, 8);
				}
				if(dat.data == 0x1c){
					if(key_to != 0) buffer_put(&task_cons->buf, TAG_KEYBOARD, 10);
				}
				/* Tab */
				if(dat.data == 0x0f){
					if(key_to == 0){
						key_to = 1;
						make_wtitle(sht_win3, "textbox",  0);
						make_wtitle(sht_cons, "console",  1);
						cursor_c = invisible_cursor;
						boxfill(VMODE_WINDOW, sht_win3->buf, WINDOW_SCLINE(sht_win3), white, cursor_x, 28, cursor_x + 7, 43);
						sheet_refresh(sht_win3, cursor_x, 28, cursor_x + 8, 44);
						buffer_put(&task_cons->buf, 0, 2);
					}else{
						key_to = 0;
						make_wtitle(sht_win3, "textbox",  1);
						make_wtitle(sht_cons, "console",  0);
						cursor_c = black;
						buffer_put(&task_cons->buf, 0, 3);
					}
					sheet_refresh(sht_win3, 0, 0, sht_win3->bxsize, 21);
					sheet_refresh(sht_cons, 0, 0, sht_cons->bxsize, 21);
				}
				/* Left shift on */
				if(dat.data == 0x2a) key_shift |= 1;
				/* Right shift on */
				if(dat.data == 0x36) key_shift |= 2;
				/* Left shift off */
				if(dat.data == 0xaa) key_shift &= ~1;
				/* Right shift off */
				if(dat.data == 0xb6) key_shift &= ~2;
				/* CapsLock */
				if(dat.data == 0x3a){
					key_leds ^= 4;
					buffer_put(&keycmd, TAG_KEYBOARD, KEYCMD_LED);
					buffer_put(&keycmd, TAG_KEYBOARD, key_leds);
				}
				/* NumLock */
				if(dat.data == 0x45){
					key_leds ^= 2;
					buffer_put(&keycmd, TAG_KEYBOARD, KEYCMD_LED);
					buffer_put(&keycmd, TAG_KEYBOARD, key_leds);
				}
				/* ScrollLock */
				if(dat.data == 0x46){
					key_leds ^= 1;
					buffer_put(&keycmd, TAG_KEYBOARD, KEYCMD_LED);
					buffer_put(&keycmd, TAG_KEYBOARD, key_leds);
				}
				if(dat.data == 0x3b && key_shift != 0 && task_cons->tss.ss0 != 0){
					cons = (struct CONSOLE *)*((int *)0x0fec);
					cons_putstr0(cons, "\nBreak(key) :\n");
					io_cli();
					task_cons->tss.eax = (int)&(task_cons->tss.esp0);
					task_cons->tss.eip = (int)asm_end_app;
					io_sti();
				}
				if(dat.data == 0xfa) keycmd_wait = -1;
				if(dat.data == 0xfe){
					wait_KBC_sendready();
					io_out8(PORT_KEYDAT, keycmd_wait);
				}
				if(cursor_c.alpha != 0x00){
					boxfill(VMODE_WINDOW, buf_win3, WINDOW_SCLINE(sht_win3), cursor_c, cursor_x, 28, cursor_x + 7, 43);
					sheet_refresh(sht_win3, cursor_x, 28, cursor_x + 8, 44);
				}
			}
			if(dat.tag == TAG_RTC){
				lt = readrtc();
				/* エラーがおきたらもう一度読む */
				if(lt.sec == 0xff) lt = readrtc();
				sprintf(s, "%02X:%02X", lt.hour, lt.min);
				putfontstr_sht_ref(sht_back, binfo->scrnx - 45, binfo->scrny - 21, white, light_gray, s);
			}
			if(dat.tag == TAG_MOUSE){
				int decode = mouse_decode(&mdec, dat.data);

				if(decode == 0 && mdec.phase == 0 && dat.data == 0xfe){
					/* 初期化エラー */
				}

				if(decode == 1){
					sprintf(s, "[lcr %4d %4d]", mdec.x, mdec.y);
					if((mdec.btn & 0x01) != 0) s[1] = 'L';
					if((mdec.btn & 0x02) != 0) s[3] = 'R';
					if((mdec.btn & 0x04) != 0) s[2] = 'C';
					putfontstr_sht_ref(sht_back, 8, 116, white, back, s);

					if((mdec.btn & 0x01) != 0) sheet_slide(sht_win3, mx - 80, my - 8);

					/* カーソル移動 */
					mx += mdec.x;
					my += mdec.y;
					if(mx < 0) mx = 0;
					if(my < 0) my = 0;
					if(mx > binfo->scrnx - 1) mx = binfo->scrnx - 1;
					if(my > binfo->scrny - 1) my = binfo->scrny - 1;
					sprintf(s, "(%3d, %3d)", mx, my);
					putfontstr_sht_ref(sht_back, 8, 132, white, back, s);
					sheet_slide(sht_mouse, mx, my);
				}
			}
			if(dat.tag == TAG_TIMER){
				if(dat.data == 0 || dat.data == 1){
					if(cursor_c.alpha != 0x00){
						boxfill(VMODE_WINDOW, buf_win3, WINDOW_SCLINE(sht_win3), cursor_c, cursor_x, 28, cursor_x + 7, 43);
						cursor_c = dat.data == 0 ? black : white;
						sheet_refresh(sht_win3, cursor_x, 28, cursor_x + 8, 44);
					}
					timer_init(timer, &buffer, dat.data == 0 ? 1 : 0);
					timer_settime(timer, 50);
				}
			}
		}
	}
}
