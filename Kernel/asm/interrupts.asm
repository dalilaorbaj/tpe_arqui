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
	pushState

	mov rdi, %1 ; pasaje de parametro
	call exceptionDispatcher

	popState
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

    popState
    iretq


;Zero Division Exception
_exception0Handler:
	exceptionHandler 0

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