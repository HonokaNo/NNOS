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

void setPalette0(unsigned char *rgb, int start, int end)
{
	int eflags, i;

	eflags = io_load_eflags();
	io_cli();
	io_out8(0x03c8, start);
	for(i = start; i <= end; i++){
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

	setPalette0(table_rgb, 0, 15);

	unsigned char table2[216 * 3];
	int r, g, b;

	for(b = 0; b < 6; b++){
		for(g = 0; g < 6; g++){
			for(r = 0; r < 6; r++){
				table2[(r + g * 6 + b * 36) * 3 + 0] = r * 51;
				table2[(r + g * 6 + b * 36) * 3 + 1] = g * 51;
				table2[(r + g * 6 + b * 36) * 3 + 2] = b * 51;
			}
		}
	}

	setPalette0(table2, 16, 231);
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
	if(c.r == 0x00 && c.g == 0x00 && c.b == 0x00) return (char) 16;
	if(c.r == 0x33 && c.g == 0x00 && c.b == 0x00) return (char) 17;
	if(c.r == 0x66 && c.g == 0x00 && c.b == 0x00) return (char) 18;
	if(c.r == 0x99 && c.g == 0x00 && c.b == 0x00) return (char) 19;
	if(c.r == 0xcc && c.g == 0x00 && c.b == 0x00) return (char) 20;
	if(c.r == 0xff && c.g == 0x00 && c.b == 0x00) return (char) 21;
	if(c.r == 0x00 && c.g == 0x33 && c.b == 0x00) return (char) 22;
	if(c.r == 0x33 && c.g == 0x33 && c.b == 0x00) return (char) 23;
	if(c.r == 0x66 && c.g == 0x33 && c.b == 0x00) return (char) 24;
	if(c.r == 0x99 && c.g == 0x33 && c.b == 0x00) return (char) 25;
	if(c.r == 0xcc && c.g == 0x33 && c.b == 0x00) return (char) 26;
	if(c.r == 0xff && c.g == 0x33 && c.b == 0x00) return (char) 27;
	if(c.r == 0x00 && c.g == 0x66 && c.b == 0x00) return (char) 28;
	if(c.r == 0x33 && c.g == 0x66 && c.b == 0x00) return (char) 29;
	if(c.r == 0x66 && c.g == 0x66 && c.b == 0x00) return (char) 30;
	if(c.r == 0x99 && c.g == 0x66 && c.b == 0x00) return (char) 31;
	if(c.r == 0xcc && c.g == 0x66 && c.b == 0x00) return (char) 32;
	if(c.r == 0xff && c.g == 0x66 && c.b == 0x00) return (char) 33;
	if(c.r == 0x00 && c.g == 0x99 && c.b == 0x00) return (char) 34;
	if(c.r == 0x33 && c.g == 0x99 && c.b == 0x00) return (char) 35;
	if(c.r == 0x66 && c.g == 0x99 && c.b == 0x00) return (char) 36;
	if(c.r == 0x99 && c.g == 0x99 && c.b == 0x00) return (char) 37;
	if(c.r == 0xcc && c.g == 0x99 && c.b == 0x00) return (char) 38;
	if(c.r == 0xff && c.g == 0x99 && c.b == 0x00) return (char) 39;
	if(c.r == 0x00 && c.g == 0xcc && c.b == 0x00) return (char) 40;
	if(c.r == 0x33 && c.g == 0xcc && c.b == 0x00) return (char) 41;
	if(c.r == 0x66 && c.g == 0xcc && c.b == 0x00) return (char) 42;
	if(c.r == 0x99 && c.g == 0xcc && c.b == 0x00) return (char) 43;
	if(c.r == 0xcc && c.g == 0xcc && c.b == 0x00) return (char) 44;
	if(c.r == 0xff && c.g == 0xcc && c.b == 0x00) return (char) 45;
	if(c.r == 0x00 && c.g == 0xff && c.b == 0x00) return (char) 46;
	if(c.r == 0x33 && c.g == 0xff && c.b == 0x00) return (char) 47;
	if(c.r == 0x66 && c.g == 0xff && c.b == 0x00) return (char) 48;
	if(c.r == 0x99 && c.g == 0xff && c.b == 0x00) return (char) 49;
	if(c.r == 0xcc && c.g == 0xff && c.b == 0x00) return (char) 50;
	if(c.r == 0xff && c.g == 0xff && c.b == 0x00) return (char) 51;
	if(c.r == 0x00 && c.g == 0x00 && c.b == 0x33) return (char) 52;
	if(c.r == 0x33 && c.g == 0x00 && c.b == 0x33) return (char) 53;
	if(c.r == 0x66 && c.g == 0x00 && c.b == 0x33) return (char) 54;
	if(c.r == 0x99 && c.g == 0x00 && c.b == 0x33) return (char) 55;
	if(c.r == 0xcc && c.g == 0x00 && c.b == 0x33) return (char) 56;
	if(c.r == 0xff && c.g == 0x00 && c.b == 0x33) return (char) 57;
	if(c.r == 0x00 && c.g == 0x33 && c.b == 0x33) return (char) 58;
	if(c.r == 0x33 && c.g == 0x33 && c.b == 0x33) return (char) 59;
	if(c.r == 0x66 && c.g == 0x33 && c.b == 0x33) return (char) 60;
	if(c.r == 0x99 && c.g == 0x33 && c.b == 0x33) return (char) 61;
	if(c.r == 0xcc && c.g == 0x33 && c.b == 0x33) return (char) 62;
	if(c.r == 0xff && c.g == 0x33 && c.b == 0x33) return (char) 63;
	if(c.r == 0x00 && c.g == 0x66 && c.b == 0x33) return (char) 64;
	if(c.r == 0x33 && c.g == 0x66 && c.b == 0x33) return (char) 65;
	if(c.r == 0x66 && c.g == 0x66 && c.b == 0x33) return (char) 66;
	if(c.r == 0x99 && c.g == 0x66 && c.b == 0x33) return (char) 67;
	if(c.r == 0xcc && c.g == 0x66 && c.b == 0x33) return (char) 68;
	if(c.r == 0xff && c.g == 0x66 && c.b == 0x33) return (char) 69;
	if(c.r == 0x00 && c.g == 0x99 && c.b == 0x33) return (char) 70;
	if(c.r == 0x33 && c.g == 0x99 && c.b == 0x33) return (char) 71;
	if(c.r == 0x66 && c.g == 0x99 && c.b == 0x33) return (char) 72;
	if(c.r == 0x99 && c.g == 0x99 && c.b == 0x33) return (char) 73;
	if(c.r == 0xcc && c.g == 0x99 && c.b == 0x33) return (char) 74;
	if(c.r == 0xff && c.g == 0x99 && c.b == 0x33) return (char) 75;
	if(c.r == 0x00 && c.g == 0xcc && c.b == 0x33) return (char) 76;
	if(c.r == 0x33 && c.g == 0xcc && c.b == 0x33) return (char) 77;
	if(c.r == 0x66 && c.g == 0xcc && c.b == 0x33) return (char) 78;
	if(c.r == 0x99 && c.g == 0xcc && c.b == 0x33) return (char) 79;
	if(c.r == 0xcc && c.g == 0xcc && c.b == 0x33) return (char) 80;
	if(c.r == 0xff && c.g == 0xcc && c.b == 0x33) return (char) 81;
	if(c.r == 0x00 && c.g == 0xff && c.b == 0x33) return (char) 82;
	if(c.r == 0x33 && c.g == 0xff && c.b == 0x33) return (char) 83;
	if(c.r == 0x66 && c.g == 0xff && c.b == 0x33) return (char) 84;
	if(c.r == 0x99 && c.g == 0xff && c.b == 0x33) return (char) 85;
	if(c.r == 0xcc && c.g == 0xff && c.b == 0x33) return (char) 86;
	if(c.r == 0xff && c.g == 0xff && c.b == 0x33) return (char) 87;
	if(c.r == 0x00 && c.g == 0x00 && c.b == 0x66) return (char) 88;
	if(c.r == 0x33 && c.g == 0x00 && c.b == 0x66) return (char) 89;
	if(c.r == 0x66 && c.g == 0x00 && c.b == 0x66) return (char) 90;
	if(c.r == 0x99 && c.g == 0x00 && c.b == 0x66) return (char) 91;
	if(c.r == 0xcc && c.g == 0x00 && c.b == 0x66) return (char) 92;
	if(c.r == 0xff && c.g == 0x00 && c.b == 0x66) return (char) 93;
	if(c.r == 0x00 && c.g == 0x33 && c.b == 0x66) return (char) 94;
	if(c.r == 0x33 && c.g == 0x33 && c.b == 0x66) return (char) 95;
	if(c.r == 0x66 && c.g == 0x33 && c.b == 0x66) return (char) 96;
	if(c.r == 0x99 && c.g == 0x33 && c.b == 0x66) return (char) 97;
	if(c.r == 0xcc && c.g == 0x33 && c.b == 0x66) return (char) 98;
	if(c.r == 0xff && c.g == 0x33 && c.b == 0x66) return (char) 99;
	if(c.r == 0x00 && c.g == 0x66 && c.b == 0x66) return (char)100;
	if(c.r == 0x33 && c.g == 0x66 && c.b == 0x66) return (char)101;
	if(c.r == 0x66 && c.g == 0x66 && c.b == 0x66) return (char)102;
	if(c.r == 0x99 && c.g == 0x66 && c.b == 0x66) return (char)103;
	if(c.r == 0xcc && c.g == 0x66 && c.b == 0x66) return (char)104;
	if(c.r == 0xff && c.g == 0x66 && c.b == 0x66) return (char)105;
	if(c.r == 0x00 && c.g == 0x99 && c.b == 0x66) return (char)106;
	if(c.r == 0x33 && c.g == 0x99 && c.b == 0x66) return (char)107;
	if(c.r == 0x66 && c.g == 0x99 && c.b == 0x66) return (char)108;
	if(c.r == 0x99 && c.g == 0x99 && c.b == 0x66) return (char)109;
	if(c.r == 0xcc && c.g == 0x99 && c.b == 0x66) return (char)110;
	if(c.r == 0xff && c.g == 0x99 && c.b == 0x66) return (char)111;
	if(c.r == 0x00 && c.g == 0xcc && c.b == 0x66) return (char)112;
	if(c.r == 0x33 && c.g == 0xcc && c.b == 0x66) return (char)113;
	if(c.r == 0x66 && c.g == 0xcc && c.b == 0x66) return (char)114;
	if(c.r == 0x99 && c.g == 0xcc && c.b == 0x66) return (char)115;
	if(c.r == 0xcc && c.g == 0xcc && c.b == 0x66) return (char)116;
	if(c.r == 0xff && c.g == 0xcc && c.b == 0x66) return (char)117;
	if(c.r == 0x00 && c.g == 0xff && c.b == 0x66) return (char)118;
	if(c.r == 0x33 && c.g == 0xff && c.b == 0x66) return (char)119;
	if(c.r == 0x66 && c.g == 0xff && c.b == 0x66) return (char)120;
	if(c.r == 0x99 && c.g == 0xff && c.b == 0x66) return (char)121;
	if(c.r == 0xcc && c.g == 0xff && c.b == 0x66) return (char)122;
	if(c.r == 0xff && c.g == 0xff && c.b == 0x66) return (char)123;
	if(c.r == 0x00 && c.g == 0x00 && c.b == 0x99) return (char)124;
	if(c.r == 0x33 && c.g == 0x00 && c.b == 0x99) return (char)125;
	if(c.r == 0x66 && c.g == 0x00 && c.b == 0x99) return (char)126;
	if(c.r == 0x99 && c.g == 0x00 && c.b == 0x99) return (char)127;
	if(c.r == 0xcc && c.g == 0x00 && c.b == 0x99) return (char)128;
	if(c.r == 0xff && c.g == 0x00 && c.b == 0x99) return (char)129;
	if(c.r == 0x00 && c.g == 0x33 && c.b == 0x99) return (char)130;
	if(c.r == 0x33 && c.g == 0x33 && c.b == 0x99) return (char)131;
	if(c.r == 0x66 && c.g == 0x33 && c.b == 0x99) return (char)132;
	if(c.r == 0x99 && c.g == 0x33 && c.b == 0x99) return (char)133;
	if(c.r == 0xcc && c.g == 0x33 && c.b == 0x99) return (char)134;
	if(c.r == 0xff && c.g == 0x33 && c.b == 0x99) return (char)135;
	if(c.r == 0x00 && c.g == 0x66 && c.b == 0x99) return (char)136;
	if(c.r == 0x33 && c.g == 0x66 && c.b == 0x99) return (char)137;
	if(c.r == 0x66 && c.g == 0x66 && c.b == 0x99) return (char)138;
	if(c.r == 0x99 && c.g == 0x66 && c.b == 0x99) return (char)139;
	if(c.r == 0xcc && c.g == 0x66 && c.b == 0x99) return (char)140;
	if(c.r == 0xff && c.g == 0x66 && c.b == 0x99) return (char)141;
	if(c.r == 0x00 && c.g == 0x99 && c.b == 0x99) return (char)142;
	if(c.r == 0x33 && c.g == 0x99 && c.b == 0x99) return (char)143;
	if(c.r == 0x66 && c.g == 0x99 && c.b == 0x99) return (char)144;
	if(c.r == 0x99 && c.g == 0x99 && c.b == 0x99) return (char)145;
	if(c.r == 0xcc && c.g == 0x99 && c.b == 0x99) return (char)146;
	if(c.r == 0xff && c.g == 0x99 && c.b == 0x99) return (char)147;
	if(c.r == 0x00 && c.g == 0xcc && c.b == 0x99) return (char)148;
	if(c.r == 0x33 && c.g == 0xcc && c.b == 0x99) return (char)149;
	if(c.r == 0x66 && c.g == 0xcc && c.b == 0x99) return (char)150;
	if(c.r == 0x99 && c.g == 0xcc && c.b == 0x99) return (char)151;
	if(c.r == 0xcc && c.g == 0xcc && c.b == 0x99) return (char)152;
	if(c.r == 0xff && c.g == 0xcc && c.b == 0x99) return (char)153;
	if(c.r == 0x00 && c.g == 0xff && c.b == 0x99) return (char)154;
	if(c.r == 0x33 && c.g == 0xff && c.b == 0x99) return (char)155;
	if(c.r == 0x66 && c.g == 0xff && c.b == 0x99) return (char)156;
	if(c.r == 0x99 && c.g == 0xff && c.b == 0x99) return (char)157;
	if(c.r == 0xcc && c.g == 0xff && c.b == 0x99) return (char)158;
	if(c.r == 0xff && c.g == 0xff && c.b == 0x99) return (char)159;
	if(c.r == 0x00 && c.g == 0x00 && c.b == 0xcc) return (char)160;
	if(c.r == 0x33 && c.g == 0x00 && c.b == 0xcc) return (char)161;
	if(c.r == 0x66 && c.g == 0x00 && c.b == 0xcc) return (char)162;
	if(c.r == 0x99 && c.g == 0x00 && c.b == 0xcc) return (char)163;
	if(c.r == 0xcc && c.g == 0x00 && c.b == 0xcc) return (char)164;
	if(c.r == 0xff && c.g == 0x00 && c.b == 0xcc) return (char)165;
	if(c.r == 0x00 && c.g == 0x33 && c.b == 0xcc) return (char)166;
	if(c.r == 0x33 && c.g == 0x33 && c.b == 0xcc) return (char)167;
	if(c.r == 0x66 && c.g == 0x33 && c.b == 0xcc) return (char)168;
	if(c.r == 0x99 && c.g == 0x33 && c.b == 0xcc) return (char)169;
	if(c.r == 0xcc && c.g == 0x33 && c.b == 0xcc) return (char)170;
	if(c.r == 0xff && c.g == 0x33 && c.b == 0xcc) return (char)171;
	if(c.r == 0x00 && c.g == 0x66 && c.b == 0xcc) return (char)172;
	if(c.r == 0x33 && c.g == 0x66 && c.b == 0xcc) return (char)173;
	if(c.r == 0x66 && c.g == 0x66 && c.b == 0xcc) return (char)174;
	if(c.r == 0x99 && c.g == 0x66 && c.b == 0xcc) return (char)175;
	if(c.r == 0xcc && c.g == 0x66 && c.b == 0xcc) return (char)176;
	if(c.r == 0xff && c.g == 0x66 && c.b == 0xcc) return (char)177;
	if(c.r == 0x00 && c.g == 0x99 && c.b == 0xcc) return (char)178;
	if(c.r == 0x33 && c.g == 0x99 && c.b == 0xcc) return (char)179;
	if(c.r == 0x66 && c.g == 0x99 && c.b == 0xcc) return (char)180;
	if(c.r == 0x99 && c.g == 0x99 && c.b == 0xcc) return (char)181;
	if(c.r == 0xcc && c.g == 0x99 && c.b == 0xcc) return (char)182;
	if(c.r == 0xff && c.g == 0x99 && c.b == 0xcc) return (char)183;
	if(c.r == 0x00 && c.g == 0xcc && c.b == 0xcc) return (char)184;
	if(c.r == 0x33 && c.g == 0xcc && c.b == 0xcc) return (char)185;
	if(c.r == 0x66 && c.g == 0xcc && c.b == 0xcc) return (char)186;
	if(c.r == 0x99 && c.g == 0xcc && c.b == 0xcc) return (char)187;
	if(c.r == 0xcc && c.g == 0xcc && c.b == 0xcc) return (char)188;
	if(c.r == 0xff && c.g == 0xcc && c.b == 0xcc) return (char)189;
	if(c.r == 0x00 && c.g == 0xff && c.b == 0xcc) return (char)190;
	if(c.r == 0x33 && c.g == 0xff && c.b == 0xcc) return (char)191;
	if(c.r == 0x66 && c.g == 0xff && c.b == 0xcc) return (char)192;
	if(c.r == 0x99 && c.g == 0xff && c.b == 0xcc) return (char)193;
	if(c.r == 0xcc && c.g == 0xff && c.b == 0xcc) return (char)194;
	if(c.r == 0xff && c.g == 0xff && c.b == 0xcc) return (char)195;
	if(c.r == 0x00 && c.g == 0x00 && c.b == 0xff) return (char)196;
	if(c.r == 0x33 && c.g == 0x00 && c.b == 0xff) return (char)197;
	if(c.r == 0x66 && c.g == 0x00 && c.b == 0xff) return (char)198;
	if(c.r == 0x99 && c.g == 0x00 && c.b == 0xff) return (char)199;
	if(c.r == 0xcc && c.g == 0x00 && c.b == 0xff) return (char)200;
	if(c.r == 0xff && c.g == 0x00 && c.b == 0xff) return (char)201;
	if(c.r == 0x00 && c.g == 0x33 && c.b == 0xff) return (char)202;
	if(c.r == 0x33 && c.g == 0x33 && c.b == 0xff) return (char)203;
	if(c.r == 0x66 && c.g == 0x33 && c.b == 0xff) return (char)204;
	if(c.r == 0x99 && c.g == 0x33 && c.b == 0xff) return (char)205;
	if(c.r == 0xcc && c.g == 0x33 && c.b == 0xff) return (char)206;
	if(c.r == 0xff && c.g == 0x33 && c.b == 0xff) return (char)207;
	if(c.r == 0x00 && c.g == 0x66 && c.b == 0xff) return (char)208;
	if(c.r == 0x33 && c.g == 0x66 && c.b == 0xff) return (char)209;
	if(c.r == 0x66 && c.g == 0x66 && c.b == 0xff) return (char)210;
	if(c.r == 0x99 && c.g == 0x66 && c.b == 0xff) return (char)211;
	if(c.r == 0xcc && c.g == 0x66 && c.b == 0xff) return (char)212;
	if(c.r == 0xff && c.g == 0x66 && c.b == 0xff) return (char)213;
	if(c.r == 0x00 && c.g == 0x99 && c.b == 0xff) return (char)214;
	if(c.r == 0x33 && c.g == 0x99 && c.b == 0xff) return (char)215;
	if(c.r == 0x66 && c.g == 0x99 && c.b == 0xff) return (char)216;
	if(c.r == 0x99 && c.g == 0x99 && c.b == 0xff) return (char)217;
	if(c.r == 0xcc && c.g == 0x99 && c.b == 0xff) return (char)218;
	if(c.r == 0xff && c.g == 0x99 && c.b == 0xff) return (char)219;
	if(c.r == 0x00 && c.g == 0xcc && c.b == 0xff) return (char)220;
	if(c.r == 0x33 && c.g == 0xcc && c.b == 0xff) return (char)221;
	if(c.r == 0x66 && c.g == 0xcc && c.b == 0xff) return (char)222;
	if(c.r == 0x99 && c.g == 0xcc && c.b == 0xff) return (char)223;
	if(c.r == 0xcc && c.g == 0xcc && c.b == 0xff) return (char)224;
	if(c.r == 0xff && c.g == 0xcc && c.b == 0xff) return (char)225;
	if(c.r == 0x00 && c.g == 0xff && c.b == 0xff) return (char)226;
	if(c.r == 0x33 && c.g == 0xff && c.b == 0xff) return (char)227;
	if(c.r == 0x66 && c.g == 0xff && c.b == 0xff) return (char)228;
	if(c.r == 0x99 && c.g == 0xff && c.b == 0xff) return (char)229;
	if(c.r == 0xcc && c.g == 0xff && c.b == 0xff) return (char)230;
	if(c.r == 0xff && c.g == 0xff && c.b == 0xff) return (char)231;
	/* 未対応なのは黒 */
	return 0;
}

void putPixel8(char *vram, int scline, int x, int y, struct color c)
{
	vram[y * scline + x] = rgb2pal(c.r, c.g, c.b, x, y);
}

void putPixel16(char *vram, int scline, int x, int y, struct color c)
{
	short *p = &vram[y * scline + x * 16 / 8];
	short r = ((c.r >> 3) << 11) & 0xf800;
	short g = ((c.g >> 2) << 5) & 0x07e0;
	short b = (c.b >> 3) & 0x001f;
	short data = (r | g | b);

	p[0] = data;
}

void putPixel24(char *vram, int scline, int x, int y, struct color c)
{
	vram[y * scline + x * 24 / 8 + 0] = c.b;
	vram[y * scline + x * 24 / 8 + 1] = c.g;
	vram[y * scline + x * 24 / 8 + 2] = c.r;
}

void putPixel32(char *vram, int scline, int x, int y, struct color c)
{
	int *p = (int *)&vram[y * scline + x * 32 / 8];
	int col = (c.alpha << 24) | (c.r << 16) | (c.g << 8) | (c.b << 0);
	p[0] = col;
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
		for(; *s != 0; s++){
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

unsigned char rgb2pal(int r, int g, int b, int x, int y)
{
	static int table[4] = {3, 1, 0, 2};
	int i;

	x &= 1;
	y &= 1;
	i = table[x + y * 2];
	r = (r * 21) / 256;
	g = (g * 21) / 256;
	b = (b * 21) / 256;
	r = (r + i) / 4;
	g = (g + i) / 4;
	b = (b + i) / 4;

	return 16 + r + g * 6 + b * 36;
}
