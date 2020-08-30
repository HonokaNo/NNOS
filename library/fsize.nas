[FORMAT "WCOFF"]
[INSTRSET "i486p"]
[BITS 32]
[FILE "fsize.nas"]

		GLOBAL	_api_fsize

[SECTION .text]

_api_fsize:			; int api_fsize(int fhandle, int mode);
		mov		edx,25
		mov		eax,[esp+4]
		mov		ecx,[esp+8]
		int		0x40
		ret
