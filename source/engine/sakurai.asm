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
	include "timer.asm"
	include "render.asm"
	include "input.asm"


;==============================
Main:
	; Welcome message
	mov ax, seg_data
	mov ds, ax

	mov dx, str_separator
	call PrintLogString ; (ds:dx)

	mov dx, str_hello
	call PrintLogString ; (ds:dx)

	; Load game
	mov dx, str_game_filename
	call FileOpen ; (ds:dx)

	mov bx, seg_game_data
	mov ds, bx
	mov dx, game_data
	mov cx, 32000
	call FileRead ; (ax = fp, ds:dx = dest, cx = size)
	call FileClose ; (ax)

	; Modules initialization
	call TimeInit
	call InputInit
	call RenderInit
	call IntFDInit

	; Clean buffer memory
	mov ax, seg_buffer_data
	mov es, ax
	mov di, buffer_data
	mov cx, BUFFER_DATA_SIZE

	call MemoryClean

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
		jae Main_loop_time ; No, we don't

		mov bl, 41
		sub bl, al
		mov al, bl

		call TimeSleep ; (ax, ds implicit)
		jmp Main_loop_no_sleep

Main_loop_time:
		;call PrintLogNumber ; (ax)

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

		cmp al, 0x06 ; CODE_DRAW_RECTANGLE_PRECISE
		je DrawRectPrecise

		; Next instruction
Main_loop_instructions_table_continue:
		add si, 8 ; Draw instruction size
		cmp si, INSTRUCTIONS_TABLE_SIZE
		jb Main_loop_instructions_table

Main_loop_instructions_table_break:

		; Copy from buffer to VGA memory
		mov ax, seg_buffer_data
		mov ds, ax
		mov si, buffer_data

		mov ax, VGA_SEGMENT
		mov es, ax
		mov di, VGA_OFFSET

		mov cx, BUFFER_DATA_SIZE
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
include "shared.inc"
