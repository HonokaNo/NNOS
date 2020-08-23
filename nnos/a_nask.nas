[FORMAT "WCOFF"]
[INSTRSET "i486p"]
[BITS 32]
[FILE "a_nask.nas"]

		GLOBAL	_api_putchar
		GLOBAL	_api_putstr0
		GLOBAL	_api_end
		GLOBAL	_api_openwin
		GLOBAL	_api_putstrwin
		GLOBAL	_api_boxfilwin

[SECTION .text]

_api_putchar:	; void api_putchar(int c);
		mov		edx,1
		mov		al,[esp+4]
		int		0x40
		ret


_api_putstr0:	; void api_putstr0(char *s);
		push	ebx
		mov		edx,2
		mov		ebx,[esp+8]
		int		0x40
		pop		ebx
		ret

_api_end:		; void api_end(void);
		mov		edx,4
		int		0x40

_api_openwin:	; int api_openwin(char *buf, int xsize, int ysize, char *title);
		push	edi
		push	esi
		push	ebx
		mov		edx,5
		mov		ebx,[esp+16]
		mov		esi,[esp+20]
		mov		edi,[esp+24]
		mov		ecx,[esp+28]
		int		0x40
		pop		ebx
		pop		esi
		pop		edi
		ret

_api_putstrwin:	; void api_putstrwin(int win, int x, int y, struct color c, char *str);
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

_api_boxfilwin:	; void api_boxfilwin(int win, int x0, int y0, int x1, int y1, struct color c);
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
