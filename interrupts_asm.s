.set IRQ_BASE, 0x20

.section .text
.extern _ZN16InterruptManager15HandleInterruptEhj
.global _ZN16InterruptManager22IgnoreInterruptRequestEv

.data
    interruptNum: .byte 0


.macro HandleExceptions num
.global _ZN16InterruptManager16HandleExceptions\num\()Ev
_ZN16InterruptManager16HandleExceptions\num\()Ev:
    movb $\num, (interruptNum)
    jmp int_bottom
.endm


.macro HandleInterruptRequest num
.global _ZN16InterruptManager26HandleInterruptRequest\num\()Ev
_ZN16InterruptManager26HandleInterruptRequest\num\()Ev:
    movb $\num + IRQ_BASE, (interruptNum)
    jmp int_bottom
.endm


HandleInterruptRequest 0x00
HandleInterruptRequest 0x01

int_bottom:
    pusha
    pushl %ds
    pushl %es
    pushl %fs
    pushl %gs

    push %esp
    push (interruptNum)
    call _ZN16InterruptManager15HandleInterruptEhj

    movl %eax, %esp

    popl %gs
    popl %fs
    popl %es
    popl %ds
    popa

_ZN16InterruptManager22IgnoreInterruptRequestEv:
    iret