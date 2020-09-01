[FORMAT "WCOFF"]
[INSTRSET "i486p"]
[BITS 32]
[FILE "getclock.nas"]

		GLOBAL	_api_getclock

[SECTION .text]

_api_getclock:			; void *api_getclock(struct time *time);
		mov		edx,30
		mov		ebx,[esp+4]
		int		0x40
