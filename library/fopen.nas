[FORMAT "WCOFF"]
[INSTRSET "i486p"]
[BITS 32]
[FILE "fopen.nas"]

		GLOBAL	_api_fopen

[SECTION .text]

_api_fopen:			; int api_fopen(char *fname);
		push	ebx
		mov		edx,22
		mov		ebx,[esp+8]
		int		0x40
		pop		ebx
		ret
