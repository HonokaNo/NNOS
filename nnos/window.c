#include "bootpack.h"
#include <string.h>

void make_wtitle(struct SHEET *sht, char *title, char act)
{
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
	char b;
	int x, y;

	struct color c, tc, tbc;
	struct color      white = {0xff, 0xff, 0xff, 0xff};
	struct color      black = {0x00, 0x00, 0x00, 0xff};
	struct color  dark_blue = {0x00, 0x00, 0x84, 0xff};
	struct color light_gray = {0xc6, 0xc6, 0xc6, 0xff};
	struct color  dark_gray = {0x84, 0x84, 0x84, 0xff};

	if(act != 0){
		tc = white;
		tbc = dark_blue;
	}else{
		tc = light_gray;
		tbc = dark_gray;
	}

	boxfill(VMODE_WINDOW, sht->buf, WINDOW_SCLINE(sht), tbc, 3, 3, sht->bxsize - 4, 20);
	putfontstr_sht(sht, 24, 4, tc, tbc, title);

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

void make_window(struct SHEET *sht, char *title, char act)
{
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
	make_wtitle(sht, title, act);

	return;
}

void putfontstr_sht(struct SHEET *sht, int x, int y, struct color c, struct color bc, char *s)
{
	int l = strlen(s);

	boxfill(VMODE_WINDOW, sht->buf, WINDOW_SCLINE(sht), bc, x, y, x + 8 * l - 1, y + 15);
	putfontstr(VMODE_WINDOW, sht->buf, WINDOW_SCLINE(sht), x, y, c, s);
}

void putfontstr_sht_ref(struct SHEET *sht, int x, int y, struct color c, struct color bc, char *s)
{
	int l = strlen(s);

	putfontstr_sht(sht, x, y, c, bc, s);
	sheet_refresh(sht, x, y, x + 8 * l, y + 16);
}

void make_textbox(struct SHEET *sht, int x0, int y0, int sx, int sy, struct color c)
{
	struct color white = {0xff, 0xff, 0xff, 0xff};
	struct color black = {0x00, 0x00, 0x00, 0xff};
	struct color light_gray = {0xc6, 0xc6, 0xc6, 0xff};
	struct color dark_gray = {0x84, 0x84, 0x84, 0xff};
	int x1 = x0 + sx, y1 = y0 + sy;

	boxfill(VMODE_WINDOW, sht->buf, WINDOW_SCLINE(sht),  dark_gray, x0 - 2, y0 - 3, x1 + 1, y0 - 3);
	boxfill(VMODE_WINDOW, sht->buf, WINDOW_SCLINE(sht),  dark_gray, x0 - 3, y0 - 3, x0 - 3, y1 + 1);
	boxfill(VMODE_WINDOW, sht->buf, WINDOW_SCLINE(sht),      white, x0 - 3, y1 + 2, x1 + 1, y1 + 2);
	boxfill(VMODE_WINDOW, sht->buf, WINDOW_SCLINE(sht),      white, x1 + 2, y0 - 3, x1 + 2, y1 + 2);
	boxfill(VMODE_WINDOW, sht->buf, WINDOW_SCLINE(sht),      black, x0 - 1, y0 - 2, x1 + 0, y0 - 2);
	boxfill(VMODE_WINDOW, sht->buf, WINDOW_SCLINE(sht),      black, x0 - 2, y0 - 2, x0 - 2, y1 + 0);
	boxfill(VMODE_WINDOW, sht->buf, WINDOW_SCLINE(sht), light_gray, x0 - 2, y1 + 1, x1 + 0, y1 + 1);
	boxfill(VMODE_WINDOW, sht->buf, WINDOW_SCLINE(sht), light_gray, x1 + 1, y0 - 2, x1 + 1, y1 + 1);
	boxfill(VMODE_WINDOW, sht->buf, WINDOW_SCLINE(sht),          c, x0 - 1, y0 - 1, x1 + 0, y1 + 0);

	return;
}

struct color getColorWin(struct SHEET *sht, int bx, int by)
{
	struct color c;

	c.b = sht->buf[by * WINDOW_SCLINE(sht) + bx * VMODE_WINDOW / 8 + 0];
	c.g = sht->buf[by * WINDOW_SCLINE(sht) + bx * VMODE_WINDOW / 8 + 1];
	c.r = sht->buf[by * WINDOW_SCLINE(sht) + bx * VMODE_WINDOW / 8 + 2];
	c.alpha = sht->buf[by * WINDOW_SCLINE(sht) + bx * VMODE_WINDOW / 8 + 3];

	return c;
}

void change_wtitle(struct SHEET *sht, char act)
{
	struct color white = {0xff, 0xff, 0xff, 0xff};
	struct color light_gray = {0xc6, 0xc6, 0xc6, 0xff};
	struct color  dark_gray = {0x84, 0x84, 0x84, 0xff};
	struct color  dark_blue = {0x00, 0x00, 0x84, 0xff};

	int x, y, xsize = sht->bxsize;
	struct color c, tc_new, tbc_new, tc_old, tbc_old;

	if(act != 0){
		tc_new = white;
		tbc_new = dark_blue;
		tc_old = light_gray;
		tbc_old = dark_gray;
	}else{
		tc_new = light_gray;
		tbc_new = dark_gray;
		tc_old = white;
		tbc_old = dark_blue;
	}

	for(y = 3; y <= 20; y++){
		for(x = 3; x <= xsize; x++){
			c = getColorWin(sht, x, y);
			if(eqColor(c, tbc_old) && y <= 5 && x <= xsize - 4) c = tbc_new;
			if(eqColor(c, tbc_old) && y >= 18 && x <= xsize - 4) c = tbc_new;
			if(eqColor(c, tbc_old) && y > 5 && y < 18 && x <= xsize - 38) c = tbc_new;
			if(eqColor(c, tbc_old) && y > 5 && y < 18 && x >= xsize - 22 && x <= xsize - 21) c = tbc_new;
			if(eqColor(c, tbc_old) && y > 5 && y < 18 && x >= xsize - 5 && x <= xsize - 3) c = tbc_new;
			if(eqColor(c, tc_old) && x <= xsize - 45) c = tc_new;
			putPixel(VMODE_WINDOW, sht->buf, WINDOW_SCLINE(sht), x, y, c);
		}
	}
	sheet_refresh(sht, 3, 3, xsize, 21);
	return;
}
