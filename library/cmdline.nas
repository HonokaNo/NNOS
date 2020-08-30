[FORMAT "WCOFF"]
[INSTRSET "i486p"]
[BITS 32]
[FILE "cmdline.nas"]

		GLOBAL	_api_cmdline

[SECTION .text]

_api_cmdline		; int api_cmdline(char *buf, int maxsize);
		push	ebx
		mov		edx,27
		mov		ebx,[esp+8]
		mov		ecx,[esp+12]
		int		0x40
		pop		ebx
		ret
