#include <stdio.h>
#include <string.h>
#include "bootpack.h"

struct BUFFER buffer;
struct localtime lt;

#define KEYCMD_LED	0xed

void keywin_off(struct SHEET *key_win);
void keywin_on(struct SHEET *key_win);
void close_constask(struct TASK *task);
void close_console(struct SHEET *sht);

struct rgb
{
	unsigned char b, g, r, t;
};

int info_JPEG(struct DLL_STRPICENV *env, int *info, int size, char *fp);
int decode0_JPEG(struct DLL_STRPICENV *env, int size, char *fp, int b_type, char *buf, int skip);
int info_BMP(struct DLL_STRPICENV *env, int *info, int size, char *fp);
int decode0_BMP(struct DLL_STRPICENV *env, int size, char *fp, int b_type, char *buf, int skip);

#define DEBUG 0

void HariMain(void)
{
	struct color white = {0xff, 0xff, 0xff, 0xff};
	struct color black = {0x00, 0x00, 0x00, 0xff};
	struct color light_gray = {0xc6, 0xc6, 0xc6, 0xff};
	struct color back24  = {0x99, 0xd9, 0xea, 0xff};
	struct color back8 = {0x00, 0x84, 0x84, 0xff};
	struct color back, c;

	struct BOOTINFO *binfo = (struct BOOTINFO *)ADR_BOOTINFO;
	char s[35];
	unsigned int memtotal;
	struct MEMMAN *memman = (struct MEMMAN *)MEMMAN_ADDR;;
	struct BUFDATA dat;
	struct SHTCTL *shtctl;
	struct SHEET *sht_back, *sht_mouse;
	unsigned char *buf_back, buf_mouse[256 * 4];
	struct MOUSE_DEC mdec;
	int mx, my;
	struct TASK *task_a, *task;
	struct BUFFER keycmd;
	int key_shift = 0, key_leds = (binfo->leds >> 4) & 7, keycmd_wait = -1;
	int x, y, j, mmx = -0x7fffffff, mmy = -0x7fffffff;
	struct SHEET *sht = 0, *key_win, *sht2;
	struct FILEINFO *finfo;
	int i, *fat;
	unsigned char *nihongo;
	extern char hankaku[4096];
	int fsiz, info[8], bgx, bgy;
	char *bbuf;
	struct rgb *ctable;
	struct DLL_STRPICENV env;

	static char keytable0[0x80] = {
		0,   0,   '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '-', '^', 0x08, 0,
		'Q', 'W', 'E', 'R', 'T', 'Y', 'U', 'I', 'O', 'P', '@', '[', 0x0a, 0,  'A', 'S',
		'D', 'F', 'G', 'H', 'J', 'K', 'L', ';', ':', 0,   0,   ']', 'Z', 'X', 'C', 'V',
		'B', 'N', 'M', ',', '.', '/', 0,   '*', 0,   ' ', 0,   0,   0,   0,   0,   0,
		0,   0,   0,   0,   0,   0,   0,   '7', '8', '9', '-', '4', '5', '6', '+', '1',
		'2', '3', '0', '.', 0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
		0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
		0,   0,   0,   0x5c, 0,  0,   0,   0,   0,   0,   0,   0,   0,   0x5c, 0,  0
	};

	static char keytable1[0x80] = {
		0,   0,   '!', 0x22, '#', '$', '%', '&', 0x27, '(', ')', '~', '=', '~', 0x08, 0,
		'Q', 'W', 'E', 'R', 'T', 'Y', 'U', 'I', 'O', 'P', '`', '{', 0x0a, 0,  'A', 'S',
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

	*((int *)0x0fec) = (int)&buffer;

	init_pit();

	/* メモリ量取得 */
	memtotal = memtest(ADR_BOTPAK, 0xffffffff);
	memman_init(memman);
	memman_free(memman, 0x00001000, 0x0009e000);
	memman_free(memman, 0x00400000, memtotal - 0x00400000);

	fat = (int *)memman_alloc_4k(memman, 2880 * 4);
	file_readfat(fat, (unsigned char *)(ADR_DISKIMG + 0x0200));

	if(binfo->vmode == 8){
		setPalette();
		back = back8;
	}else back = back24;

	init_timerctl();

	init_keyboard();
	enable_mouse(&mdec);

	shtctl = shtctl_init(memman, binfo->vram, binfo->scrnx, binfo->scrny);
	*((int *)0x0fe4) = (int)shtctl;

	/* 背景 */
	sht_back = sheet_alloc(shtctl);
	buf_back = (unsigned char *)memman_alloc_4k(memman, binfo->scrnx * binfo->scrny * 4);
	sheet_setbuf(sht_back, buf_back, binfo->scrnx, binfo->scrny);
	init_screen(VMODE_WINDOW, buf_back, WINDOW_SCLINE(sht_back), binfo->scrnx, binfo->scrny, back);

	if(!DEBUG){
		finfo = file_search("backg.ima", (struct FILEINFO *)(ADR_DISKIMG + 0x2600), 224);
		fsiz = finfo->size;
		bbuf = file_loadfile2(finfo->clustno, &fsiz, fat);
		ctable = (struct rgb *)memman_alloc_4k(memman, sizeof(struct color) * binfo->scrnx * (binfo->scrny - 26));
		i = info_JPEG(&env, info, fsiz, bbuf);

		if(i == 0) info_BMP(&env, info, fsiz, bbuf);

		if(i != 0){
			/* info[2] = width info[3] = height */
			if(info[2] == binfo->scrnx && info[3] == binfo->scrny - 26){
				if(info[0] == 1) decode0_BMP(&env, fsiz, bbuf, 4, (char *)ctable, 0);
				else i = decode0_JPEG(&env, fsiz, bbuf, 4, (char *)ctable, 0);
				if(i == 0){
					for(bgy = 0; bgy < binfo->scrny - 26; bgy++){
						for(bgx = 0; bgx < binfo->scrnx; bgx++){
							struct color c;
							c.r = ctable[bgy * binfo->scrnx + bgx].r;
							c.g = ctable[bgy * binfo->scrnx + bgx].g;
							c.b = ctable[bgy * binfo->scrnx + bgx].b;
							c.alpha = 0xff;

							putPixel(VMODE_WINDOW, buf_back, WINDOW_SCLINE(sht_back), bgx, bgy, c);
						}
					}
				}
			}
		}
		memman_free_4k(memman, (int)bbuf, fsiz);
		memman_free_4k(memman, (int)ctable, binfo->scrnx * (binfo->scrny - 26) * 4);
	}

	task_a = task_init(memman);
	buffer.task = task_a;
	task_run(task_a, 1, 2);
	task_a->langmode = 0;

	/* マウス */
	sht_mouse = sheet_alloc(shtctl);
	sheet_setbuf(sht_mouse, buf_mouse, 16, 16);

	init_mouse_cursor(sht_mouse);
	buf_mouse[1 * WINDOW_SCLINE(sht_mouse) + 10 * VMODE_WINDOW / 8 + 3] = 0xff;
	mx = (binfo->scrnx - 16) / 2;
	my = (binfo->scrny - 28 - 16) / 2;

	key_win = open_console(shtctl, memtotal, fat);

	if(binfo->vmode == 24 || binfo->vmode == 32){
		sheet_slide(sht_back,    0,   0);
		sheet_slide(key_win,   400,  10);
		sheet_slide(sht_mouse,  mx,  my);

		sheet_updown(sht_back,  0);
		sheet_updown(key_win,   1);
		sheet_updown(sht_mouse, 2);
	}else{
		sheet_slide(sht_back,   0,  0);
		sheet_slide(key_win,   56,  6);
		sheet_slide(sht_mouse, mx, my);

		sheet_updown(sht_back,  0);
		sheet_updown(key_win,   1);
		sheet_updown(sht_mouse, 2);
	}

	keywin_on(key_win);

	/* 画面初期化 */
	if(DEBUG){
		putfontstr_sht(sht_back,  8,  8, white, back, "ABC 123");
		putfontstr(VMODE_WINDOW, buf_back, WINDOW_SCLINE(sht_back), 31, 31, black, "No Name OS.");
		putfontstr(VMODE_WINDOW, buf_back, WINDOW_SCLINE(sht_back), 30, 30, white, "No Name OS.");
	}

	/* 時刻取得 */
	lt = readrtc();
	/* エラーがおきたらもう一度読む */
	if(lt.sec == 0xff) lt = readrtc();

	sprintf(s, "%02d:%02d", lt.hour, lt.min);
	putfontstr_sht(sht_back, binfo->scrnx - 45, binfo->scrny - 21, white, light_gray, s);

	if(DEBUG){
		if(memtotal / (1024 * 1024) >= 1024){
			sprintf(s, "memory %dMB / %dGB", memtotal / (1024 * 1024), memtotal / (1024 * 1024 * 1024));
		}else sprintf(s, "memory %dMB", memtotal / (1024 * 1024));
		putfontstr_sht(sht_back, 8, 48, white, back, s);

		putfontstr_sht(sht_back, 8, 80, white, back, "keydata: not found");

		sheet_refresh(sht_back, 0, 0, binfo->scrnx, 120);
	}

	sheet_refresh(sht_back, binfo->scrnx - 45, binfo->scrny - 21, binfo->scrnx - 5, binfo->scrny - 5);

	buffer_put(&keycmd, TAG_KEYBOARD, KEYCMD_LED);
	buffer_put(&keycmd, TAG_KEYBOARD, key_leds);

	/* nihongo.fntの読み込み */
	finfo = file_search("nihongo.fnt", (struct FILEINFO *)(ADR_DISKIMG + 0x2600), 224);
	if(finfo != 0){
		i = finfo->size;
		nihongo = file_loadfile2(finfo->clustno, &i, fat);
	}else{
		nihongo = (unsigned char *)memman_alloc_4k(memman, 16 * 256 + 32 * 94 * 47);
		/* 本来英語フォントの部分は英語フォント */
		for(i = 0; i < 16 * 256; i++){
			nihongo[i] = hankaku[i];
		}

		/* フォントがない部分は0xffで埋め尽くす */
		for(i = 16 * 256; i < 16 * 256 + 32 * 94 * 47; i++){
			nihongo[i] = 0xff;
		}
	}
	*((int *)0x0fe8) = (int)nihongo;

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

			if(key_win != 0 && key_win->flags == 0){
				if(shtctl->top == 1) key_win = 0;
				else{
					key_win = shtctl->sheets[shtctl->top - 1];
					keywin_on(key_win);
				}
			}

			if(dat.tag == TAG_KEYBOARD){
				if(DEBUG){
					boxfill(VMODE_WINDOW, buf_back, WINDOW_SCLINE(sht_back), back, 8, 80, 240, 80 + 16);
					sprintf(s, "keydata: %02X", dat.data);
					putfontstr(VMODE_WINDOW, buf_back, WINDOW_SCLINE(sht_back), 8, 80, white, s);
					/* keydata: not foundよりも短い文字列で書き換えるのでrefresh範囲を広めにとってrefresh */
					sheet_refresh(sht_back, 8, 80, 240, 96);
				}

				if(dat.data < 0x80){
					if(key_shift == 0) s[0] = keytable0[dat.data];
					else s[0] = keytable1[dat.data];
				}else s[0] = 0;
				if('A' <= s[0] && s[0] <= 'Z'){
					if(((key_leds & 4) == 0 && key_shift == 0) || ((key_leds & 4) != 0 && key_shift != 0)){
						s[0] += 0x20;
					}
				}
				if(s[0] != 0 && key_win != 0){
					buffer_put(&key_win->task->buf, TAG_KEYBOARD, s[0]);
				}
				/* Tab */
				if(dat.data == 0x0f && key_win != 0){
					keywin_off(key_win);
					j = key_win->height - 1;
					if(j == 0) j = shtctl->top - 1;
					key_win = shtctl->sheets[j];
					keywin_on(key_win);
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
				/* Shift + F1 */
				if(dat.data == 0x3b && key_shift != 0 && key_win != 0){
					task = key_win->task;
					if(task != 0 && task->tss.ss0 != 0){
						cons_putstr0(task->cons, "\nBreak(key) :\n");
						io_cli();
						task->tss.eax = (int)&(task->tss.esp0);
						task->tss.eip = (int)asm_end_app;
						io_sti();
						task_run(task, -1, 0);
					}
				}
				/* Shift + F2 */
				if(dat.data == 0x3c && key_shift != 0){
					if(key_win != 0) keywin_off(key_win);
					key_win = open_console(shtctl, memtotal, fat);
					sheet_slide(key_win, 32, 4);
					sheet_updown(key_win, shtctl->top);
					keywin_on(key_win);
				}
				/* F11 */
				if(dat.data == 0x57 && shtctl->top > 2){
					sheet_updown(shtctl->sheets[1], shtctl->top - 1);
				}

				if(dat.data == 0xfa) keycmd_wait = -1;
				if(dat.data == 0xfe){
					wait_KBC_sendready();
					io_out8(PORT_KEYDAT, keycmd_wait);
				}
			}
			if(dat.tag == TAG_RTC){
				lt = readrtc();
				/* エラーがおきたらもう一度読む */
				if(lt.sec == 0xff) lt = readrtc();
				sprintf(s, "%02d:%02d", lt.hour, lt.min);
				putfontstr_sht_ref(sht_back, binfo->scrnx - 45, binfo->scrny - 21, white, light_gray, s);
			}
			if(dat.tag == TAG_MOUSE){
				int decode = mouse_decode(&mdec, dat.data);

				if(decode == 0 && mdec.phase == 0 && dat.data == 0xfe){
					/* 初期化エラー */
				}

				if(decode == 1){
					if(DEBUG){
						sprintf(s, "[lcr %4d %4d]", mdec.x, mdec.y);
						if((mdec.btn & 0x01) != 0) s[1] = 'L';
						if((mdec.btn & 0x02) != 0) s[3] = 'R';
						if((mdec.btn & 0x04) != 0) s[2] = 'C';
						putfontstr_sht_ref(sht_back, 8, 116, white, back, s);
					}

					/* カーソル移動 */
					mx += mdec.x;
					my += mdec.y;
					if(mx < 0) mx = 0;
					if(my < 0) my = 0;
					if(mx > binfo->scrnx - 1) mx = binfo->scrnx - 1;
					if(my > binfo->scrny - 1) my = binfo->scrny - 1;
					if(DEBUG){
						sprintf(s, "(%3d, %3d)", mx, my);
						putfontstr_sht_ref(sht_back, 8, 132, white, back, s);
					}
					sheet_slide(sht_mouse, mx, my);

					if((mdec.btn & 0x01) != 0){
						if(mmx < 0){
							for(j = shtctl->top - 1; j > 0; j--){
								sht = shtctl->sheets[j];
								x = mx - sht->vx0;
								y = my - sht->vy0;
								if(0 <= x && x < sht->bxsize && 0 <= y && y < sht->bysize){
									c = getColorWin(sht, x, y);
									if(c.alpha != 0x00){
										sheet_updown(sht, shtctl->top - 1);
										if(sht != key_win){
											keywin_off(key_win);
											key_win = sht;
											keywin_on(key_win);
										}
										if(3 <= x && x <= sht->bxsize - 3 && 3 <= y && y < 38){
											/* ウィンドウを移動する */
											mmx = mx;
											mmy = my;
										}
										if(key_win->bxsize - 38 <= x && x < key_win->bxsize - 22 && 5 <= y && y < 19){
											/* 最大化ボタン */
											if(sht->bxsize != binfo->scrnx && sht->bysize != binfo->scrny - 26){
												task = sht->task;

												memman_free_4k(memman, (int)sht->buf, sht->bxsize * (sht->bysize - 26) * 4);
												char *buf2 = (unsigned char *)memman_alloc_4k(memman, (binfo->scrnx * (binfo->scrny - 26) * 4));
												sheet_setbuf(sht, buf2, binfo->scrnx, binfo->scrny - 26);
												make_window(sht, sht->title, 0);

												sheet_slide(sht, 0, 0);
												/* 256 165 */
												make_textbox(sht, 8, 28, sht->bxsize - 16, sht->bysize - 35, black);
												putfontstr_sht(sht, 8, 28, white, black, ">");

												io_sti();
												buffer_put(&sht->task->buf, 0, 5);
												io_cli();

												sheet_refreshsub(sht_back->ctl, 0, 0, binfo->scrnx, binfo->scrny, 0, sht_back->ctl->top);

												task_run(task, -1, 0);
											}
										}
										if(sht->bxsize - 21 <= x && x < sht->bxsize - 5 && 5 <= y && y < 19){
											/* ×ボタンを押された */
											/* アプリで作られたウィンドウのみ */
											if((sht->flags & 0x10) != 0){
												task = sht->task;
												cons_putstr0(task->cons, "\nBreak(mouse) :\n");
												io_cli();
												task->tss.eax = (int)&(task->tss.esp0);
												task->tss.eip = (int)asm_end_app;
												io_sti();
												task_run(task, -1, 0);
											}else{
												task = sht->task;
												sheet_updown(sht, -1);
												keywin_off(key_win);
												key_win = shtctl->sheets[shtctl->top - 1];
												keywin_on(key_win);
												io_cli();
												buffer_put(&task->buf, 0, 4);
												io_sti();
											}
										}
										break;
									}
								}
							}
						}else{
							x = mx - mmx;
							y = my - mmy;
							sheet_slide(sht, sht->vx0 + x, sht->vy0 + y);
							mmx = mx;
							mmy = my;
						}
					}else mmx = -0x7fffffff;
				}
			}
			if(dat.tag == TAG_CONSOLE){
				if(dat.data >= 2024) {
					sht2 = shtctl->sheets0 + (dat.data - 2024);
					memman_free_4k(memman, (int)sht2->buf, 256 * 165 * 4);
					sheet_free(sht2);
				}else if(dat.data >= 1024) close_constask(taskctl->tasks0 + (dat.data - 1024));
				else close_console(shtctl->sheets0 + dat.data);
			}
		}
	}
}

void keywin_off(struct SHEET *key_win)
{
	change_wtitle(key_win, 0);
	if((key_win->flags & 0x20) != 0){
		buffer_put(&key_win->task->buf, 0, 3);
	}
	return;
}

void keywin_on(struct SHEET *key_win)
{
	change_wtitle(key_win, 1);
	if((key_win->flags & 0x20) != 0){
		buffer_put(&key_win->task->buf, 0, 2);
	}
	return;
}

struct TASK *open_constask(struct SHEET *sht, unsigned int memtotal, int *fat)
{
	struct MEMMAN *memman = (struct MEMMAN *)MEMMAN_ADDR;
	struct TASK *task = task_alloc();

	task->cons_stack = memman_alloc_4k(memman, 64 * 1024);
	task->tss.esp = task->cons_stack + 64 * 1024 - 12;
	task->tss.eip = (int)&console_task;
	task->tss.es = 1 * 8;
	task->tss.cs = 2 * 8;
	task->tss.ss = 1 * 8;
	task->tss.ds = 1 * 8;
	task->tss.fs = 1 * 8;
	task->tss.gs = 1 * 8;
	*((int *)(task->tss.esp + 4)) = (int)sht;
	*((int *)(task->tss.esp + 8)) = memtotal;
	task_run(task, 2, 2);
	buffer_init(&task->buf, 128, task);
	task->fat = fat;

	return task;
}

struct SHEET *open_console(struct SHTCTL *shtctl, unsigned int memtotal, int *fat)
{
	struct MEMMAN *memman = (struct MEMMAN *)MEMMAN_ADDR;
	struct SHEET *sht = sheet_alloc(shtctl);
	unsigned char *buf = (unsigned char *)memman_alloc_4k(memman, 256 * 165 * 4);
	struct color black = {0x00, 0x00, 0x00, 0xff};

	sheet_setbuf(sht, buf, 256, 165);
	make_window(sht, "console", 0);
	make_textbox(sht, 8, 28, 240, 128, black);
	sht->task = open_constask(sht, memtotal, fat);
	sht->flags |= 0x20;

	return sht;
}

void close_constask(struct TASK *task)
{
	struct MEMMAN *memman = (struct MEMMAN *)MEMMAN_ADDR;
	task_sleep(task);
	memman_free_4k(memman, task->cons_stack, 64 * 1024);
	memman_free_4k(memman, (int)task->buf.buf, 128 * 4);
	task->flags = 0;
	return;
}

void close_console(struct SHEET *sht)
{
	struct MEMMAN *memman = (struct MEMMAN *)MEMMAN_ADDR;
	struct TASK *task = sht->task;
	memman_free_4k(memman, (int)sht->buf, 256 * 165 * 4);
	sheet_free(sht);
	close_constask(task);
	return;
}
