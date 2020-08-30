[FORMAT "WCOFF"]
[INSTRSET "i486p"]
[BITS 32]
[FILE "malloc.nas"]

		GLOBAL	_api_malloc

[SECTION .text]

_api_malloc:		; char *api_malloc(int size);
		push	ebx
		mov		edx,9
		mov		ebx,[cs:0x0020]
		mov		ecx,[esp+8]
		int		0x40
		pop		ebx
		ret
