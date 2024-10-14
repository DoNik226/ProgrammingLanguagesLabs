%include "lib.inc"

%define BYTE 8

global find_word

find_word:
	push r12
	push r13
	mov r12, rdi 
	mov r13, rsi
	.loop:
		test r13, r13 
		jz .not_found

		mov rdi, r12
		lea rsi, [r13 + BYTE]
		call string_equals
		test rax, rax
		jnz .found
		mov r13, [r13]
		jmp .loop
	.found:
		mov rax, r13
		pop r13
		pop r12
		ret
	.not_found:
		xor rax, rax
		pop r13
		pop r12
		ret	
