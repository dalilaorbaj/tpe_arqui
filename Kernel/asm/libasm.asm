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
	cli ; evitamos interrupciones durante la captura del snapshot

	pushfq
	pop qword [registersArray] ; guardamos rflags
	
	mov [registersArray + 8], rax
	lea rax, [registersArray + 16]
	mov [rax], rbx
	add rax, 8
	mov [rax], rcx
	add rax, 8
	mov [rax], rdx
	add rax, 8
	mov [rax], rsi
	add rax, 8
	mov [rax], rdi
	add rax, 8
	mov [rax], rbp
	add rax, 8
	mov [rax], rsp
	add rax, 8
	mov [rax], r8
	add rax, 8
	mov [rax], r9
	add rax, 8
	mov [rax], r10
	add rax, 8
	mov [rax], r11
	add rax, 8
	mov [rax], r12
	add rax, 8
	mov [rax], r13
	add rax, 8
	mov [rax], r14
	add rax, 8
	mov [rax], r15

	; restauramos rax
	mov rax, [registersArray + 8]

	sti
	ret


section .bss
	registersArray resq 18
