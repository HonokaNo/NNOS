[FORMAT "WCOFF"]
[INSTRSET "i486p"]
[BITS 32]
[FILE "closewin.nas"]

		GLOBAL	_api_closewin

[SECTION .text]

_api_closewin:		; void api_closewin(int win);
		push	ebx
		mov		edx,14
		mov		ebx,[esp+8]
		int		0x40
		pop		ebx
		ret
