[INSTRSET "i486p"]

; 画面モード
; 基本:
;   0x10f  320x 200 8:8:8 color
;   0x112  640x 480 8:8:8 color
;   0x115  800x 600 8:8:8 color
;   0x118 1024x 768 8:8:8 color
;   0x11b 1280x1024 8:8:8 color
VIDEOMODE	equ		0x117

BOTPAK	equ		0x00280000
DSKCAC	equ		0x00100000
DSKCAC0	equ		0x00008000

CYLS	equ		0x0ff0
LEDS	equ		0x0ff1
VMODE	equ		0x0ff2
SCRNX	equ		0x0ff4
SCRNY	equ		0x0ff6
VRAM	equ		0x0ff8
SCLINE	equ		0x0ffc
ACPI	equ		0x0ffe

		org		0xc200

		cli

; A20line enable
		call	waitkbdout
		mov		al,0xd1
		out		0x64,al
		call	waitkbdout
		mov		al,0xdf
		out		0x60,al
		call	waitkbdout

		sti

; VBE version check
		mov		ax,0x9000
		mov		es,ax
		mov		di,0
		mov		ax,0x4f00
		int		0x10
		cmp		ax,0x004f
		jne		nsupportvbe

		mov		ax,[es:di+4]
		cmp		ax,0x0200
		jb		nsupportvbe

; get VBE mode info
		mov		cx,VIDEOMODE
		mov		ax,0x4f01
		int		0x10
		cmp		ax,0x004f
		jne		nsupportvbe
		mov		ax,[es:di]
		and		ax,0x0080
		jz		nsupportvbe

		mov		bx,VIDEOMODE + 0x4000
		mov		ax,0x4f02
		int		0x10
		mov		ax,[es:di+0x10]
		mov		[SCLINE],ax
		mov		al,[es:di+0x19]
		mov		[VMODE],al
		mov		ax,[es:di+0x12]
		mov		[SCRNX],ax
		mov		ax,[es:di+0x14]
		mov		[SCRNY],ax
		mov		eax,[es:di+0x28]
		mov		[VRAM],eax
		jmp		keystatus

; not support VBE 2.0
nsupportvbe:
		mov		ax,0x0013
		int		0x10
		mov		byte [VMODE],8
		mov		word [SCRNX],320
		mov		word [SCRNY],200
		mov		dword [VRAM],0x000a0000
		mov		word [SCLINE],320

; get key status
keystatus:
		mov		ah,0x02
		int		0x16
		mov		[LEDS],al

		mov		al,0xff
		out		0x21,al
		nop
		out		0xa1,al

		cli

; get ACPI mem
		mov		dword [ACPI],0x00000000
		mov		ebx,0
		mov		ax,memdata >> 4
		mov		es,ax
reload:
		mov		ecx,20
		mov		eax,0xe820
		mov		di,0
		mov		edx,"PAMS"
; get System memory map
		int		0x15
; error! end
		jc		end_acpi_mem
		cmp		eax,"PAMS"
		jne		end_acpi_mem
		mov		eax,[memdata.type]
		cmp		eax,3
		je		set_acpi_address
		cmp		ebx,0
		jne		reload
		jmp		end_acpi_mem
set_acpi_address:
		mov		eax,[memdata.base]
		mov		[ACPI],eax
end_acpi_mem:

; set GDT
		lgdt	[GDTR0]
		mov		eax,cr0
		and		eax,0x7fffffff
		or		eax,0x00000001
		mov		cr0,eax
		jmp		pipelineflush
pipelineflush:
		mov		ax,1*8
		mov		ds,ax
		mov		es,ax
		mov		fs,ax
		mov		gs,ax
		mov		ss,ax

; init FPU
		fninit

		mov		esi,bootpack
		mov		edi,BOTPAK
		mov		ecx,512*1024/4
		call	memcpy

		mov		esi,0x7c00
		mov		edi,DSKCAC
		mov		ecx,512/4
		call	memcpy

		mov		esi,DSKCAC0+512
		mov		edi,DSKCAC+512
		mov		ecx,0
		mov		cl,byte [CYLS]
		imul	ecx,512*18*2/4
		sub		ecx,512/4
		call	memcpy

		mov		ebx,BOTPAK
		mov		ecx,[ebx+16]
		add		ecx,3
		shr		ecx,2
		jz		skip
		mov		esi,[ebx+20]
		add		esi,ebx
		mov		edi,[ebx+12]
		call	memcpy
skip:
		mov		esp,[ebx+12]

		jmp		dword 2*8:0x0000001b

waitkbdout:
		in		al,0x64
		and		al,0x02
		in		al,0x60
		jnz		waitkbdout
		ret

memcpy:
		mov		eax,[esi]
		add		esi,4
		mov		[edi],eax
		add		edi,4
		dec		ecx
		jnz		memcpy
		ret

		alignb	16

memdata:
.base:
		resb	4		; base0
.base1:
		resb	4		; base1
.size:
		resb	4		; length0
.size1:
		resb	4		; length1
.type
		resb	4		; type

GDT0:
		resb	8
		dw		0xffff,0x0000,0x9200,0x00cf
		dw		0xffff,0x0000,0x9a28,0x0047

		dw		0
GDTR0:
		dw		8*3-1
		dd		GDT0

		alignb	16

bootpack:
