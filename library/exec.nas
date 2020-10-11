[FORMAT "WCOFF"]
[INSTRSET "i486p"]
[BITS 32]
[FILE "exec.nas"]

		GLOBAL	_api_exec

[SECTION .text]

_api_exec:		; void api_exec(char *arg);
		push	ebx
		mov		edx,32
		mov		ebx,[esp+8]
		int		0x40
		pop		ebx
		ret
