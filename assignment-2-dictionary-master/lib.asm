%define EXIT 60
%define READ 0
%define WRITE 1
%define STDOUT 1
%define STDERR 2
%define STDIN 0
%define SPACE     0x20
%define TAB       0x9
%define NEW_LINE  0xA


section .text

global exit
global string_length
global print_string
global print_newline
global print_char
global print_int
global print_uint
global string_equals
global read_char
global read_word
global parse_uint
global parse_int
global string_copy
global print_error


 
 
; Принимает код возврата и завершает текущий процесс
exit: 
    mov  rax, EXIT             ; Это функция exit
    xor  rdi, rdi
    syscall

; Принимает указатель на нуль-терминированную строку, возвращает её длину
string_length:
    mov  rax, rdi
  .counter:
    cmp  byte [rdi], 0
    je   .end
    inc  rdi
    jmp  .counter
  .end:
    sub  rdi, rax
    mov  rax, rdi
    ret

; Принимает указатель на нуль-терминированную строку, выводит её в stdout
print_string:
    push rdi
    call string_length
	pop rdi
	mov  rsi, rdi
    mov  rdx, rax
    mov  rax, WRITE
    mov  rdi, STDOUT
    syscall
    ret

; Принимает код символа и выводит его в stdout
print_char:
	push rdi
    mov rsi, rsp
	mov rdi, STDOUT
	mov rax, WRITE
	mov rdx, 1
	syscall
	pop rdi
    ret

; Переводит строку (выводит символ с кодом 0xA)
print_newline:
    mov rdi, NEW_LINE
	jmp print_char

; Выводит беззнаковое 8-байтовое число в десятичном формате 
; Совет: выделите место в стеке и храните там результаты деления
; Не забудьте перевести цифры в их ASCII коды.
print_uint:
	push rbp
	mov rbp, rsp
    mov r10, 10
    mov rax, rdi
	sub rsp, 24
	.char:
	  xor rdx, rdx
	  div r10
	  add rdx, '0'
	  dec rbp
	  mov byte [rbp], dl
	  test rax, rax
	  jne .char
	mov rdi, rbp
	call print_string
	add rsp, 24
	pop rbp
    ret

; Выводит знаковое 8-байтовое число в десятичном формате 
print_int:
    test rdi, rdi
	jge .pos
	neg rdi
	push rdi
	mov rdi, '-'
	call print_char
	pop rdi
	.pos:
	  jmp print_uint

; Принимает два указателя на нуль-терминированные строки, возвращает 1 если они равны, 0 иначе
string_equals:
    xor rax, rax
	.loop:
	  mov al, byte[rdi] ;символ из первой строки в rax
	  cmp al, byte[rsi] ;сравнивает сивол второй строки с содержимым al
	  jne .not
	  inc rdi
	  inc rsi
	  test al, al
	  je .yes
	  jmp .loop
	.yes:
	  mov rax, 1
	  ret
	.not:
	  mov rax, 0
	  ret

; Читает один символ из stdin и возвращает его. Возвращает 0 если достигнут конец потока
read_char:
	sub rsp, 8
	mov rsi, rsp
	xor rdi, rdi
    xor rax, rax
    mov rdx, 1
	syscall
	test rax, rax
	jle .end
	pop rax
	ret
.end:
	pop rax
	xor rax, rax
	ret 
 

; Принимает: адрес начала буфера, размер буфера
; Читает в буфер слово из stdin, пропуская пробельные символы в начале, .
; Пробельные символы это пробел 0x20, табуляция 0x9 и перевод строки 0xA.
; Останавливается и возвращает 0 если слово слишком большое для буфера
; При успехе возвращает адрес буфера в rax, длину слова в rdx.
; При неудаче возвращает 0 в rax
; Эта функция должна дописывать к слову нуль-терминатор

read_word:
    push r13
	push r14
	push r15
	mov r13, rdi 
	mov r14, rdi 
	xor r15, rsi 
	.space:           ;проверка на пробелы в начале строки
	  call read_char
        cmp al, TAB
        je .space
        cmp al, SPACE
        je .space
        cmp al, NEW_LINE
        je .space
	.word:
	  cmp rax, SPACE
	  je .return
	  cmp rax, NEW_LINE
	  je .return
	  cmp rax, TAB
	  je .return
	  test rax, rax
	  je .return        
	  dec r15
        test r15, r15
        jz .wrong
        mov byte[r14], al
        inc r14
        call read_char
	  jmp .word
	.wrong:
	  xor rax, rax
	  xor rdx, rdx
	  pop r15
	  pop r14
	  pop r13
	  ret
	.return:
	  mov byte [r14], 0
	  sub r14, r13
	  mov rax, r13
	  mov rdx, r14
	  pop r15
	  pop r14
	  pop r13
	  ret




 

; Принимает указатель на строку, пытается
; прочитать из её начала беззнаковое число.
; Возвращает в rax: число, rdx : его длину в символах
; rdx = 0 если число прочитать не удалось
parse_uint:
    xor r10, r10
    mov r10, 10
	xor rax, rax ;число
	xor rdx, rdx ;длина числа
	xor r9, r9
	.digit:
	  movzx r9, byte [rdi + rdx]
	  test r9b, r9b    ;проверка на конец строки
	  je .return
	  cmp r9b, '0'  ;проверка на то, что символ не цифра
      jb .return
      cmp r9b, '9'  ;проверка на то, что символ не цифра
      ja .return
	  push rdx
	  mul r10        ;умножение rax на 10
	  pop rdx
	  sub r9b, '0'
	  add rax, r9
	  inc rdx
	  jmp .digit
	.return:
	  ret




; Принимает указатель на строку, пытается
; прочитать из её начала знаковое число.
; Если есть знак, пробелы между ним и числом не разрешены.
; Возвращает в rax: число, rdx : его длину в символах (включая знак, если он был) 
; rdx = 0 если число прочитать не удалось
parse_int:
    cmp byte [rdi], '-'
	je .minus
	cmp byte [rdi], '+'
	je .plus
	call parse_uint     ;работа с положительным числом без знака
    ret
	.minus:              ;работа с отрицательным числом со знаком
	  inc rdi
	  call parse_uint
	  add rdx, 1
	  neg rax
	  ret
	.plus:                  ;работа с положительным числом со знаком
	  inc rdi
	  call parse_uint
	  add rdx, 1
	  ret
	.wrong:           ;ошибка в написании числа
	  ret

; Принимает указатель на строку, указатель на буфер и длину буфера
; Копирует строку в буфер
; Возвращает длину строки если она умещается в буфер, иначе 0
string_copy:
    xor rax, rax
	.lenght:
	  cmp rax, rdx   ;проверка на допустимую длину строки
	  jge .wrong
	.read:            ;запись строки
	  mov r9b, byte [rdi] ;кладем элемент первой строки в регистр
	  mov byte [rsi], r9b ;кладем элемент первой строки из регистра во вторую строку
	  inc rdi
	  inc rsi
	  inc rax
	  test r9, r9     ;проверка на конец строки
	  je .end
	  jmp .lenght
	.end:         
	  ret
	.wrong:
	  xor rax, rax ;обнуление длины строки
	  ret


print_error:
	push rdi
	call string_length
	pop rsi
	mov rdx, rax
	mov rax, WRITE
	mov rdi, STDERR
	syscall
	ret

