
GLOBAL in
GLOBAL out


section .text

; leemos del puerto E/S
; recibimos la direccion de memoria por rdi
in:
    push rbp
    mov rbp, rsp

    mov rdx, rdi 
    xor rax, rax
    in al, dx

    leave
    pop rbp
    ret

; primer parámetro (rdi): valor
; segundo parámetro (rsi): direccion de memoria
out:
    push rbp
    mov rbp, rsp    

    mov dx, di      ;el numero de puerto a escribir (16 bits)
    mov al, sil     ;el valor a escribir (8 bits)
    out dx, al      ;ejecutamos la salida del puerto

    leave
    pop rbp
    ret

    
 