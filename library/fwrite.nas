[FORMAT "WCOFF"]
[INSTRSET "i486p"]
[BITS 32]
[FILE "fwrite.nas"]

		GLOBAL	_api_fwrite

[SECTION .text]

_api_fwrite:			; int api_fwrite(char *buf, int size, int fhandle);
		push	ebx
		mov		edx,29
		mov		ebx,[esp+8]
		mov		ecx,[esp+12]
		mov		eax,[esp+16]
		int		0x40
		pop		ebx
		ret
