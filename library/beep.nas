[FORMAT "WCOFF"]
[INSTRSET "i486p"]
[BITS 32]
[FILE "beep.nas"]

		GLOBAL	_api_beep

[SECTION .text]

_api_beep:			; void api_beep(int tone);
		mov		edx,21
		mov		eax,[esp+4]
		int		0x40
		ret
