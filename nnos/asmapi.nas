[FORMAT "WCOFF"]
[INSTRSET "i486p"]
[BITS 32]
[FILE "asmapi.nas"]

		GLOBAL	_asm_hrb_api
		GLOBAL	_asm_end_app
		EXTERN	_hrb_api

[SECTION .text]

_asm_hrb_api:
		sti
		push	ds
		push	es
		pushad
		pushad
		mov		ax,ss
		mov		ds,ax
		mov		es,ax
		call	_hrb_api
		cmp		eax,0
		jne		_asm_end_app
		add		esp,32
		popad
		pop		es
		pop		ds
		iretd
_asm_end_app:
		mov		esp,[eax]
		mov		dword [eax+4],0
		popad
		ret
