[FORMAT "WCOFF"]
[INSTRSET "i486p"]
[BITS 32]
[FILE "end.nas"]

		GLOBAL	_api_end

[SECTION .text]

_api_end:			; void api_end(void);
		mov		edx,4
		int		0x40
