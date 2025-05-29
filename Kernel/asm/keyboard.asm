global sys_getKey
; polling
section .text
sys_getKey:
    push rbp
    mov rbp, rsp
    mov rax,0

    ; leo la tecla
    in al, 0x60

    mov rsp, rbp
    pop rbp
    ret