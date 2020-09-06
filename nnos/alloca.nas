[FORMAT "WCOFF"]
[INSTRSET "i486p"]
[BITS 32]
[FILE "alloca.nas"]

		GLOBAL	__alloca

[SECTION .text]

__alloca:
		add		eax,-4
		sub		esp,eax
		jmp		dword [esp+eax]
