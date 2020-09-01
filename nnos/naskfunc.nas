[FORMAT "WCOFF"]
[INSTRSET "i486p"]
[BITS 32]

[FILE "naskfunc.nas"]

		GLOBAL	_io_hlt, _io_cli, _io_sti, _io_stihlt
		GLOBAL	_io_in8, _io_out8
		GLOBAL	_io_load_eflags, _io_store_eflags
		GLOBAL	_load_gdtr, _load_idtr
		GLOBAL	_load_cr0, _store_cr0
		GLOBAL	_load_tr
		GLOBAL	_memtest_sub
		GLOBAL	_wait10
		GLOBAL	_farjmp
		GLOBAL	_farcall
		GLOBAL	_start_app
		GLOBAL	_asm_hrb_api
		GLOBAL	_asm_end_app

		EXTERN	_hrb_api

[SECTION .text]

_io_hlt:	; void io_hlt(void);
		hlt
		ret

_io_cli:	; void io_cli(void);
		cli
		ret

_io_sti:	; void io_sti(void);
		sti
		ret

_io_stihlt:	; void io_stihlt(void);
		sti
		hlt
		ret

_io_in8:	; int io_in8(int port);
		mov		edx,[esp+4]
		mov		eax,0
		in		al,dx
		ret

_io_out8:	; void io_out8(int port, int data);
		mov		edx,[esp+4]
		mov		al,[esp+8]
		out		dx,al
		ret

_io_load_eflags:	; int io_load_eflags(void);
		pushfd
		pop		eax
		ret

_io_store_eflags:	; void io_store_eflags(int eflags);
		mov		eax,[esp+4]
		push	eax
		popfd
		ret

_load_gdtr:		; void load_gdtr(int limit, int addr);
		mov		ax,[esp+4]
		mov		[esp+6],ax
		lgdt	[esp+6]
		ret

_load_idtr:		; void load_idtr(int limit, int addr);
		mov		ax,[esp+4]
		mov		[esp+6],ax
		lidt	[esp+6]
		ret

_load_cr0:		; int load_cr0(void);
		mov		eax,cr0
		ret

_store_cr0:		; void store_cr0(int cr0);
		mov		eax,[esp+4]
		mov		cr0,eax
		ret

_load_tr:		; void load_tr(int tr);
		ltr		[esp+4]
		ret

_memtest_sub:	; unsigned int memtest_sub(unsigned int start, unsigned int end)
		push	edi
		push	esi
		push	ebx
		mov		esi,0xaa55aa55
		mov		edi,0x55aa55aa
		mov		eax,[esp+12+4]
mts_loop:
		mov		ebx,eax
		add		ebx,0xffc
		mov		edx,[ebx]
		mov		[ebx],esi
		xor		dword [ebx],0xffffffff
		cmp		edi,[ebx]
		jne		mts_fin
		xor		dword [ebx],0xffffffff
		cmp		esi,[ebx]
		jne		mts_fin
		mov		[ebx],edx
		add		eax,0x10000				; 64KBずつチェック
		cmp		eax,[esp+12+8]
		jbe		mts_loop
		pop		ebx
		pop		esi
		pop		edi
		ret
mts_fin:
		mov		[ebx],edx
		pop		ebx
		pop		esi
		pop		edi
		ret

_wait10:
		push	cx
		mov		cx,10
		rep		nop
		pop		cx
		ret

_farjmp:		; void farjmp(int eip, int cs);
		jmp		far [esp+4]
		ret

_farcall:		; void farcall(int eip, int cs);
		call	far [esp+4]
		ret

_start_app:		; void start_app(int eip, int cs, int esp, int ds, int *tss_esp0);
		pushad
		mov		eax,[esp+36]
		mov		ecx,[esp+40]
		mov		edx,[esp+44]
		mov		ebx,[esp+48]
		mov		ebp,[esp+52]
		mov		[ebp+0],esp
		mov		[ebp+4],ss
		mov		es,bx
		mov		ds,bx
		mov		fs,bx
		mov		gs,bx
		or		ecx,3
		or		ebx,3
		push	ebx
		push	edx
		push	ecx
		push	eax
		retf

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
