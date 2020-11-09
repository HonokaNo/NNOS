#ifndef __NN_STDIO_H__
#define __NN_STDIO_H__

#include "d_stddef.h"

/* 絶対に1や2にはならない */
#define stdout	(FILE *)1
#define stdin	(FILE *)2

typedef struct
{
	int filehandle;
	/* 読み書き禁止:0 すでにクローズされている */
	/* rb:バイナリ読み込み = 1 */
	/* wb:バイナリ書き込み 長さ0にして書く = 2 */
	char flag;
}FILE;

int putchar(int c);
int printf(const char *format, ...);

FILE *fopen(const char *filename, const char *mode);
FILE *fclose(FILE *stream);
int fflush(FILE *stream);

#endif
