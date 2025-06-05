GLOBAL _cli
GLOBAL _sti
GLOBAL picMasterMask
GLOBAL picSlaveMask
GLOBAL haltcpu
GLOBAL _hlt

GLOBAL _irq00Handler
GLOBAL _irq01Handler
GLOBAL _irq02Handler
GLOBAL _irq03Handler
GLOBAL _irq04Handler
GLOBAL _irq05Handler
GLOBAL _irq80Handler
global _lidt


GLOBAL _exception0Handler
GLOBAL _exception_invalidOpcodeHandler
GLOBAL _exception8Handler

EXTERN registersArrayAux
EXTERN registersArrayException
EXTERN irqDispatcher		;para interrupciones de hardware
EXTERN syscallDispatcher	;para interrupciones de software
EXTERN exceptionDispatcher

SECTION .text

%macro pushState 0
	push rax
	push rbx
	push rcx
	push rdx
	push rbp
	push rdi
	push rsi
	push r8
	push r9
	push r10
	push r11
	push r12
	push r13
	push r14
	push r15
%endmacro

%macro popState 0
	pop r15
	pop r14
	pop r13
	pop r12
	pop r11
	pop r10
	pop r9
	pop r8
	pop rsi
	pop rdi
	pop rbp
	pop rdx
	pop rcx
	pop rbx
	pop rax
%endmacro

%macro popStateMinusRax 0
	pop r15
	pop r14
	pop r13
	pop r12
	pop r11
	pop r10
	pop r9
	pop r8
	pop rsi
	pop rdi
	pop rbp
	pop rdx
	pop rcx
	pop rbx
%endmacro

%macro irqHandlerMaster 1
	pushState

	mov rdi, %1 ; pasaje de parametro
	call irqDispatcher

	; signal pic EOI (End of Interrupt)
	mov al, 20h
	out 20h, al

	popState
	iretq
%endmacro


%macro exceptionHandler 1
    cli
	
	push rax
	mov [registersArrayException + 8], rax
	; guardamos rax antes de modificarlo
	
	mov rax, [rsp + 8*3] ; rax = rflags
	mov [registersArrayException], rax	  ; rflags guardado en el array de registros
    
	
	; la shell llamada a irq01, irq01 llama a keyboardHandler
	; o sea, antes en el stack tengo el stack de la shel

    mov [registersArrayException + 16], rbx      ; rbx
    mov [registersArrayException + 24], rcx      ; rcx
    mov [registersArrayException + 32], rdx      ; rdx
    mov [registersArrayException + 40], rsi      ; rsi
    mov [registersArrayException + 48], rdi      ; rdi
    mov [registersArrayException + 56], rbp      ; rbp

	mov rax, [rsp + 8*4]				; rax = rsp del llamador
	mov [registersArrayException + 64], rax      ; rsp del llamador

    mov [registersArrayException + 72], r8       ; r8
    mov [registersArrayException + 80], r9       ; r9
    mov [registersArrayException + 88], r10      ; r10
    mov [registersArrayException + 96], r11      ; r11
    mov [registersArrayException + 104], r12     ; r12
    mov [registersArrayException + 112], r13     ; r13
    mov [registersArrayException + 120], r14     ; r14
    mov [registersArrayException + 128], r15     ; r15
	
    mov rax, [rsp + 8]                      ; RIP guardado por la CPU
    mov [registersArrayException + 136], rax     ; rip
	mov rax, [rsp + 8*2]
	mov [registersArrayException + 144], rax     ; cs guardado por la CPU
	mov rax, [rsp + 8*5]
	mov [registersArrayException + 152], rax     ; ss guardado por la CPU

	pop rax ; recupero rax

	push rdi
	push rsi

	mov rdi, %1                             
	mov rsi, regs

	call exceptionDispatcher
	pop rsi
	pop rdi

	popState
    call getStackBase
	mov [rsp+24], rax
    mov rax, userland
    mov [rsp], rax

    sti
    iretq
%endmacro


_hlt:
	sti
	hlt
	ret

_cli:
	cli
	ret


_sti:
	sti
	ret

picMasterMask:
	push rbp
    mov rbp, rsp
    mov ax, di
    out	21h,al
    pop rbp
    retn

picSlaveMask:
	push    rbp
    mov     rbp, rsp
    mov     ax, di  ; ax = mascara de 16 bits
    out	0A1h,al
    pop     rbp
    retn


;8254 Timer (Timer Tick)
_irq00Handler:
	irqHandlerMaster 0

;Keyboard
_irq01Handler:

	push rax ; backupeo rax
	; IMPORTANTE: hacer un push mueve el stack, por lo que RIP, RSP, RFLAGS etc tambien se mueven

	; antes en el stack se tenia:
	; EIP de donde provenis
	; CS de donde provenis
	; RFLAGS
	; RSP de donde provenis
	; SS de donde provenis

	mov [registersArrayAux + 8], rax
	; guardamos rax antes de modificarlo
	
	mov rax, [rsp + 8*3] ; rax = rflags
	mov [registersArrayAux], rax	  ; rflags guardado en el array de registros
    
	
	; la shell llamada a irq01, irq01 llama a keyboardHandler
	; o sea, antes en el stack tengo el stack de la shel

    mov [registersArrayAux + 16], rbx      ; rbx
    mov [registersArrayAux + 24], rcx      ; rcx
    mov [registersArrayAux + 32], rdx      ; rdx
    mov [registersArrayAux + 40], rsi      ; rsi
    mov [registersArrayAux + 48], rdi      ; rdi
    mov [registersArrayAux + 56], rbp      ; rbp

	mov rax, [rsp + 8*4]				; rax = rsp del llamador
	mov [registersArrayAux + 64], rax      ; rsp del llamador

    mov [registersArrayAux + 72], r8       ; r8
    mov [registersArrayAux + 80], r9       ; r9
    mov [registersArrayAux + 88], r10      ; r10
    mov [registersArrayAux + 96], r11      ; r11
    mov [registersArrayAux + 104], r12     ; r12
    mov [registersArrayAux + 112], r13     ; r13
    mov [registersArrayAux + 120], r14     ; r14
    mov [registersArrayAux + 128], r15     ; r15
	
    mov rax, [rsp + 8]                      ; RIP guardado por la CPU
    mov [registersArrayAux + 136], rax     ; rip
	mov rax, [rsp + 8*2]
	mov [registersArrayAux + 144], rax     ; cs guardado por la CPU
	mov rax, [rsp + 8*5]
	mov [registersArrayAux + 152], rax     ; ss guardado por la CPU

	pop rax ; recupero rax

	irqHandlerMaster 1

;Cascade pic never called
_irq02Handler:
	irqHandlerMaster 2

;Serial Port 2 and 4
_irq03Handler:
	irqHandlerMaster 3

;Serial Port 1 and 3
_irq04Handler:
	irqHandlerMaster 4

;USB
_irq05Handler:
	irqHandlerMaster 5
                ; vuelve a userland con RAX = retorno de syscall

_irq80Handler:
    pushState

    ; Pasar argumentos según la convención x86-64:
    ; rdi = syscall_num (rax), rsi = arg1 (rdi), rdx = arg2 (rsi), 
    ; rcx = arg3 (rdx), r8 = arg4 (rcx), r9 = arg5 (r8)
    mov r9, r8      ; arg5 = r8
    mov r8, rcx     ; arg4 = rcx  
    mov rcx, rdx    ; arg3 = rdx
    mov rdx, rsi    ; arg2 = rsi
    mov rsi, rdi    ; arg1 = rdi
    mov rdi, rax    ; syscall_num = rax

    call syscallDispatcher

    popStateMinusRax
	
	add rsp, 8 ; restaurar el stack al estado anterior al syscall

    iretq



;Zero Division Exception
_exception0Handler:
	exceptionHandler 0

;double fault exception
_exception8Handler:
    exceptionHandler 8

;Invalid Opcode Exception
_exception_invalidOpcodeHandler:
	exceptionHandler 6

haltcpu:
	cli
	hlt
	ret

_lidt:
    mov rax, [rdi]        ; primero el limit y luego la base
    lidt [rdi]
    ret

SECTION .bss
	aux resq 1

