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

#endif
