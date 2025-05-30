global sys_read
global sys_write
global sys_get_registers
global sys_beep
global sys_set_font_size
global sys_clear_screen
global sys_draw_pixel
global sys_draw_rectangle
global sys_draw_letter
global sys_get_screen_info
global sys_nano_sleep
global sys_get_time
global sys_get_key
global sys_write_color

section .text


%macro base_syscall 1
    push rbp
    mov rbp, rsp
    mov rax, %1
    int 80h
    mov rsp, rbp
    pop rbp
    ret
%endmacro


sys_read:             base_syscall 3
sys_write:            base_syscall 4
; sys_write_color:      base_syscall 12
sys_nano_sleep:       base_syscall 2
sys_get_time:         base_syscall 0
sys_set_font_size:    base_syscall 1
sys_clear_screen:     base_syscall 5
sys_draw_pixel:       base_syscall 6
sys_draw_rectangle:   base_syscall 7
sys_draw_letter:      base_syscall 8
sys_get_screen_info:  base_syscall 9
sys_get_registers:    base_syscall 10
sys_beep:             base_syscall 11
sys_get_key:          base_syscall 20
