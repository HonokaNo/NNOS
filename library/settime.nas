[FORMAT "WCOFF"]
[INSTRSET "i486p"]
[BITS 32]
[FILE "settime.nas"]

		GLOBAL	_api_settime

[SECTION .text]

_api_settime:		; void api_settime(int timer, int time);
		push	ebx
		mov		edx,18
		mov		ebx,[esp+ 8]
		mov		eax,[esp+12]
		int		0x40
		pop		ebx
		ret
