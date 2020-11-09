#ifndef __NN_STDIO_H__
#define __NN_STDIO_H__

#include "d_stddef.h"

/* ��΂�1��2�ɂ͂Ȃ�Ȃ� */
#define stdout	(FILE *)1
#define stdin	(FILE *)2

typedef struct
{
	int filehandle;
	/* �ǂݏ����֎~:0 ���łɃN���[�Y����Ă��� */
	/* rb:�o�C�i���ǂݍ��� = 1 */
	/* wb:�o�C�i���������� ����0�ɂ��ď��� = 2 */
	char flag;
}FILE;

int putchar(int c);
int printf(const char *format, ...);

FILE *fopen(const char *filename, const char *mode);
FILE *fclose(FILE *stream);
int fflush(FILE *stream);

#endif
