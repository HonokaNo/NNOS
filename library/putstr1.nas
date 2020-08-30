[FORMAT "WCOFF"]
[INSTRSET "i486p"]
[BITS 32]
[FILE "putstr1.nas"]

		GLOBAL	_api_putstr1

[SECTION .text]

_api_putstr1:		; void api_putstr1(char *s, int len);
		mov		edx,2
		mov		ebx,[esp+4]
		mov		ecx,[esp+8]
		int		0x40
		ret
