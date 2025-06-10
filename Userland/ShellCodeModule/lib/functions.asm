section .text

global hlt
global divZero
global invOpcode

hlt:
    hlt
    ret

invOpcode:
    push rbp
    mov rbp, rsp

    ud2

    mov rsp, rbp
    pop rbp
    ret

divZero:
    push rbp
    mov rbp, rsp

    mov rdx, 1
    mov rax, 0
    div rax

    mov rsp, rbp
    pop rbp
    ret