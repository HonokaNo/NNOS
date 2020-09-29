CYLS	equ		25

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
		mov		bx,18*2*CYLS-1
		call	readfast

		mov		byte [0x0ff0],CYLS
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

readfast:
		mov		ax,es
		shl		ax,3
		and		ah,0x7f
		mov		al,128
		sub		al,ah

		mov		ah,bl
		cmp		bh,0
		je		.skip1
		mov		ah,18
.skip1:
		cmp		al,ah
		jbe		.skip2
		mov		al,ah
.skip2:
		mov		ah,19
		sub		ah,cl
		cmp		al,ah
		jbe		.skip3
		mov		al,ah
.skip3:
		push	bx
		mov		si,0
retry:
		mov		ah,0x02
		mov		bx,0
		mov		dl,0x00
		push	es
		push	dx
		push	cx
		push	ax
		int		0x13
		jnc		next
		inc		si
		cmp		si,5
		jae		error
		mov		ah,0x00
		mov		dl,0x00
		int		0x13
		pop		ax
		pop		cx
		pop		dx
		pop		es
		jmp		retry
next:
		pop		ax
		pop		cx
		pop		dx
		pop		bx
		shr		bx,5
		mov		ah,0
		add		bx,ax
		shl		bx,5
		mov		es,bx
		pop		bx
		sub		bx,ax
		jz		.ret
		add		cl,al
		cmp		cl,18
		jbe		readfast
		mov		cl,1
		inc		dh
		cmp		dh,2
		jb		readfast
		mov		dh,0
		inc		ch
		jmp		readfast
.ret
		ret

		resb	0x7dfe-$		; 0x7dfe‚Ü‚Å‚ð0x00‚Å–„‚ß‚é–½—ß

		db		0x55,0xaa
