[FORMAT "WCOFF"]
[INSTRSET "i486p"]
[BITS 32]
[FILE "getwindata.nas"]

		GLOBAL	_api_getwindata

[SECTION .text]

_api_getwindata:	; void api_getwindata(int win, int mode, struct ret *ret);
		push	ebx
		mov		edx,31
		mov		ebx,[esp+8]
		mov		eax,[esp+12]
		mov		esi,[esp+16]
		int		0x40
		pop		ebx
		ret
