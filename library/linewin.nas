[FORMAT "WCOFF"]
[INSTRSET "i486p"]
[BITS 32]
[FILE "linewin.nas"]

		GLOBAL	_api_linewin

[SECTION .text]

_api_linewin:		; void api_linewin(int win, int x0, int y0, int x1, int y1, struct color c);
		push	edi
		push	esi
		push	ebp
		push	ebx
		mov		edx,13
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
