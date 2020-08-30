CYLS	equ		20

		org		0x7c00

		jmp		entry
		nop
		db		"NONAMEOS"
		dw		512
		db		1
		dw		1
		db		2
		dw		224
		dw		2880
		db		0xf0
		dw		9
		dw		18
		dw		2
		dd		0
		dd		2880
		db		0,0,0x29
		dd		0xffffffff
		db		"NONAME_OS  "
		db		"fat12   "
		resb	18

entry:
		mov		ax,0
		mov		ss,ax
		mov		sp,0x7c00
		mov		ds,ax

		mov		ax,0x0820
		mov		es,ax
		mov		cx,0x0002
		mov		dh,0
readloop:
		mov		si,0
retry:
		mov		ax,0x0201
		mov		bx,0
		mov		dl,0x00
		int		0x13
		jnc		next
		inc		si
		cmp		si,3
		jae		error
		mov		ah,0x00
		mov		dl,0x00
		int		0x13
		jmp		retry
next:
		mov		ax,es
		add		ax,0x0020
		mov		es,ax
		inc		cl
		cmp		cl,18
		jbe		readloop
		mov		cl,1
		inc		dh
		cmp		dh,2
		jb		readloop
		mov		dh,0
		inc		ch
		cmp		ch,CYLS
		jb		readloop

		mov		[0x0ff0],ch
		jmp		0xc200

error:
		mov		ax,0
		mov		es,ax
		mov		si,msg

putloop:
		lodsb
		cmp		al,0
		je		fin
		mov		ah,0x0e
		mov		bx,15
		int		0x10
		jmp		putloop

fin:
		hlt
		jmp		fin

msg:
		db		0x0a,0x0a
		db		"load error"
		db		0x0a
		db		0

		resb	0x7dfe-$		; 0x7dfe‚Ü‚Å‚ð0x00‚Å–„‚ß‚é–½—ß

		db		0x55,0xaa
