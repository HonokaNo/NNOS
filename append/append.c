#include "nnos.h"

void HariMain(void)
{
	const char *append = "append!\n";
	int fh, i;

	fh = api_fopen("euc.txt", 0);

	api_fseek(fh, 2, FSEEK_SET);

	api_fwrite(append, 8, fh);

	api_end();
}
