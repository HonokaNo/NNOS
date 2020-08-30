[FORMAT "WCOFF"]
[INSTRSET "i486p"]
[BITS 32]
[FILE "boxfilwin.nas"]

		GLOBAL	_api_boxfilwin

[SECTION .text]

_api_boxfilwin:		; void api_boxfilwin(int win, int x0, int y0, int x1, int y1, struct color *c);
		push	edi
		push	esi
		push	ebp
		push	ebx
		mov		edx,7
		mov		ebx,[esp+20]
		mov		eax,[esp+24]
		mov		ecx,[esp+28]
		mov		esi,[esp+32]
		mov		edi,[esp+36]
		mov		ebp,[esp+40]
		int		0x40
		pop		ebx
		pop		ebp
		pop		esi
		pop		edi
		ret
