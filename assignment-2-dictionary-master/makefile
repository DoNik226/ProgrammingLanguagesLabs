ASM = nasm
ASMFLAGS = -felf64
LD = ld


%.o: %.asm 
        $(ASM) $(ASMFLAGS) -o $@ $<

dict.o: dict.asm lib.inc
main.o: main.asm lib.inc colon.inc words.inc
words.inc: colon.inc

main: main.o lib.o dict.o
        $(LD) -o main $^

.PHONY: all test clean

clean:
        rm *.o
all:
        ./main

test:
        python3 test.py
