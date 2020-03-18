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


; [input.asm]
; - Alexander Brandt 2020


;==============================
InputInit:
; http://retired.beyondlogic.org/interrupts/interupt.htm#2
; http://www.ctyme.com/intr/rb-5956.htm

	push ax
	push bx
	push dx
	push ds
	push es

	mov ax, seg_data ; The messages and previous vector lives here
	mov ds, ax

	mov dx, str_input_init
	call PrintLogString ; (ds:dx)

	; Get current vector (Int 21/AH=35h)
	; http://www.ctyme.com/intr/rb-2740.htm
	mov ah, 0x35
	mov al, 0x09 ; Interrupt number
	int 0x21

	mov [input_previous_vector_sector], es
	mov [input_previous_vector_offset], bx

	; Print it
	mov dx, str_segment
	call PrintLogString ; (ds:dx)

	mov ax, es
	call PrintLogNumber ; (ax)

	mov dx, str_offset
	call PrintLogString ; (ds:dx)

	mov ax, bx
	call PrintLogNumber ; (ax)

	; Set new vector (Int 21/AH=25h)
	; http://www.ctyme.com/intr/rb-2602.htm
	mov ax, seg_code
	mov ds, ax
	mov dx, _InputVector

	mov al, 0x09 ; Interrupt number
	mov ah, 0x25
	int 0x21

	; Bye!
	pop es
	pop ds
	pop dx
	pop bx
	pop ax
	ret


;==============================
_InputVector:
; https://stackoverflow.com/a/40963633
; http://www.ctyme.com/intr/rb-0045.htm#Table6
; TODO: the previous two references didn't use the
; following method?, (Why I cited these?)

; https://wiki.osdev.org/%228042%22_PS/2_Controller
; « Using interrupts is easy. When IRQ1 occurs you just read
; from IO Port 0x60 (there is no need to check bit 0 in the
; Status Register first), send the EOI to the interrupt
; controller and return from the interrupt handler. You know
; that the data came from the first PS/2 device because you
; received an IRQ1.» (OSDEV)

	push ax
	push bx
	push dx
	push ds

	mov ax, seg_data ; The keys state lives here
	mov ds, ax

	; Retrieve input in AL
	mov dx, 0x60 ; PROTIP: call at least once after the
	in al, dx    ; custom vector is set. Or grab a cup of
	             ; coffee to see how DOS bugs (DOSBox at least)

	; Determine if is an 'release' key, we discard them
	; https://wiki.osdev.org/PS/2_Keyboard
	cmp al, 0x80 ; 0x80 is the last 'press' key,
	             ; anything higher is a 'release'
	             ; (technically isn't the last, but...)

	ja _InputVector_bye ; Jump if Above

	; A keyboard with more than 84 keys
	; Eewwww... we don't do that here!
	cmp al, INPUT_STATE_LEN
	jae _InputVector_bye ; Jump if Above

	; Set our input state
	mov bh, 0x00
	mov bl, al
	mov byte [input_state + bx], 0x01 ; Press

_InputVector_bye:

	; Notify PIC to end this interruption? (TODO)
	; http://stanislavs.org/helppc/8259.html
	mov dx, 0x20
	mov al, 0x20
	out dx, al

	; Bye!
	pop ds
	pop dx
	pop bx
	pop ax
	iret


;==============================
InputStop:
	push ax
	push dx
	push ds

	mov ax, seg_data ; To retrieve previous vector
	mov ds, ax

	mov dx, str_input_stop
	call PrintLogString ; (ds:dx)

	; Print previous vector
	mov dx, str_segment
	call PrintLogString ; (ds:dx)

	mov ax, [input_previous_vector_sector]
	call PrintLogNumber ; (ax)

	mov dx, str_offset
	call PrintLogString ; (ds:dx)

	mov ax, [input_previous_vector_offset]
	call PrintLogNumber ; (ax)

	; Restore previous vector (Int 21/AH=25h)
	; http://www.ctyme.com/intr/rb-2602.htm
	mov dx, [input_previous_vector_offset] ; PROTIP: next one changes ds
	mov ax, [input_previous_vector_sector]
	mov ds, ax
	mov al, 0x09 ; Interrupt number
	mov ah, 0x25
	int 0x21

	; Bye!
	pop ds
	pop dx
	pop ax
	ret


;==============================
InputClean:
	push bx
	mov bx, (INPUT_STATE_LEN-1)

InputClean_loop:
	mov byte [input_state + bx], 0x00 ; Release
	dec bx
	jnz InputClean_loop

	pop bx
	ret
