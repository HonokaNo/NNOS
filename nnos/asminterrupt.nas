[FORMAT "WCOFF"]
[INSTRSET "i486p"]
[BITS 32]
[FILE "asminterrupt.nas"]

		GLOBAL	_asm_inthandler20
		GLOBAL	_asm_inthandler21
		GLOBAL	_asm_inthandler26
		GLOBAL	_asm_inthandler28, _asm_inthandler2c
		GLOBAL	_asm_inthandler07, _asm_inthandler0c, _asm_inthandler0d
		EXTERN	_inthandler20
		EXTERN	_inthandler21
		EXTERN	_inthandler26
		EXTERN	_inthandler28, _inthandler2c
		EXTERN	_inthandler07, _inthandler0c, _inthandler0d
		EXTERN	_asm_end_app

[SECTION .text]

_asm_inthandler20:
		push	es
		push	ds
		pushad
		mov		eax,esp
		push	eax
		mov		ax,ss
		mov		ds,ax
		mov		es,ax
		call	_inthandler20
		pop		eax
		popad
		pop		ds
		pop		es
		iretd

_asm_inthandler21:
		push	es
		push	ds
		pushad
		mov		eax,esp
		push	eax
		mov		ax,ss
		mov		ds,ax
		mov		es,ax
		call	_inthandler21
		pop		eax
		popad
		pop		ds
		pop		es
		iretd

_asm_inthandler26:
		push	es
		push	ds
		pushad
		mov		eax,esp
		push	eax
		mov		ax,ss
		mov		ds,ax
		mov		es,ax
		call	_inthandler26
		pop		eax
		popad
		pop		ds
		pop		es
		iretd

_asm_inthandler28:
		push	es
		push	ds
		pushad
		mov		eax,esp
		push	eax
		mov		ax,ss
		mov		ds,ax
		mov		es,ax
		call	_inthandler28
		pop		eax
		popad
		pop		ds
		pop		es
		iretd

_asm_inthandler2c:
		push	es
		push	ds
		pushad
		mov		eax,esp
		push	eax
		mov		ax,ss
		mov		ds,ax
		mov		es,ax
		call	_inthandler2c
		pop		eax
		popad
		pop		ds
		pop		es
		iretd

_asm_inthandler07:
		sti
		push	es
		push	ds
		pushad
		mov		eax,esp
		push	eax
		mov		ax,ss
		mov		ds,ax
		mov		es,ax
		call	_inthandler07
		cmp		eax,0
		jne		_asm_end_app
		pop		eax
		popad
		pop		ds
		pop		es
		iretd

_asm_inthandler0c:
		sti
		push	es
		push	ds
		pushad
		mov		eax,esp
		push	eax
		mov		ax,ss
		mov		ds,ax
		mov		es,ax
		call	_inthandler0c
		cmp		eax,0
		jne		_asm_end_app
		pop		eax
		popad
		pop		ds
		pop		es
		add		esp,4
		iretd

_asm_inthandler0d:
		sti
		push	es
		push	ds
		pushad
		mov		eax,esp
		push	eax
		mov		ax,ss
		mov		ds,ax
		mov		es,ax
		call	_inthandler0d
		cmp		eax,0
		jne		_asm_end_app
		pop		eax
		popad
		pop		ds
		pop		es
		add		esp,4
		iretd
