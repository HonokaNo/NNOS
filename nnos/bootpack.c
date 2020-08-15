#include <stdio.h>
#include "bootpack.h"

struct BUFFER buffer;

void make_window(struct SHEET *sht, char *title);

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

void init_pic(void)
{
	/* 割り込み禁止 */
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

int mouse_decode(struct MOUSE_DEC *mdec, unsigned char dat);

void HariMain(void)
{
	struct color white = {0xff, 0xff, 0xff, 0xff};
	struct color black = {0x00, 0x00, 0x00, 0xff};
	struct color light_gray = {0xc6, 0xc6, 0xc6, 0xff};
	struct color back  = {0x00, 0x84, 0x84, 0xff};

	struct BOOTINFO *binfo = (struct BOOTINFO *)0x0ff0;
	struct localtime lt;
	char s[35];
	unsigned int memtotal;
	struct MEMMAN *memman = (struct MEMMAN *)MEMMAN_ADDR;;
	struct BUFDATA dat;
	struct SHTCTL *shtctl;
	struct SHEET *sht_back, *sht_win, *sht_mouse, *sht_win2;
	unsigned char *buf_back, *buf_win, buf_mouse[256 * 4], *buf_win2;
	struct MOUSE_DEC mdec;
	int mx, my;

	init_gdtidt();
	init_pic();

	/* rtc初期化 */
	io_out8(0x70, 0x0b);
	char prev = io_in8(0x71) | 0x10;
	io_out8(0x71, prev);

	io_sti();

	/* 割り込み有効化 11111001 */
	io_out8(PIC0_IMR, 0xf9);
	/* 割り込み有効化 11101110 */
	io_out8(PIC1_IMR, 0xee);

	buffer_init(&buffer, 200);

	init_keyboard();
	enable_mouse(&mdec);

	/* メモリ量取得 */
	memtotal = memtest(ADR_BOTPAK, 0xffffffff);
	memman_init(memman);
	memman_free(memman, 0x00001000, 0x0009e000);
	memman_free(memman, 0x00400000, memtotal - 0x00400000);

	if(binfo->vmode == 8) setPalette();

	shtctl = shtctl_init(memman, binfo->vram, binfo->scrnx, binfo->scrny);

	/* 背景 */
	sht_back = sheet_alloc(shtctl);
	buf_back = (unsigned char *)memman_alloc_4k(memman, binfo->scrnx * binfo->scrny * VMODE_WINDOW / 8);
	sheet_setbuf(sht_back, buf_back, binfo->scrnx, binfo->scrny);
	init_screen(VMODE_WINDOW, buf_back, WINDOW_SCLINE(sht_back), binfo->scrnx, binfo->scrny);

	/* ウィンドウ */
	sht_win = sheet_alloc(shtctl);
	buf_win = (unsigned char *)memman_alloc_4k(memman, 160 * 68 * VMODE_WINDOW / 8);
	sheet_setbuf(sht_win, buf_win, 160, 68);
	make_window(sht_win, "window");
	putfontstr(VMODE_WINDOW, buf_win, WINDOW_SCLINE(sht_win), 24, 28, black, "Welcome to");
	putfontstr(VMODE_WINDOW, buf_win, WINDOW_SCLINE(sht_win), 24, 44, black, "  No Name OS!");

	sht_win2 = sheet_alloc(shtctl);
	buf_win2 = (unsigned char *)memman_alloc_4k(memman, 160 * 68 * VMODE_WINDOW / 8);
	sheet_setbuf(sht_win2, buf_win2, 160, 68);
	make_window(sht_win2, "window2");
	putfontstr(VMODE_WINDOW, buf_win2, WINDOW_SCLINE(sht_win2), 24, 28, black, "Welcome to");
	putfontstr(VMODE_WINDOW, buf_win2, WINDOW_SCLINE(sht_win2), 24, 44, black, "  No Name OS!");

	/* マウス */
	sht_mouse = sheet_alloc(shtctl);
	sheet_setbuf(sht_mouse, buf_mouse, 16, 16);

	init_mouse_cursor(sht_mouse);
	buf_mouse[1 * WINDOW_SCLINE(sht_mouse) + 10 * VMODE_WINDOW / 8 + 3] = 0xff;
	mx = (binfo->scrnx - 16) / 2;
	my = (binfo->scrny - 28 - 16) / 2;

	sheet_slide(sht_back, 0, 0);
	sheet_slide(sht_win, 80, 80);
//	sheet_slide(sht_win2, 300, 300);
	sheet_slide(sht_mouse, mx, my);

	sheet_updown(sht_back,  0);
	sheet_updown(sht_win,   1);
//	sheet_updown(sht_win2,  2);
	sheet_updown(sht_mouse, 3);

	/* 画面初期化 */
	putfontstr(VMODE_WINDOW, buf_back, WINDOW_SCLINE(sht_back),  8,  8, white, "ABC 123");
	putfontstr(VMODE_WINDOW, buf_back, WINDOW_SCLINE(sht_back), 31, 31, black, "NNOS / No Name OS.");
	putfontstr(VMODE_WINDOW, buf_back, WINDOW_SCLINE(sht_back), 30, 30, white, "NNOS / No Name OS.");

	/* 時刻取得 */
	lt = readrtc();
	/* エラーがおきたらもう一度読む */
	if(lt.sec == 0xff) lt = readrtc();

	sprintf(s, "%02X:%02X", lt.hour, lt.min);
	putfontstr(VMODE_WINDOW, buf_back, WINDOW_SCLINE(sht_back), binfo->scrnx - 45, binfo->scrny - 21, white, s);

	if(memtotal / (1024 * 1024) >= 1024){
		sprintf(s, "memory %dMB / %dGB", memtotal / (1024 * 1024), memtotal / (1024 * 1024 * 1024));
	}else sprintf(s, "memory %dMB", memtotal / (1024 * 1024));
	putfontstr(VMODE_WINDOW, buf_back, WINDOW_SCLINE(sht_back), 8, 48, white, s);

	sprintf(s, "free memory %dKB", memman_total(memman) / 1024);
	putfontstr(VMODE_WINDOW, buf_back, WINDOW_SCLINE(sht_back), 8, 64, white, s);

	putfontstr(VMODE_WINDOW, buf_back, WINDOW_SCLINE(sht_back), 8, 80, white, "keydata: not found");

	sheet_refresh(sht_back, 0, 0, binfo->scrnx, 120);
	sheet_refresh(sht_back, binfo->scrnx - 45, binfo->scrny - 21, binfo->scrnx - 5, binfo->scrny - 5);
	sheet_refresh(sht_back, 400, 400, 550, 416);

	sheet_refresh(sht_back, 140, 120, 250, 240);

	for(;;){
		io_cli();
		if(buffer_status(&buffer) == 0) io_stihlt();
		else{
			/* バッファリード + 処理 */
			dat = buffer_get(&buffer);
			io_sti();

			if(dat.tag == TAG_KEYBOARD){
				boxfill(VMODE_WINDOW, buf_back, WINDOW_SCLINE(sht_back), back, 8, 80, 8 + 8 * 20, 80 + 16);
				sprintf(s, "keydata: %02X", dat.data);
				putfontstr(VMODE_WINDOW, buf_back, WINDOW_SCLINE(sht_back), 8, 80, white, s);
				sheet_refresh(sht_back, 8, 80, 168, 96);
			}
			if(dat.tag == TAG_RTC){
				boxfill(VMODE_WINDOW, buf_back, WINDOW_SCLINE(sht_back), light_gray, binfo->scrnx - 45, binfo->scrny - 21, binfo->scrnx - 45 + 8 * 5, binfo->scrny - 21 + 16);
				lt = readrtc();
				/* エラーがおきたらもう一度読む */
				if(lt.sec == 0xff) lt = readrtc();
				sprintf(s, "%02X:%02X", lt.hour, lt.min);
				putfontstr(VMODE_WINDOW, buf_back, WINDOW_SCLINE(sht_back), binfo->scrnx - 45, binfo->scrny - 21, white, s);
				sheet_refresh(sht_back, binfo->scrnx - 45, binfo->scrny - 21, binfo->scrnx - 5, binfo->scrny - 5);
			}
			if(dat.tag == TAG_MOUSE){
				int decode = mouse_decode(&mdec, dat.data);
				if(decode == 1){
					sprintf(s, "[lcr %4d %4d]", mdec.x, mdec.y);
					if((mdec.btn & 0x01) != 0) s[1] = 'L';
					if((mdec.btn & 0x02) != 0) s[3] = 'R';
					if((mdec.btn & 0x04) != 0) s[2] = 'C';
					send_string("start mouse interrupt\n");
					boxfill(VMODE_WINDOW, buf_back, WINDOW_SCLINE(sht_back), back, 8, 116, 8 + 15 * 8, 116 + 16);
					putfontstr(VMODE_WINDOW, buf_back, WINDOW_SCLINE(sht_back), 8, 116, white, s);
					sheet_refresh(sht_back, 8, 116, 8 + 15 * 8, 116 + 16);
					send_string("end mouse interrupt\n");

					if((mdec.btn & 0x01) != 0){
						sheet_slide(sht_win, mx, my);
					}

					/* カーソル移動 */
					mx += mdec.x;
					my += mdec.y;
					if(mx < 0) mx = 0;
					if(my < 0) my = 0;
					if(mx > binfo->scrnx - 1) mx = binfo->scrnx - 1;
					if(my > binfo->scrny - 1) my = binfo->scrny - 1;
					sprintf(s, "(%3d, %3d)", mx, my);
					boxfill(VMODE_WINDOW, buf_back, WINDOW_SCLINE(sht_back), back, 8, 132, 8 + 10 * 8, 136 + 16);
					putfontstr(VMODE_WINDOW, buf_back, WINDOW_SCLINE(sht_back), 8, 132, white, s);
					sheet_refresh(sht_back, 8, 132, 8 + 10 * 8, 136 + 16);
					sheet_slide(sht_mouse, mx, my);
				}
			}
		}
		boxfill(VMODE_WINDOW, buf_back, WINDOW_SCLINE(sht_back), back, 8, 100, 8 + 8 * 20, 100 + 16);
		sprintf(s, "%d %d", buffer.r, buffer.w);
		putfontstr(VMODE_WINDOW, buf_back, WINDOW_SCLINE(sht_back), 8, 100, white, s);
		sheet_refresh(sht_back, 8, 100, 64, 116);
		sheet_refresh(sht_back, 400, 400, 550, 416);
	}
}

void make_window(struct SHEET *sht, char *title)
{
	char b;
	struct color c;
	int x, y;

	static char closebtn[14][16] = {
		"OOOOOOOOOOOOOOO@",
		"OQQQQQQQQQQQQQ$@",
		"OQQQQQQQQQQQQQ$@",
		"OQQQ@@QQQQ@@QQ$@",
		"OQQQQ@@QQ@@QQQ$@",
		"OQQQQQ@@@@QQQQ$@",
		"OQQQQQQ@@QQQQQ$@",
		"OQQQQQ@@@@QQQQ$@",
		"OQQQQ@@QQ@@QQQ$@",
		"OQQQ@@QQQQ@@QQ$@",
		"OQQQQQQQQQQQQQ$@",
		"OQQQQQQQQQQQQQ$@",
		"O$$$$$$$$$$$$$$@",
		"@@@@@@@@@@@@@@@@"
	};
	static char sizebtn[14][16] = {
		"OOOOOOOOOOOOOOO@",
		"OQQQQQQQQQQQQQ$@",
		"OQQ@@@@@@@@@QQ$@",
		"OQQ@QQQQQQQ@QQ$@",
		"OQQ@QQQQQQQ@QQ$@",
		"OQQ@QQQQQQQ@QQ$@",
		"OQQ@QQQQQQQ@QQ$@",
		"OQQ@QQQQQQQ@QQ$@",
		"OQQ@QQQQQQQ@QQ$@",
		"OQQ@QQQQQQQ@QQ$@",
		"OQQ@@@@@@@@@QQ$@",
		"OQQQQQQQQQQQQQ$@",
		"O$$$$$$$$$$$$$$@",
		"@@@@@@@@@@@@@@@@"
	};

	struct color      black = {0x00, 0x00, 0x00, 0xff};
	struct color      white = {0xff, 0xff, 0xff, 0xff};
	struct color light_gray = {0xc6, 0xc6, 0xc6, 0xff};
	struct color  dark_gray = {0x84, 0x84, 0x84, 0xff};
	struct color  dark_blue = {0x00, 0x00, 0x84, 0xff};

	boxfill(VMODE_WINDOW, sht->buf, WINDOW_SCLINE(sht), light_gray,               0,               0, sht->bxsize - 1,               0);
	boxfill(VMODE_WINDOW, sht->buf, WINDOW_SCLINE(sht),      white,               1,               1, sht->bxsize - 2,               1);
	boxfill(VMODE_WINDOW, sht->buf, WINDOW_SCLINE(sht), light_gray,               0,               0,               0, sht->bysize - 1);
	boxfill(VMODE_WINDOW, sht->buf, WINDOW_SCLINE(sht),      white,               1,               1,               1, sht->bysize - 2);
	boxfill(VMODE_WINDOW, sht->buf, WINDOW_SCLINE(sht),  dark_gray, sht->bxsize - 2,               1, sht->bxsize - 2, sht->bysize - 2);
	boxfill(VMODE_WINDOW, sht->buf, WINDOW_SCLINE(sht),      black, sht->bxsize - 1,               0, sht->bxsize - 1, sht->bysize - 1);
	boxfill(VMODE_WINDOW, sht->buf, WINDOW_SCLINE(sht), light_gray,               2,               2, sht->bxsize - 3, sht->bysize - 3);
	boxfill(VMODE_WINDOW, sht->buf, WINDOW_SCLINE(sht),  dark_blue,               3,               3, sht->bxsize - 4,              20);
	boxfill(VMODE_WINDOW, sht->buf, WINDOW_SCLINE(sht),  dark_gray,               1, sht->bysize - 2, sht->bxsize - 2, sht->bysize - 2);
	boxfill(VMODE_WINDOW, sht->buf, WINDOW_SCLINE(sht),      black,               0, sht->bysize - 1, sht->bxsize - 1, sht->bysize - 1);
	putfontstr(VMODE_WINDOW, sht->buf, WINDOW_SCLINE(sht), 24, 4, white, title);

	for(y = 0; y < 14; y++){
		for(x = 0; x < 16; x++){
			b = closebtn[y][x];
			if(b == '@') c = black;
			else if(b == '$') c = dark_gray;
			else if(b == 'Q') c = light_gray;
			else c = white;
			putPixel(VMODE_WINDOW, sht->buf, WINDOW_SCLINE(sht), (sht->bxsize - 21 + x), (5 + y), c);
		}
	}

	for(y = 0; y < 14; y++){
		for(x = 0; x < 16; x++){
			b = sizebtn[y][x];
			if(b == '@') c = black;
			else if(b == '$') c = dark_gray;
			else if(b == 'Q') c = light_gray;
			else c = white;
			putPixel(VMODE_WINDOW, sht->buf, WINDOW_SCLINE(sht), (sht->bxsize - 38 + x), (5 + y), c);
		}
	}

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
