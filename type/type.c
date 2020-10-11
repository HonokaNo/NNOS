#include "nnos.h"

void HariMain(void)
{
	int fh;
	char c, cmdline[20], *p;

/*	api_cmdline(cmdline, 20);

	for(p = cmdline; *p != ' '; p++);
	for(; *p == ' '; p++);
	fh = api_fopen(p);
	if(fh != 0){
		for(;;){
			if(api_fread(&c, 1, fh) == 0) break;
			api_putchar(c);
		}
	}else api_putstr0("File not found.");*/

	fh = api_fopen("test.txt");

	cmdline[0] = 'H';
	cmdline[1] = 'e';
	cmdline[2] = 'l';
	cmdline[3] = 'l';
	cmdline[4] = 'o';
	cmdline[5] = ',';
	cmdline[6] = ' ';
	cmdline[7] = 'w';
	cmdline[8] = 'o';
	cmdline[9] = 'r';
	cmdline[10] = 'l';
	cmdline[11] = 'd';
	cmdline[12] = '.';

	api_fwrite(cmdline, 12, fh);

	api_end();
}
