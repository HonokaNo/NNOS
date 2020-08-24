#ifndef __NNOS_HEADER__
#define __NNOS_HEADER__

struct color
{
	unsigned char r, g, b;
	char alpha;
};

void api_putchar(int c);
void api_putstr0(char *s);
void api_end(void);
int api_openwin(char *buf, int xsize, int ysize, char *title);
void api_putstrwin(int win, int x, int y, struct color *c, char *str);
void api_boxfilwin(int win, int x0, int y0, int x1, int y1, struct color *c);
void api_initmalloc(void);
char *api_malloc(int size);
void api_free(char *addr, int size);
void api_point(int win, int x, int y, struct color *c);
void api_refreshwin(int win, int x0, int y0, int x1, int y1);
void api_linewin(int win, int x0, int y0, int x1, int y1, struct color *c);
void api_closewin(int win);
int api_getkey(int mode);

#endif
