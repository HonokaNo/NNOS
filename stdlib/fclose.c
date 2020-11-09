#include "d_stdio.h"
#include "nnos.h"

int *fclose(FILE *stream)
{
	stream->flag = 0;
	api_fclose(stream->filehandle);

	/* “Á‚ÉˆÙíI—¹‚Í‚È‚µ */
	return 0;
}
