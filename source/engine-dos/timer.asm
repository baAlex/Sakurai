;
; MIT License
;
; Copyright (c) 2020 Alexander Brandt
;
; Permission is hereby granted, free of charge, to any person obtaining a copy
; of this software and associated documentation files (the "Software"), to deal
; in the Software without restriction, including without limitation the rights
; to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
; copies of the Software, and to permit persons to whom the Software is
; furnished to do so, subject to the following conditions:
;
; The above copyright notice and this permission notice shall be included in all
; copies or substantial portions of the Software.
;
; THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
; IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
; FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
; AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
; LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
; OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
; SOFTWARE.


; [timer.asm]
; - Alexander Brandt 2020


;==============================
TimeInit:
; http://retired.beyondlogic.org/interrupts/interupt.htm#2
; http://www.osdever.net/bkerndev/Docs/pit.htm
; http://www.intel-assembler.it/portale/5/Programming-the-Intel-8253-8354-pit/howto-program-intel-pit-8253-8254.asp

	push ax
	push bx
	push dx
	push ds
	push es

	mov ax, seg_data ; The messages and previous vector lives here
	mov ds, ax

	mov dx, str_time_init
	call near PrintLogString ; (ds:dx)

	; Get current vector (Int 21/AH=35h)
	; http://www.ctyme.com/intr/rb-2740.htm
	mov ah, 0x35
	mov al, 0x08 ; Interrupt number
	int 0x21

	mov [time_previous_vector_sector], es
	mov [time_previous_vector_offset], bx

	; Print it
	mov dx, str_vector_segment
	call near PrintLogString ; (ds:dx)

	mov ax, es
	call near PrintLogNumber ; (ax)

	mov dx, str_vector_offset
	call near PrintLogString ; (ds:dx)

	mov ax, bx
	call near PrintLogNumber ; (ax)

	; Set new vector (Int 21/AH=25h)
	; http://www.ctyme.com/intr/rb-2602.htm
	SetDsDx seg_code, _TimeVector
	mov al, 0x08 ; Interrupt number
	mov ah, 0x25
	int 0x21

	; Set mode
	; https://en.wikipedia.org/wiki/Intel_8253
	; http://stanislavs.org/helppc/ports.html
	; http://stanislavs.org/helppc/8253.html
	mov dx, 0x43 ; Mode Control Register
	mov al, 00110110b ; Mode
	; bits 7-6: '00' = Counter 0
	; bits 5-4: '11' = Write low byte then high byte
	; bits 3-1: '011' = Square wave generator, the clock
	;                   reset itself without Cpu indication
	; bit 0: '0' = Binary counter
	out dx, al

	; Set frequency
	mov dx, 0x40 ; Counter 0
	mov ax, 1193 ; PIT frequency / 1000... an approximation
	out dx, al
	mov al, ah
	out dx, al

	; Bye!
	pop es
	pop ds
	pop dx
	pop bx
	pop ax
	ret


;==============================
_TimeVector:
	push ax
	push dx
	push ds

	mov ax, seg_data ; The millisecond counter lives here
	mov ds, ax

	inc word [time_milliseconds]

	; Notify PIC to end this interruption? (TODO)
	; http://stanislavs.org/helppc/8259.html
	mov dx, 0x20
	mov al, 0x20
	out dx, al

	; Bye!
	pop ds
	pop dx
	pop ax
	iret


;==============================
TimeStop:
	push ax
	push dx
	push ds

	mov ax, seg_data ; To retrieve previous vector
	mov ds, ax

	mov dx, str_time_stop
	call near PrintLogString ; (ds:dx)

	; Print previous vector
	mov dx, str_vector_segment
	call near PrintLogString ; (ds:dx)

	mov ax, [time_previous_vector_sector]
	call near PrintLogNumber ; (ax)

	mov dx, str_vector_offset
	call near PrintLogString ; (ds:dx)

	mov ax, [time_previous_vector_offset]
	call near PrintLogNumber ; (ax)

	; Restore previous vector (Int 21/AH=25h)
	; http://www.ctyme.com/intr/rb-2602.htm
	mov dx, [time_previous_vector_offset] ; PROTIP: next one changes ds
	mov ax, [time_previous_vector_sector]
	mov ds, ax
	mov al, 0x08 ; Interrupt number
	mov ah, 0x25
	int 0x21

	; Restore default mode, frequency (18.2)
	mov dx, 0x43 ; Mode Control Register
	mov al, 00110110b ; This is the default mode? (FIXME)
	out dx, al

	mov dx, 0x40 ; Counter 0
	mov ax, 0 ; Default is 'no divisor'
	out dx, al
	mov al, ah
	out dx, al

	; Bye!
	pop ds
	pop dx
	pop ax
	ret


;==============================
TimeSleep:
; ax - Milliseconds to sleep
; ds - Implicit

	add ax, [time_milliseconds]

TimeSleep_loop:
	nop
	nop
	nop
	nop

	hlt ; Sleep until the PIC wake up us

	cmp [time_milliseconds], ax
	jb TimeSleep_loop ; Jump if Below

	; Bye!
	ret


;==============================
TimeGet:
; ax - Returns milliseconds since initialization
; ds - Implicit

	mov ax, [time_milliseconds]
	ret
