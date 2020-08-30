[FORMAT "WCOFF"]
[INSTRSET "i486p"]
[BITS 32]
[FILE "initmalloc.nas"]

		GLOBAL	_api_initmalloc

[SECTION .text]

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
