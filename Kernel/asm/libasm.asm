GLOBAL cpuVendor
GLOBAL rtc
GLOBAL hlt
GLOBAL get_snapshot
GLOBAL registersArray

section .text
	
cpuVendor:
	push rbp
	mov rbp, rsp

	push rbx

	mov rax, 0
	cpuid


	mov [rdi], ebx
	mov [rdi + 4], edx
	mov [rdi + 8], ecx

	mov byte [rdi+13], 0

	mov rax, rdi

	pop rbx

	mov rsp, rbp
	pop rbp
	ret

rtc:
    push rbp
    mov rbp, rsp

    mov al, dil
    out 70h, al
    in al, 71h

    mov rsp, rbp
    pop rbp
    ret

hlt:
	sti
	hlt
	ret


get_snapshot:
    cli
    
    ; Cuando llegamos aquí, el stack tiene esta estructura:
    ; (desde el tope hacia abajo, después de pushState en _irq80Handler)
    ;
    ; [RSP + 0]   = return address de get_snapshot
    ; [RSP + 8]   = return address de sys_get_registers  
    ; [RSP + 16]  = return address de syscallDispatcher
    ; [RSP + 24]  = rax (guardado por pushState)
    ; [RSP + 32]  = rbx (guardado por pushState)
    ; [RSP + 40]  = rcx (guardado por pushState)
    ; [RSP + 48]  = rdx (guardado por pushState)
    ; [RSP + 56]  = rbp (guardado por pushState)
    ; [RSP + 64]  = rdi (guardado por pushState)
    ; [RSP + 72]  = rsi (guardado por pushState)
    ; [RSP + 80]  = r8  (guardado por pushState)
    ; [RSP + 88]  = r9  (guardado por pushState)
    ; [RSP + 96]  = r10 (guardado por pushState)
    ; [RSP + 104] = r11 (guardado por pushState)
    ; [RSP + 112] = r12 (guardado por pushState)
    ; [RSP + 120] = r13 (guardado por pushState)
    ; [RSP + 128] = r14 (guardado por pushState)
    ; [RSP + 136] = r15 (guardado por pushState)
    ; [RSP + 144] = rip (guardado por int 80h)
    ; [RSP + 152] = cs  (guardado por int 80h)
    ; [RSP + 160] = rflags (guardado por int 80h)
    ; [RSP + 168] = rsp original (guardado por int 80h)
    ; [RSP + 176] = ss  (guardado por int 80h)

    ; Capturar rflags original (guardado por hardware)
    mov rax, [rsp + 160]
    mov [registersArray + 0], rax

    ; Capturar registros originales (guardados por pushState)
    mov rax, [rsp + 24]         ; rax original
    mov [registersArray + 8], rax

    mov rax, [rsp + 32]         ; rbx original
    mov [registersArray + 16], rax

    mov rax, [rsp + 40]         ; rcx original
    mov [registersArray + 24], rax

    mov rax, [rsp + 48]         ; rdx original
    mov [registersArray + 32], rax

    mov rax, [rsp + 72]         ; rsi original
    mov [registersArray + 40], rax

    mov rax, [rsp + 64]         ; rdi original
    mov [registersArray + 48], rax

    mov rax, [rsp + 56]         ; rbp original
    mov [registersArray + 56], rax

    mov rax, [rsp + 168]        ; rsp original (guardado por hardware)
    mov [registersArray + 64], rax

    mov rax, [rsp + 80]         ; r8 original
    mov [registersArray + 72], rax

    mov rax, [rsp + 88]         ; r9 original
    mov [registersArray + 80], rax

    mov rax, [rsp + 96]         ; r10 original
    mov [registersArray + 88], rax

    mov rax, [rsp + 104]        ; r11 original
    mov [registersArray + 96], rax

    mov rax, [rsp + 112]        ; r12 original
    mov [registersArray + 104], rax

    mov rax, [rsp + 120]        ; r13 original
    mov [registersArray + 112], rax

    mov rax, [rsp + 128]        ; r14 original
    mov [registersArray + 120], rax

    mov rax, [rsp + 136]        ; r15 original
    mov [registersArray + 128], rax

	mov rax, [rsp + 144] ; rip original (guardado por hardware)
	mov [registersArray + 136], rax

    sti
    ret

;get_snapshot:
;	cli ; evitamos interrupciones durante la captura del snapshot
;
;	;pushState
;	pushfq
;	pop qword [registersArray] ; guardamos rflags
;	
;	mov [registersArray + 8], rax
;	mov rax, [rsp + 24] ; 
;
;	lea rax, [registersArray + 16]
;	mov [rax], rbx
;	add rax, 8
;	mov [rax], rcx
;	add rax, 8
;	mov [rax], rdx
;	add rax, 8
;	mov [rax], rsi
;	add rax, 8
;	mov [rax], rdi
;	add rax, 8
;	mov [rax], rbp ; 
;	add rax, 8
;	mov [rax], rsp ; 
;	add rax, 8
;	mov [rax], r8
;	add rax, 8
;	mov [rax], r9
;	add rax, 8
;	mov [rax], r10
;	add rax, 8
;	mov [rax], r11
;	add rax, 8
;	mov [rax], r12
;	add rax, 8
;	mov [rax], r13
;	add rax, 8
;	mov [rax], r14
;	add rax, 8
;	mov [rax], r15
;
;	; restauramos rax
;	mov rax, [registersArray + 8]
;	;popState
;	sti
;	ret



section .bss
	registersArray resq 18
