#include "bootpack.h"

void init_screen(char vmode, char *vram, int scline, int scrnx, int scrny, struct color back)
{
	struct color light_gray = {0xc6, 0xc6, 0xc6, 0xff};
	struct color  dark_gray = {0x84, 0x84, 0x84, 0xff};
	struct color      white = {0xff, 0xff, 0xff, 0xff};
	struct color      black = {0x00, 0x00, 0x00, 0xff};

	boxfill(vmode, vram, scline,       back,          0,          0, scrnx -  1, scrny - 29);
	boxfill(vmode, vram, scline, light_gray,          0, scrny - 28, scrnx -  1, scrny - 28);
	boxfill(vmode, vram, scline,      white,          0, scrny - 27, scrnx -  1, scrny - 27);
	boxfill(vmode, vram, scline, light_gray,          0, scrny - 26, scrnx -  1, scrny -  1);

	boxfill(vmode, vram, scline,      white,          3, scrny - 24,         59, scrny - 24);
	boxfill(vmode, vram, scline,      white,          2, scrny - 24,          2, scrny -  4);
	boxfill(vmode, vram, scline,  dark_gray,          3, scrny -  4,         59, scrny -  4);
	boxfill(vmode, vram, scline,  dark_gray,         59, scrny - 23,         59, scrny -  5);
	boxfill(vmode, vram, scline,      black,          2, scrny -  3,         59, scrny -  3);
	boxfill(vmode, vram, scline,      black,         60, scrny - 24,         60, scrny -  3);

	boxfill(vmode, vram, scline,  dark_gray, scrnx - 47, scrny - 24, scrnx -  4, scrny - 24);
	boxfill(vmode, vram, scline,  dark_gray, scrnx - 47, scrny - 23, scrnx - 47, scrny -  4);
	boxfill(vmode, vram, scline,      white, scrnx - 47, scrny -  3, scrnx -  4, scrny -  3);
	boxfill(vmode, vram, scline,      white, scrnx -  3, scrny - 24, scrnx -  3, scrny -  3);
}

void setPalette0(unsigned char *rgb)
{
	int eflags, i;

	eflags = io_load_eflags();
	io_cli();
	io_out8(0x03c8, 0);
	for(i = 0; i < 16; i++){
		io_out8(0x03c9, rgb[0] / 4);
		io_out8(0x03c9, rgb[1] / 4);
		io_out8(0x03c9, rgb[2] / 4);
		rgb += 3;
	}
	io_store_eflags(eflags);
	return;
}

void setPalette(void)
{
	static unsigned char table_rgb[16 * 3] = {
		0x00, 0x00, 0x00,
		0xff, 0x00, 0x00,
		0x00, 0xff, 0x00,
		0xff, 0xff, 0x00,
		0x00, 0x00, 0xff,
		0xff, 0x00, 0xff,
		0x00, 0xff, 0xff,
		0xff, 0xff, 0xff,
		0xc6, 0xc6, 0xc6,
		0x84, 0x00, 0x00,
		0x00, 0x84, 0x00,
		0x84, 0x84, 0x00,
		0x00, 0x00, 0x84,
		0x84, 0x00, 0x84,
		0x00, 0x84, 0x84,
		0x84, 0x84, 0x84
	};
	setPalette0(table_rgb);
	return;
}

char col2pal(struct color c)
{
	if(c.r == 0x00 && c.g == 0x00 && c.b == 0x00) return (char)  0;
	if(c.r == 0xff && c.g == 0x00 && c.b == 0x00) return (char)  1;
	if(c.r == 0x00 && c.g == 0xff && c.b == 0x00) return (char)  2;
	if(c.r == 0xff && c.g == 0xff && c.b == 0x00) return (char)  3;
	if(c.r == 0x00 && c.g == 0x00 && c.b == 0xff) return (char)  4;
	if(c.r == 0xff && c.g == 0x00 && c.b == 0xff) return (char)  5;
	if(c.r == 0x00 && c.g == 0xff && c.b == 0xff) return (char)  6;
	if(c.r == 0xff && c.g == 0xff && c.b == 0xff) return (char)  7;
	if(c.r == 0xc6 && c.g == 0xc6 && c.b == 0xc6) return (char)  8;
	if(c.r == 0x84 && c.g == 0x00 && c.b == 0x00) return (char)  9;
	if(c.r == 0x00 && c.g == 0x84 && c.b == 0x00) return (char) 10;
	if(c.r == 0x84 && c.g == 0x84 && c.b == 0x00) return (char) 11;
	if(c.r == 0x00 && c.g == 0x00 && c.b == 0x84) return (char) 12;
	if(c.r == 0x84 && c.g == 0x00 && c.b == 0x84) return (char) 13;
	if(c.r == 0x00 && c.g == 0x84 && c.b == 0x84) return (char) 14;
	if(c.r == 0x84 && c.g == 0x84 && c.b == 0x84) return (char) 15;
	/* 未対応なのは黒 */
	return 0;
}

void putPixel8(char *vram, int scline, int x, int y, struct color c)
{
	vram[y * scline + x] = col2pal(c);
}

void putPixel16(char *vram, int scline, int x, int y, struct color c)
{
	unsigned char r = c.r & 0x1f, g = c.g & 0x3f, b = c.b & 0x1f;
	short data = ((b << 0) | (g << 5) | (r << 11));
	char d1 = data & 0xff, d2 = (data >> 8) & 0xff;

	vram[y * scline + x * 16 / 8 + 0] = d1;
	vram[y * scline + x * 16 / 8 + 1] = d2;
}

void putPixel24(char *vram, int scline, int x, int y, struct color c)
{
	vram[y * scline + x * 24 / 8 + 0] = c.b;
	vram[y * scline + x * 24 / 8 + 1] = c.g;
	vram[y * scline + x * 24 / 8 + 2] = c.r;
}

void putPixel32(char *vram, int scline, int x, int y, struct color c)
{
	vram[y * scline + x * 32 / 8 + 0] = c.b;
	vram[y * scline + x * 32 / 8 + 1] = c.g;
	vram[y * scline + x * 32 / 8 + 2] = c.r;
	vram[y * scline + x * 32 / 8 + 3] = c.alpha;
}

void putPixel(char vmode, char *vram, int scline, int x, int y, struct color c)
{
	if(vmode == 8) putPixel8(vram, scline, x, y, c);
	else if(vmode == 16) putPixel16(vram, scline, x, y, c);
	else if(vmode == 24) putPixel24(vram, scline, x, y, c);
	else if(vmode == 32) putPixel32(vram, scline, x, y, c);
}

void boxfill(char vmode, char *vram, int scline, struct color c, int x0, int y0, int x1, int y1)
{
	int x, y;

	for(y = y0; y <= y1; y++){
		for(x = x0; x <= x1; x++){
			putPixel(vmode, vram, scline, x, y, c);
		}
	}
}

void putfont(char vmode, char *vram, int scline, int x, int y, struct color c, char *font)
{
	int i;
	char d;

	for(i = 0; i < 16; i++){
		d = font[i];
		if((d & 0x80) != 0) putPixel(vmode, vram, scline, x + 0, y + i, c);
		if((d & 0x40) != 0) putPixel(vmode, vram, scline, x + 1, y + i, c);
		if((d & 0x20) != 0) putPixel(vmode, vram, scline, x + 2, y + i, c);
		if((d & 0x10) != 0) putPixel(vmode, vram, scline, x + 3, y + i, c);
		if((d & 0x08) != 0) putPixel(vmode, vram, scline, x + 4, y + i, c);
		if((d & 0x04) != 0) putPixel(vmode, vram, scline, x + 5, y + i, c);
		if((d & 0x02) != 0) putPixel(vmode, vram, scline, x + 6, y + i, c);
		if((d & 0x01) != 0) putPixel(vmode, vram, scline, x + 7, y + i, c);
	}
	return;
}

void putfontstr(char vmode, char *vram, int scline, int x, int y, struct color c, unsigned char *s)
{
	extern char hankaku[4096];
	struct TASK *task = task_now();
	char *nihongo = (char *)*((int *)0x0fe8), *font;
	int k, t;

	if(task->langmode == 0){
		for(; *s != 0x00; s++){
			putfont(vmode, vram, scline, x, y, c, hankaku + *s * 16);
			x += 8;
		}
	}
	if(task->langmode == 1){
		if(task->langbyte1 == 0){
			if((0x81 <= *s && *s <= 0x9f) || (0xe0 <= *s && *s <= 0xfc)){
				task->langbyte1 = *s;
			}else{
				putfont(vmode, vram, scline, x, y, c, nihongo + *s * 16);
			}
		}else{
			if(0x81 <= task->langbyte1 && task->langbyte1 <= 0x9f){
				k = (task->langbyte1 - 0x81) * 2;
			}else{
				k = (task->langbyte1 - 0xe0) * 2 + 62;
			}
			if(0x40 <= *s && *s <= 0x7e){
				t = *s - 0x40;
			}else if(0x80 <= *s && *s <= 0x9e){
				t = *s - 0x80 + 63;
			}else{
				t = *s - 0x9f;
				k++;
			}
			task->langbyte1 = 0;
			font = nihongo + 256 * 16 + (k * 94 + t) * 32;
			putfont(vmode, vram, scline, x - 8, y, c, font     );
			putfont(vmode, vram, scline, x    , y, c, font + 16);
		}
		x += 8;
	}
	if(task->langmode == 2){
		for(; *s != 0x00; s++){
			if(task->langbyte1 == 0){
				if(0x81 <= *s && *s <= 0xfe) task->langbyte1 = *s;
				else putfont(vmode, vram, scline, x, y, c, nihongo + *s * 16);
			}else{
				k = task->langbyte1 - 0xa1;
				t = *s - 0xa1;
				task->langbyte1 = 0;
				font = nihongo + 256 * 16 + (k * 94 + t) * 32;
				putfont(vmode, vram, scline, x - 8, y, c, font     );
				putfont(vmode, vram, scline, x    , y, c, font + 16);
			}
			x += 8;
		}
	}
	return;
}

void init_mouse_cursor(struct SHEET *sht)
{
	int x, y;

	static char cursor[16][16] = {
		"**************..",
		"*OOOOOOOOOOO*...",
		"*OOOOOOOOOO*....",
		"*OOOOOOOOO*.....",
		"*OOOOOOOO*......",
		"*OOOOOOO*.......",
		"*OOOOOOO*.......",
		"*OOOOOOOO*......",
		"*OOOO**OOO*.....",
		"*OOO*..*OOO*....",
		"*OO*....*OOO*...",
		"*O*......*OOO*..",
		"**........*OOO*.",
		"*..........*OOO*",
		"............*OO*",
		".............***"
	};

	struct color black = {0x00, 0x00, 0x00, 0xff};
	struct color white = {0xff, 0xff, 0xff, 0xff};
	struct color back = {0x00, 0x84, 0x84, 0x00};

	for(y = 0; y < 16; y++){
		for(x = 0; x < 16; x++){
			if(cursor[y][x] == '*') putPixel(VMODE_WINDOW, sht->buf, WINDOW_SCLINE(sht), x, y, black);
			if(cursor[y][x] == 'O') putPixel(VMODE_WINDOW, sht->buf, WINDOW_SCLINE(sht), x, y, white);
			if(cursor[y][x] == '.') putPixel(VMODE_WINDOW, sht->buf, WINDOW_SCLINE(sht), x, y, back);
		}
	}
	return;
}

int eqColor(struct color c1, struct color c2)
{
	/* アルファ値が両方0なら両方透明なので一緒だよね */
	if(c1.alpha == 0x00 && c2.alpha == 0x00) return 1;

	if(c1.r == c2.r && c1.g == c2.g && c1.b == c2.b) return 1;

	return 0;
}
