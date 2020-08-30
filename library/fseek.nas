[FORMAT "WCOFF"]
[INSTRSET "i486p"]
[BITS 32]
[FILE "fseek.nas"]

		GLOBAL	_api_fseek

[SECTION .text]

_api_fseek:			; int api_fseek(int fhandle, int offset, int mode);
		push	ebx
		mov		edx,24
		mov		eax,[esp+8]
		mov		ebx,[esp+12]
		mov		ecx,[esp+16]
		int		0x40
		pop		ebx
		ret
