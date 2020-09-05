[FORMAT "WCOFF"]
[INSTRSET "i486p"]
[BITS 32]
[FILE "alloca.nas"]

		GLOBAL	__alloca

[SECTION .text]

__alloca:
		sub		esp,eax
		add		esp,4
		jmp		dword [esp+eax-4]
