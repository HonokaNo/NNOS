[FORMAT "WCOFF"]
[INSTRSET "i486p"]
[BITS 32]
[FILE "fclose.nas"]

		GLOBAL	_api_fclose

[SECTION .text]

_api_fclose:			; int api_fclose(int fhandle);
		mov		edx,23
		mov		eax,[esp+4]
		int		0x40
		ret
