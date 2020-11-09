#include <string.h>
#include "d_stdio.h"
#include "nnos.h"

FILE *fopen(const char *filename, const char *mode)
{
	FILE file;

	if(!strcmp(mode, "rb")){
		/* �t�@�C���͂������V�K�쐬���Ȃ� */
		file.filehandle = api_fopen(filename, 0);
		file.flag = 1;
	}else if(!strcmp(mode, "wb")){
		file.filehandle = api_fopen(filename, 1);
		file.flag = 2;
	/* ��Ή����[�h */
	}else return NULL;

	/* �t�@�C����������Ȃ������̂ŃG���[ */
	if(file.filehandle == 0) return NULL;

	return &file;
}
