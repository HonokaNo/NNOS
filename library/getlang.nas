[FORMAT "WCOFF"]
[INSTRSET "i486p"]
[BITS 32]
[FILE "getlang.nas"]

		GLOBAL	_api_getlang

[SECTION .text]

_api_getlang:		; int api_getlang(void);
		mov		edx,28
		int		0x40
		ret
