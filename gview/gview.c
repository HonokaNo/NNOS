#include "nnos.h"

struct DLL_STRPICENV
{
	int work[64 * 1024 / 4];
};

struct RGB
{
	unsigned char b, g, r, t;
};

int info_BMP(struct DLL_STRPICENV *env, int *info, int size, char *fp);
int decode0_BMP(struct DLL_STRPICENV *env, int size, char *fp, int b_type, char *buf, int skip);

int info_JPEG(struct DLL_STRPICENV *env, int *info, int size, char *fp);
int decode0_JPEG(struct DLL_STRPICENV *env, int size, char *fp, int b_type, char *buf, int skip);

void error(char *s);

void HariMain(void)
{
	struct DLL_STRPICENV env;
	char filebuf[512 * 1024], winbuf[1040 * 805 * 4];
	char s[32], *p;
	int win, i, j, fsize, xsize, info[8];
	struct RGB picbuf[1024 * 768], *q;

	api_cmdline(s, 30);
	for(p = s; *p > ' '; p++);
	for(; *p == ' '; p++);

	i = api_fopen(p);
	if(i == 0) error("file not found.\n");
	fsize = api_fsize(i, 0);
	if(fsize > 512 * 1024) error("file too large");
	api_fread(filebuf, fsize, i);
	api_fclose(i);

	if(info_BMP(&env, info, fsize, filebuf) == 0){
		if(info_JPEG(&env, info, fsize, filebuf) == 0){
			api_putstr0("file type unknown.\n");
			api_end();
		}
	}

	/* info[0] ファイルタイプ */
	/* info[1] カラー情報 */
	/* info[2] xsize */
	/* info[3] ysize */

	if(info[2] > 1024 || info[3] > 768) error("picture too large.\n");

	xsize = info[2] + 16;
	if(xsize < 136) xsize = 136;
	win = api_openwin(winbuf, xsize, info[3] + 37, "gview");

	if(info[0] == 1) i = decode0_BMP(&env, fsize, filebuf, 4, (char *)picbuf, 0);
	else i = decode0_JPEG(&env, fsize, filebuf, 4, (char *)picbuf, 0);

	if(i != 0) error("decode error.\n");

	/* 表示 */
	int b = (xsize - info[2]) / 2;
	for(i = 0; i < info[3]; i++){
		for(j = 0; j < info[2]; j++){
			struct color c;
			q = picbuf + i * info[2];
			c.r = q[j].r;
			c.g = q[j].g;
			c.b = q[j].b;
			c.alpha = 0xff;

			api_point(win, b + j, i + 29, &c);
		}
	}

	for(;;){
		i = api_getkey(1);
		if(i == 'Q' || i == 'q') api_end();
	}
}

void error(char *s)
{
	api_putstr0(s);
	api_end();
}
