%include "lib.inc"
%include "colon.inc"
%include "words.inc"
%include "dict.inc"

%define BUFFER_SIZE 256

section .rodata
not_found_message: db "Element not found", 0
invalid_input_message: db "Invalid input", 0

section .bss
buffer: times BUFFER_SIZE db 0

section .text
global _start:

_start:
	mov rdi, buffer
	mov rsi, BUFFER_SIZE
	call read_word
	test rax, rax
	jz .invalid_input

	
	push rdx	

	mov rdi, buffer
	mov rsi, NEXT
	call find_word

	pop rdx	

	test rax, rax
	jz .not_found
		
	lea rdi, [rax + 8 + rdx + 1] 
	call print_string
	jmp .end

	.not_found:
		mov rdi, not_found_message
		call print_error
		jmp .end
	.invalid_input:
		mov rdi, invalid_input_message
		call print_error
	.end:
		call print_newline
		call exit
