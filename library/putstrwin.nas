[FORMAT "WCOFF"]
[INSTRSET "i486p"]
[BITS 32]
[FILE "putstrwin.nas"]

		GLOBAL	_api_putstrwin

[SECTION .text]

_api_putstrwin:		; void api_putstrwin(int win, int x, int y, struct color *c, char *str);
		push	edi
		push	esi
		push	ebp
		push	ebx
		mov		edx,6
		mov		ebx,[esp+20]
		mov		esi,[esp+24]
		mov		edi,[esp+28]
		mov		eax,[esp+32]
		mov		ebp,[esp+36]
		int		0x40
		pop		ebx
		pop		ebp
		pop		esi
		pop		edi
		ret
