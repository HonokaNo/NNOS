[FORMAT "WCOFF"]
[INSTRSET "i486p"]
[BITS 32]
[FILE "putstr0.nas"]

		GLOBAL	_api_putstr0

[SECTION .text]

_api_putstr0:		; void api_putstr0(char *s);
		push	ebx
		mov		edx,2
		mov		ebx,[esp+8]
		int		0x40
		pop		ebx
		ret
