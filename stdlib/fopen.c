#include <string.h>
#include "d_stdio.h"
#include "nnos.h"

FILE *fopen(const char *filename, const char *mode)
{
	FILE file;

	if(!strcmp(mode, "rb")){
		/* ファイルはもちろん新規作成しない */
		file.filehandle = api_fopen(filename, 0);
		file.flag = 1;
	}else if(!strcmp(mode, "wb")){
		file.filehandle = api_fopen(filename, 1);
		file.flag = 2;
	/* 非対応モード */
	}else return NULL;

	/* ファイルが見つからなかったのでエラー */
	if(file.filehandle == 0) return NULL;

	return &file;
}
