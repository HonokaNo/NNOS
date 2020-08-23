[FORMAT "WCOFF"]
[INSTRSET "i486p"]
[BITS 32]
[FILE "a_nask.nas"]

		GLOBAL	_api_putchar

[SECTION .text]

_api_putchar:
		mov		edx,1
		mov		al,[esp+4]
		int		0x40
		ret
