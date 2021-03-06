#ifndef __NNOS_HEADER__
#define __NNOS_HEADER__

struct color
{
	unsigned char r, g, b;
	char alpha;
};

struct time
{
	unsigned char y0, y1;
	unsigned char month, day;
	unsigned char hour, min, sec;
};

struct ret
{
	short v0, v1;
};

void api_putchar(int c);
void api_putstr0(char *s);
void api_putstr1(char *s, int len);

void api_end(void);

int api_openwin(char *buf, int xsize, int ysize, char *title, char resize);
void api_putstrwin(int win, int x, int y, struct color *c, char *str);
void api_boxfilwin(int win, int x0, int y0, int x1, int y1, struct color *c);
void api_point(int win, int x, int y, struct color *c);
void api_refreshwin(int win, int x0, int y0, int x1, int y1);
void api_linewin(int win, int x0, int y0, int x1, int y1, struct color *c);
void api_closewin(int win);

void api_initmalloc(void);
char *api_malloc(int size);
void api_free(char *addr, int size);

int api_getkey(int mode);

int api_alloctimer(void);
void api_inittimer(int timer, int data);
void api_settime(int timer, int time);
void api_freetimer(int timer);
int api_gettimer(int mode);

void api_beep(int tone);

#define FSEEK_SET		0
#define FSEEK_CUR		1
#define FSEEK_END		2

<<<<<<< HEAD
int api_fopen(char *fname);
=======
int api_fopen(char *fname, int create);
>>>>>>> developer
int api_fclose(int fhandle);
int api_fseek(int fhandle, int offset, int mode);
int api_fsize(int fhandle, int mode);
int api_fread(char *buf, int maxsize, int fhandle);
int api_fwrite(char *buf, int size, int fhandle);

int api_cmdline(char *buf, int maxsize);

#define LANGMODE_ASCII		0
#define LANGMODE_SJIS		1
#define LANGMODE_EUC		2

int api_getlang(void);
void api_getclock(struct time *time);
void api_getwindata(int win, int mode, struct ret *ret);

void api_exec(char *arg);

#endif
