[FORMAT "WCOFF"]
[INSTRSET "i486p"]
[BITS 32]
[FILE "getkey.nas"]

		GLOBAL	_api_getkey

[SECTION .text]

_api_getkey:		; int api_getkey(int mode);
		mov		edx,15
		mov		eax,[esp+4]
		int		0x40
		ret
