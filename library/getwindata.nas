[FORMAT "WCOFF"]
[INSTRSET "i486p"]
[BITS 32]
[FILE "getwindata.nas"]

		GLOBAL	_api_getwindata

[SECTION .text]

_api_getwindata:	; void api_getwindata(int win, int mode, struct ret *ret);
		push	ebx
		push	esi
		mov		edx,31
		mov		ebx,[esp+12]
		mov		eax,[esp+16]
		mov		esi,[esp+20]
		int		0x40
		pop		esi
		pop		ebx
		ret
