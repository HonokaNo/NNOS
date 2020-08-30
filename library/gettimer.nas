[FORMAT "WCOFF"]
[INSTRSET "i486p"]
[BITS 32]
[FILE "gettimer.nas"]

		GLOBAL	_api_gettimer

[SECTION .text]

_api_gettimer:		; int api_gettimer(int mode);
		mov		edx,20
		mov		eax,[esp+4]
		int		0x40
		ret
