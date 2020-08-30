[FORMAT "WCOFF"]
[INSTRSET "i486p"]
[BITS 32]
[FILE "inittimer.nas"]

		GLOBAL	_api_inittimer

[SECTION .text]

_api_inittimer:		; void api_inittimer(int timer, int data);
		push	ebx
		mov		edx,17
		mov		ebx,[esp+ 8]
		mov		eax,[esp+12]
		int		0x40
		pop		ebx
		ret
