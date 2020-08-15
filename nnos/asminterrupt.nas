[FORMAT "WCOFF"]
[INSTRSET "i486p"]
[BITS 32]
[FILE "asminterrupt.nas"]

		GLOBAL	_asm_inthandler21, _asm_inthandler27
		GLOBAL	_asm_inthandler28, _asm_inthandler2c
		EXTERN	_inthandler21, _inthandler27
		EXTERN	_inthandler28, _inthandler2c

[SECTION .text]

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

_asm_inthandler27:
		push	es
		push	ds
		pushad
		mov		eax,esp
		push	eax
		mov		ax,ss
		mov		ds,ax
		mov		es,ax
		call	_inthandler27
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
