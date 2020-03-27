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
use32
heap 0

entry seg_code:Main


segment seg_code
	include "io.asm"
	include "draw-instructions.asm"
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
	call near PrintLogString ; (ds:dx)

	mov dx, str_hello
	call near PrintLogString ; (ds:dx)

	; Load game
	mov dx, str_game_filename
	call near FileOpen ; (ds:dx)

	mov bx, seg_game_data
	mov ds, bx
	mov dx, game_data
	mov cx, GAME_SIZE
	call near FileRead ; (ax = fp, ds:dx = dest, cx = size)
	call near FileClose ; (ax)

	; Modules initialization
	call near TimeInit
	call near InputInit
	call near RenderInit
	call near IntFDInit

	; Clean buffer memory
	mov ax, seg_buffer_data
	mov es, ax
	mov di, buffer_data
	mov cx, BUFFER_DATA_SIZE

	call near MemoryClean

	; Measure how much took a copy of an entry
	; segment into the VGA memory (ps: a lot)
	mov ax, seg_data
	mov ds, ax

	call near TimeGet ; (ax = return, ds implicit)
	mov bx, ax ; Start time

		mov ax, seg_buffer_data
		mov ds, ax
		mov si, bkg_data

		mov ax, VGA_SEGMENT
		mov es, ax
		mov di, VGA_OFFSET

		mov cx, BKG_DATA_SIZE
		call near MemoryCopy ; (ds:si = source, es:di = destination, cx)

	mov ax, seg_data ; TimeGet() requires it
	mov ds, ax
	call near TimeGet ; (ax = return, ds implicit)

	mov dx, str_copy_speed
	call near PrintLogString ; (ds:dx)

	sub ax, bx
	call near PrintLogNumber ; (ax)

	; Main loop
	mov ax, seg_data ; From here no call should change this (TODO)
	mov ds, ax

	mov dx, str_main_loop
	call near PrintLogString ; (ds:dx)

	jmp near Main_loop_no_sleep ; To avoid the first sleep

Main_loop:

		; After the previous frame we sleep
		cmp ax, 41 ; We did it before the 41 ms?
		jae near Main_loop_time ; No, we don't

		mov bl, 41
		sub bl, al
		mov al, bl

		call near TimeSleep ; (ax, ds implicit)
		jmp near Main_loop_no_sleep

Main_loop_time:
		call near PrintLogNumber ; (ax)

Main_loop_no_sleep:

		; Start time
		call near TimeGet ; (ax = return, ds implicit)
		mov bx, ax

		; Check ESC key (0x01)
		dec byte [input_state + 0x01]
		jz near Main_bye

		; Game logic frame
		; We do a call near into spooky far lands
		call far seg_game_data:GameFrame
		call near InputClean ; (ds implicit)

		; Iterate draw instructions table and do
		; what is required by the game logic
		push bx
		push ds

		mov ax, seg_game_data
		mov ds, ax
		mov si, 0x0000

Main_loop_instructions_table:
		mov eax, [si] ; Code, Color, Width, Height, Filename
		mov ebx, [si + 4] ; X, Y

		; call near PrintLogNumber

		cmp al, 0x00 ; CODE_HALT
		je near Main_loop_instructions_table_break

		cmp al, 0x01 ; CODE_DRAW_BKG
		je near DrawBkg

		cmp al, 0x02 ; CODE_DRAW_PIXEL
		je near DrawPixel

		cmp al, 0x04 ; CODE_DRAW_RECTANGLE
		je near DrawRect

		cmp al, 0x05 ; CODE_DRAW_RECTANGLE_BKG
		je near DrawRectBkg

		cmp al, 0x06 ; CODE_DRAW_RECTANGLE_PRECISE
		je near DrawRectPrecise

		cmp al, 0x07 ; CODE_DRAW_SPRITE
		je near DrawSprite

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
		call near MemoryCopy ; (ds:si = source, es:di = destination, cx)

		pop ds
		pop bx

		; End time
		call near TimeGet ; (ax = return, ds implicit)
		sub ax, bx

		jmp near Main_loop

Main_bye:

	; Bye!
	call near IntFDStop
	call near RenderStop
	call near InputStop
	call near TimeStop

	mov al, EXIT_SUCCESS
	call near Exit ; (al)


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
	push bx
	push cx
	push dx
	push ds

	mov ax, seg_game_data
	mov ds, ax

	mov ax, [ifd_arg1]

	cmp ax, 0x01
	je near _IntFDVector_print_string

	cmp ax, 0x02
	je near _IntFDVector_print_number

	cmp ax, 0x03
	je near _IntFDVector_load_background

_IntFDVector_print_string:
	mov dx, [ifd_arg2]
	call near PrintLogString ; (ds:dx)
	jmp near _IntFDVector_bye

_IntFDVector_print_number:
	mov ax, [ifd_arg2]
	call near PrintLogNumber ; (ax)
	jmp near _IntFDVector_bye

_IntFDVector_load_background:

	mov dx, [ifd_arg2]
	call near FileOpen ; (ds:dx)

	mov bx, seg_bkg_data
	mov ds, bx
	mov dx, 0x0000
	mov cx, BKG_DATA_SIZE
	call near FileRead ; (ax = fp, ds:dx = dest, cx = size)

	call near FileClose ; (ax)
	jmp near _IntFDVector_bye

	; Notify PIC to end this interruption? (TODO)
	; http://stanislavs.org/helppc/8259.html
_IntFDVector_bye:
	mov dx, 0x20
	mov al, 0x20
	out dx, al

	; Bye!
	pop ds
	pop dx
	pop cx
	pop bx
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
