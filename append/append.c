#include "nnos.h"

void HariMain(void)
{
	const char *append = "append!\n";
	int fh, i;

	fh = api_fopen("euc.txt", 0);

	api_fwrite(append, 8, fh);

	api_fseek(fh, 0, FSEEK_END);

	api_fwrite(append, 8, fh);

	api_end();
}
