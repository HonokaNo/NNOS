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
		GLOBAL	_api_initmalloc
		GLOBAL	_api_malloc
		GLOBAL	_api_free
		GLOBAL	_api_point
		GLOBAL	_api_refreshwin
		GLOBAL	_api_linewin
		GLOBAL	_api_closewin
		GLOBAL	_api_getkey
		global	_debugmb

[SECTION .text]

_api_putchar:		; void api_putchar(int c);
		mov		edx,1
		mov		al,[esp+4]
		int		0x40
		ret


_api_putstr0:		; void api_putstr0(char *s);
		push	ebx
		mov		edx,2
		mov		ebx,[esp+8]
		int		0x40
		pop		ebx
		ret

_api_end:			; void api_end(void);
		mov		edx,4
		int		0x40

_api_openwin:		; int api_openwin(char *buf, int xsize, int ysize, char *title);
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

_api_initmalloc:	; void api_initmalloc(void);
		push	ebx
		mov		edx,8
		mov		ebx,[cs:0x0020]
		mov		eax,ebx
		add		eax,12016	; MEMMANÇÃÉTÉCÉY
		mov		ecx,[cs:0x0000]
		sub		ecx,eax
		int		0x40
		pop		ebx
		ret

_api_malloc:		; char *api_malloc(int size);
		push	ebx
		mov		edx,9
		mov		ebx,[cs:0x0020]
		mov		ecx,[esp+8]
		int		0x40
		pop		ebx
		ret

_api_free:			; void api_free(char *addr, int size);
		push	ebx
		mov		edx,10
		mov		ebx,[cs:0x0020]
		mov		eax,[esp+ 8]
		mov		ecx,[esp+12]
		int		0x40
		pop		ebx
		ret

_api_point:			; void api_point(int win, int x, int y, struct color *c);
		push	edi
		push	esi
		push	ebx
		mov		edx,11
		mov		ebx,[esp+16]
		mov		esi,[esp+20]
		mov		edi,[esp+24]
		mov		eax,[esp+28]
		int		0x40
		pop		ebx
		pop		esi
		pop		edi
		ret

_api_refreshwin:	; void api_refreshwin(int win, int x0, int y0, int x1, int y1);
		push	edi
		push	esi
		push	ebx
		mov		edx,12
		mov		ebx,[esp+16]
		mov		eax,[esp+20]
		mov		ecx,[esp+24]
		mov		esi,[esp+28]
		mov		edi,[esp+32]
		int		0x40
		pop		ebx
		pop		esi
		pop		edi
		ret

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

_api_closewin:		; void api_closewin(int win);
		push	ebx
		mov		edx,14
		mov		ebx,[esp+8]
		int		0x40
		pop		ebx
		ret

_api_getkey:		; int api_getkey(int mode);
		mov		edx,15
		mov		eax,[esp+4]
		int		0x40
		ret

_debugmb:
		xchg	bx,bx
		ret
