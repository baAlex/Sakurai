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


; [sakurai.asm]
; - Alexander Brandt 2020

; https://en.wikipedia.org/wiki/Sakurai%2C_Nara
; https://flatassembler.net/docs.php?article=manual#2.4.1
; https://www.tutorialspoint.com/assembly_programming/assembly_conditions.htm
; https://groups.google.com/forum/#!topic/comp.lang.asm.x86/1rfHRzQy9xk

; $ fasm ./sakurai.asm

format MZ
entry seg_code:Main
heap 0


segment seg_code
	include "io.asm"
	include "instructions.asm"
	include "memory.asm"


;==============================
Main:
	; Welcome message
	mov ax, seg_data
	mov ds, ax

	mov dx, str_separator
	call PrintLogString ; (ds:dx)

	mov dx, str_hello
	call PrintLogString ; (ds:dx)

	; Modules initialization
	call TimeInit
	call InputInit
	call RenderInit
	call IntFDInit

	; Measure how much took a copy of an entry
	; segment into the VGA memory (ps: a lot)
	mov ax, seg_data
	mov ds, ax

	call TimeGet ; (ax = return, ds implicit)
	mov bx, ax ; Start time

		mov ax, seg_buffer_data
		mov ds, ax
		mov si, bkg_data

		mov ax, VGA_SEGMENT
		mov es, ax
		mov di, VGA_OFFSET

		mov cx, BKG_DATA_SIZE
		call MemoryCopy ; (ds:si = source, es:di = destination, cx)

	mov ax, seg_data ; TimeGet() requires it
	mov ds, ax
	call TimeGet ; (ax = return, ds implicit)

	mov dx, str_copy_speed
	call PrintLogString ; (ds:dx)

	sub ax, bx
	call PrintLogNumber ; (ax)

	; Main loop
	mov ax, seg_data ; From here no call should change this (TODO)
	mov ds, ax

	mov dx, str_main_loop
	call PrintLogString ; (ds:dx)

	jmp Main_loop_no_sleep ; To avoid the first sleep

Main_loop:

		; After the previous frame we sleep
		cmp ax, 41 ; We did it before the 41 ms?
		jae Main_loop_scream ; No, we don't

		mov bl, 41
		sub bl, al
		mov al, bl

		call TimeSleep ; (ax, ds implicit)
		jmp Main_loop_no_sleep ; To ignore the scream

Main_loop_scream:
		mov dx, str_scream
		call PrintLogString ; (ds:dx)

Main_loop_no_sleep:

		; Start time
		call TimeGet ; (ax = return, ds implicit)
		mov bx, ax

		; Check ESC key (0x01)
		dec byte [input_state + 0x01]
		jz Main_bye

		; Game logic frame
		; We do a call into spooky far lands
		call seg_game_data:GameFrame ; Far call
		call InputClean ; (ds implicit)

		; Iterate instructions table and do
		; what is required by the game logic
		push bx
		push ds

		mov ax, seg_game_data
		mov ds, ax
		mov si, 0x0000

Main_loop_instructions_table:
		mov eax, [si] ; Code, Color, Width, Height, Filename
		mov ebx, [si + 4] ; X, Y

		; call PrintLogNumber

		cmp al, 0x00 ; CODE_HALT
		je Main_loop_instructions_table_break

		cmp al, 0x01 ; CODE_DRAW_BKG
		je DrawBkg

		cmp al, 0x02 ; CODE_DRAW_PIXEL
		je DrawPixel

		cmp al, 0x03 ; CODE_LOAD_BKG
		je LoadBkg

		cmp al, 0x04 ; CODE_DRAW_RECTANGLE
		je DrawRect

		cmp al, 0x05 ; CODE_DRAW_RECTANGLE_BKG
		je DrawRectBkg

		; Next instruction
Main_loop_instructions_table_continue:
		add si, 8 ; Draw instruction size
		cmp si, INSTRUCTIONS_TABLE_SIZE
		jb Main_loop_instructions_table

Main_loop_instructions_table_break:

		; Copy from buffer to VGA memory
		mov ax, seg_buffer_data
		mov ds, ax
		mov si, bkg_data

		mov ax, VGA_SEGMENT
		mov es, ax
		mov di, VGA_OFFSET

		mov cx, BKG_DATA_SIZE
		call MemoryCopy ; (ds:si = source, es:di = destination, cx)

		pop ds
		pop bx

		; End time
		call TimeGet ; (ax = return, ds implicit)
		sub ax, bx

		; Developers, Developers, Developers...
		; call PrintLogNumber ; (ax)
		; call Exit ; (al)

		jmp Main_loop

Main_bye:

	; Bye!
	call IntFDStop
	call RenderStop
	call InputStop
	call TimeStop

	mov al, EXIT_SUCCESS
	call Exit ; (al)


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
	call PrintLogString ; (ds:dx)

	; Get current vector (Int 21/AH=35h)
	; http://www.ctyme.com/intr/rb-2740.htm
	mov ah, 0x35
	mov al, 0x08 ; Interrupt number
	int 0x21

	mov [time_previous_vector_sector], es
	mov [time_previous_vector_offset], bx

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
	mov dx, _TimeVector

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
	call PrintLogString ; (ds:dx)

	; Print previous vector
	mov dx, str_segment
	call PrintLogString ; (ds:dx)

	mov ax, [time_previous_vector_sector]
	call PrintLogNumber

	mov dx, str_offset
	call PrintLogString ; (ds:dx)

	mov ax, [time_previous_vector_offset]
	call PrintLogNumber

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
	call PrintLogNumber

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


;==============================
RenderInit:
	push ax
	push bx
	push ds

	mov ax, seg_data ; Messages, previous mode and palette...
	mov ds, ax

	mov dx, str_render_init
	call PrintLogString ; (ds:dx)

	; Get current video mode (Int 10/AH=0Fh)
	; http://www.ctyme.com/intr/rb-0108.htm
	mov ah, 0x0F
	int 0x10

	mov [render_previous_mode], al

	; Set VGA mode 13 (Int 10/AH=00h)
	; http://www.ctyme.com/intr/rb-0069.htm
	mov ah, 0x00
	mov al, 0x13
	int 0x10

	; Get current video mode, again, to check (Int 10/AH=0Fh)
	; Looks like different chips, return different values when
	; the mode is set (above section). So we need to call this
	; interrupt again.
	mov ah, 0x0F
	int 0x10

	sub al, 0x13
	jnz RenderInit_failure

	; Load palette
	; http://stanislavs.org/helppc/ports.html

	mov dx, 0x03C8 ; VGA video DAC PEL address
	mov al, 0x00 ; Color index
	out dx, al

	mov ah, (PALETTE_LEN-1)
	mov bx, 0 ; Indexing purposes
	mov dx, 0x03C9 ; VGA video DAC

RenderInit_palette_loop:
	mov al, [render_palette_data + bx]
	out dx, al
	inc bx

	mov al, [render_palette_data + bx]
	out dx, al
	inc bx

	mov al, [render_palette_data + bx]
	out dx, al
	inc bx

	dec ah
	jnz RenderInit_palette_loop

	; Bye!
	pop ds
	pop bx
	pop ax
	ret

RenderInit_failure:
	mov dx, str_vga_error
	call PrintOut ; (ds:dx)

	mov al, EXIT_FAILURE
	call Exit ; (al)


;==============================
RenderStop:
	push ax
	push ds

	mov ax, seg_data ; To retrieve previous mode
	mov ds, ax

	mov dx, str_render_stop
	call PrintLogString ; (ds:dx)

	; Set previous mode (Int 10/AH=00h)
	; http://www.ctyme.com/intr/rb-0069.htm
	mov ah, 0x00
	mov al, [render_previous_mode]
	int 0x10

	; Bye!
	pop ds
	pop ax
	ret


;==============================
IntFDInit:
	push ax
	push ds

	mov ax, seg_data
	mov ds, ax

	; DOS 2+ - GET INTERRUPT VECTOR
	; http://www.ctyme.com/intr/rb-2740.htm
	mov ah, 0x35
	mov al, 0xFD ; Interrupt number
	int 0x21

	mov [previous_ifd_vector_segment], es
	mov [previous_ifd_vector_offset], bx

	; DOS 1+ - SET INTERRUPT VECTOR
	; http://www.ctyme.com/intr/rb-2602.htm
	mov ax, seg_code
	mov ds, ax
	mov dx, _IntFDVector
	mov ah, 0x25
	mov al, 0xFD ; Interrupt number
	int 0x21

	; Bye!
	pop ds
	pop ax
	ret


;==============================
_IntFDVector:
; http://www.ctyme.com/intr/rb-8735.htm

	push ax
	push dx

	mov ax, seg_game_data
	mov ds, ax

	mov ax, [ifd_arg1]

	cmp ax, 0x01
	je _IntFDVector_print_string

	cmp ax, 0x02
	je _IntFDVector_print_number

_IntFDVector_print_string:
	mov dx, [ifd_arg2]
	call PrintLogString ; (ds:dx)
	jmp _IntFDVector_bye

_IntFDVector_print_number:
	mov ax, [ifd_arg2]
	call PrintLogNumber ; (ax)
	jmp _IntFDVector_bye


	; Notify PIC to end this interruption? (TODO)
	; http://stanislavs.org/helppc/8259.html
	mov dx, 0x20
	mov al, 0x20
	out dx, al

	; Bye!
_IntFDVector_bye:
	pop dx
	pop ax
	iret


;==============================
IntFDStop:
	push ax
	push ds

	mov ax, seg_data
	mov ds, ax

	; DOS 1+ - SET INTERRUPT VECTOR
	; http://www.ctyme.com/intr/rb-2602.htm
	mov ax, [previous_ifd_vector_segment]
	mov ds, ax
	mov dx, [previous_ifd_vector_offset]
	mov ah, 0x25
	mov al, 0xFD ; Interrupt number
	int 0x21

	; Bye!
	pop ds
	pop ax
	ret


;==============================
include "sakurai.inc"
