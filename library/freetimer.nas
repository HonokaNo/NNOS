[FORMAT "WCOFF"]
[INSTRSET "i486p"]
[BITS 32]
[FILE "freetimer.nas"]

		GLOBAL	_api_freetimer

[SECTION .text]

_api_freetimer:		; void api_freetimer(int timer);
		push	ebx
		mov		edx,19
		mov		ebx,[esp+8]
		int		0x40
		pop		ebx
		ret
