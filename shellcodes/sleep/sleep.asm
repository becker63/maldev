;https://www.vividmachines.com/shellcode/shellcode.html
; nasm -f elf sleep.asm; ld -m elf_i386 -o sleep sleep.o; objdump -d sleep

; "When this code is inserted it will cause the parent thread to suspend for five seconds (note: it will then probably crash because the stack is smashed at this point :-D)."

;char code[] = "\x31\xc0\xbb\xea\x1b\xe6\x77\x66\xb8\x88\x13\x50\xff\xd3";

[SECTION .text]

global _start

_start:
        xor eax,eax
        mov ebx, 0x77e61bea ;address of Sleep
        mov ax, 5000        ;pause for 5000ms
        push eax
        call ebx        ;Sleep(ms);