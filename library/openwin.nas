[FORMAT "WCOFF"]
[INSTRSET "i486p"]
[BITS 32]
[FILE "openwin.nas"]

		GLOBAL	_api_openwin

[SECTION .text]

_api_openwin:		; int api_openwin(char *buf, int xsize, int ysize, char *title, char resize);
		push	edi
		push	esi
		push	ebx
		mov		edx,5
		mov		ebx,[esp+16]
		mov		esi,[esp+20]
		mov		edi,[esp+24]
		mov		ecx,[esp+28]
		mov		eax,[esp+32]
		int		0x40
		pop		ebx
		pop		esi
		pop		edi
		ret
