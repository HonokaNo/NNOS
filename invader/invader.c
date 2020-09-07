#include <string.h>
#include "nnos.h"

void putstr(int win, char *winbuf, int x, int y, struct color col, unsigned char *s);
void wait(int i, int timer, char *keyflag);
void setdec(char *s, int i);

static unsigned char charset[16 * 8] = {
	/* invader 0 */
	0x00, 0x00, 0x00, 0x43, 0x5f, 0x5f, 0x5f, 0x7f,
	0x1f, 0x1f, 0x1f, 0x1f, 0x00, 0x20, 0x3f, 0x00,
	/* invader 1 */
	0x00, 0x00, 0x7f, 0xff, 0xcf, 0xcf, 0xcf, 0xff,
	0xff, 0xef, 0xff, 0xff, 0xc0, 0xc0, 0xc0, 0x00,
	/* invader 2 */
	0x00, 0x00, 0xfe, 0xff, 0xf3, 0xf3, 0xf3, 0xff,
	0xff, 0x07, 0xff, 0xff, 0x03, 0x03, 0x03, 0x00,
	/* invader 3 */
	0x00, 0x00, 0x00, 0xc2, 0xfa, 0xfa, 0xfa, 0xfe,
	0xf8, 0xf8, 0xf8, 0xf8, 0x00, 0x04, 0xfc, 0x00,
	/* fighter 0 */
	0x00, 0x00, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01,
	0x01, 0x43, 0x47, 0x4f, 0x5f, 0x7f, 0x7f, 0x00,
	/* fighter 1 */
	0x18, 0x7e, 0xff, 0xc3, 0xc3, 0xc3, 0xc3, 0xff,
	0xff, 0xff, 0xe7, 0xe7, 0xe7, 0xe7, 0xff, 0x00,
	/* fighter 2 */
	0x00, 0x00, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
	0x80, 0xc2, 0xe2, 0xf2, 0xfa, 0xfe, 0xfe, 0x00,
	/* laser */
	0x00, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18,
	0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x00
};
/* invader:"abcd" fighter:"efg" laser:"h" */

void HariMain(void)
{
	struct color black = {0x00, 0x00, 0x00, 0xff};
	struct color white = {0xff, 0xff, 0xff, 0xff};
	struct color red = {0xff, 0x00, 0x00, 0xff};
	struct color blue = {0x00, 0x00, 0xff, 0xff};
	struct color yellow = {0xff, 0xff, 0x00, 0xff};
	struct color aqua = {0x00, 0xff, 0xff, 0xff};

	int win, timer, i, j, fx, laserwait, lx = 0, ly;
	int ix, iy, movewait0, movewait, idir;
	int invline, score, high, point;
	char winbuf[336 * 261 * 4], invstr[32 * 6], s[12], keyflag[4], *p;
	static char invstr0[32] = " abcd abcd abcd abcd abcd ";

	win = api_openwin(winbuf, 336, 261, "invader", 0);
	api_boxfilwin(win, 6, 27, 329, 254, &black);
	timer = api_alloctimer();
	api_inittimer(timer, 128);

	high = 0;
	putstr(win, winbuf, 22, 0, white, "HIGH:00000000");

restart:
	score = 0;
	point = 1;
	putstr(win, winbuf, 4, 0, white, "SCORE:00000000");
	movewait0 = 20;
	fx = 18;
	putstr(win, winbuf, fx, 13, aqua, "efg");
	wait(100, timer, keyflag);

next_group:
	wait(100, timer, keyflag);
	ix = 7;
	iy = 1;
	invline = 6;
	for(i = 0; i < 6; i++){
		for(j = 0; j < 27; j++) invstr[i * 32 + j] = invstr0[j];
		putstr(win, winbuf, ix, iy + i, blue, invstr + i * 32);
	}
	keyflag[0] = 0;
	keyflag[1] = 0;
	keyflag[2] = 0;

	ly = 0;
	laserwait = 0;
	movewait = movewait0;
	idir = +1;
	wait(100, timer, keyflag);

	for(;;){
		if(laserwait != 0){
			laserwait--;
			keyflag[2] = 0;
		}

		wait(4, timer, keyflag);

		if(keyflag[0] != 0 && fx > 0){
			fx--;
			putstr(win, winbuf, fx, 13, aqua, "efg ");
			keyflag[0] = 0;
		}
		if(keyflag[1] != 0 && fx < 37){
			putstr(win, winbuf, fx, 13, aqua, " efg");
			fx++;
			keyflag[1] = 0;
		}
		if(keyflag[2] != 0 && laserwait == 0){
			laserwait = 15;
			lx = fx + 1;
			ly = 13;
		}

		if(movewait != 0) movewait--;
		else{
			movewait = movewait0;
			if(ix + idir > 14 || ix + idir < 0){
				if(iy + invline == 13) break;
				idir = -idir;
				putstr(win, winbuf, ix + 1, iy, black, "                         ");
				iy++;
			}else ix += idir;

			for(i = 0; i < invline; i++){
				putstr(win, winbuf, ix, iy + i, blue, invstr + i * 32);
			}
		}

		if(ly > 0){
			if(ly < 13){
				if(ix < lx && lx < ix + 25 && iy <= ly && ly < iy + invline){
					putstr(win, winbuf, ix, ly, blue, invstr + (ly - iy) * 32);
				}else{
					putstr(win, winbuf, lx, ly, black, " ");
				}
			}
			ly--;
			if(ly > 0) putstr(win, winbuf, lx, ly, yellow, "h");
			else{
				point -= 10;
				if(point <= 0) point = 1;
			}
			if(ix < lx && lx < ix + 25 && iy <= ly && ly < iy + invline){
				p = invstr + (ly - iy) * 32 + (lx - ix);
				if(*p != ' '){
					score += point;
					point++;
					setdec(s, score);
					putstr(win, winbuf, 10, 0, white, s);
					if(high < score){
						high = score;
						putstr(win, winbuf, 27, 0, white, s);
					}
					for(p--; *p != ' '; p--);
					for(i = 1; i < 5; i++) p[i] = ' ';
					putstr(win, winbuf, ix, ly, blue, invstr + (ly - iy) * 32);
					for(; invline > 0; invline--){
						for(p = invstr + (invline - 1) * 32; *p != 0; p++){
							if(*p != ' ') goto alive;
						}
					}
					movewait0 -= movewait / 3;
					goto next_group;
alive:
					ly = 0;
				}
			}
		}
	}

	putstr(win, winbuf, 15, 6, red, "GAME OVER");
	wait(0, timer, keyflag);
	for(i = 1; i < 14; i++){
		putstr(win, winbuf, 0, i, black, "                                        ");
	}

	goto restart;
}

void putstr(int win, char *winbuf, int x, int y, struct color col, unsigned char *s)
{
	struct color black = {0x00, 0x00, 0x00, 0xff};
	int x0, i;
	char *p, t[2], c = 0;

	x = x * 8 + 8;
	y = y * 16 + 29;
	x0 = x;
	i = strlen(s);
	api_boxfilwin(win + 1, x, y, x + i * 8 - 1, y + 15, &black);
	t[1] = 0;

	for(;;){
		c = *s;
		if(c == 0) break;
		if(c != ' '){
			if('a' <= c && c <= 'h'){
				p = charset + 16 * (c - 'a');
				for(i = 0; i < 16; i++){
					if((p[i] & 0x80) != 0) api_point(win, x + 0, y + i, &col);
					if((p[i] & 0x40) != 0) api_point(win, x + 1, y + i, &col);
					if((p[i] & 0x20) != 0) api_point(win, x + 2, y + i, &col);
					if((p[i] & 0x10) != 0) api_point(win, x + 3, y + i, &col);
					if((p[i] & 0x08) != 0) api_point(win, x + 4, y + i, &col);
					if((p[i] & 0x04) != 0) api_point(win, x + 5, y + i, &col);
					if((p[i] & 0x02) != 0) api_point(win, x + 6, y + i, &col);
					if((p[i] & 0x01) != 0) api_point(win, x + 7, y + i, &col);
				}
			}else{
				t[0] = c;
				api_putstrwin(win + 1, x, y, &col, s);
			}
		}
		s++;
		x += 8;
	}
	api_refreshwin(win, x0, y, x, y + 16);
	return;
}

void wait(int i, int timer, char *keyflag)
{
	int j;
	if(i > 0){
		api_settime(timer, i);
		i = 128;
	}else i = 0x0a;

	for(;;){
		j = api_gettimer(0);
		if(i == 128 && i == j) break;
		j = api_getkey(0);
		if(i == 0x0a && i == j) break;

		if(j == '4') keyflag[0] = 1;
		if(j == '6') keyflag[1] = 1;
		if(j == ' ') keyflag[2] = 1;
	}

	return;
}

void setdec(char *s, int i)
{
	int j;
	for(j = 7; j >= 0; j--){
		s[j] = '0' + i % 10;
		i /= 10;
	}
	s[8] = 0;
	return;
}
