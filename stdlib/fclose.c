#include "d_stdio.h"
#include "nnos.h"

int *fclose(FILE *stream)
{
	stream->flag = 0;
	api_fclose(stream->filehandle);

	/* ���Ɉُ�I���͂Ȃ� */
	return 0;
}
