[FORMAT "WCOFF"]
[INSTRSET "i486p"]
[BITS 32]
[FILE "alloctimer.nas"]

		GLOBAL	_api_alloctimer

[SECTION .text]

_api_alloctimer:	; int api_alloctimer(void);
		mov		edx,16
		int		0x40
		ret
