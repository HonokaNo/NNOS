[FORMAT "WCOFF"]
[INSTRSET "i486p"]
[BITS 32]
[FILE "fopen.nas"]

		GLOBAL	_api_fopen

[SECTION .text]

_api_fopen:			; int api_fopen(char *fname, int create);
		push	ebx
		push	esi
		mov		edx,22
		mov		ebx,[esp+12]
		mov		esi,[esp+16]
		int		0x40
		pop		esi
		pop		ebx
		ret
