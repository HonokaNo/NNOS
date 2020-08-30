[FORMAT "WCOFF"]
[INSTRSET "i486p"]
[BITS 32]
[FILE "fsize.nas"]

		GLOBAL	_api_fread

[SECTION .text]

_api_fread:			; int api_fread(char *buf, int maxsize, int fhandle);
		push	ebx
		mov		edx,26
		mov		ebx,[esp+8]
		mov		ecx,[esp+12]
		mov		eax,[esp+16]
		int		0x40
		pop		ebx
		ret
