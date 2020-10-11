[FORMAT "WCOFF"]
[INSTRSET "i486p"]
[BITS 32]
[FILE "putchar.nas"]

		GLOBAL	_api_putchar

[SECTION .text]

_api_putchar:		; void api_putchar(int c);
		mov		edx,1
		mov		al,[esp+4]
		int		0x40
		ret
