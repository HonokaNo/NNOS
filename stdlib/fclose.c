#include "d_stdio.h"
#include "nnos.h"

int *fclose(FILE *stream)
{
	stream->flag = 0;
	api_fclose(stream->filehandle);

	/* 特に異常終了はなし */
	return 0;
}
