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
	include "game-commands.asm"
	include "game-interruptions.asm"
	include "io.asm"
	include "keyboard.asm"
	include "memory.asm"
	include "render.asm"
	include "timer.asm"
	include "macros.asm"


;==============================
Main:
	; Welcome message
	mov dx, seg_data
	mov ds, dx

	mov dx, str_separator
	call near PrintLogString ; (ds:dx)

	mov dx, str_hello
	call near PrintLogString ; (ds:dx)

	; Load game
	mov dx, str_game_filename
	call near FileOpen ; (ds:dx, return in ax)

	SetDsDx seg_game_data, game_data
	mov cx, GAME_SIZE
	call near FileRead ; (ax = fp, ds:dx = dest, cx = size)
	call near FileClose ; (ax)

	; Memory pool initialization
	SetDsDx seg_pool_a, pool_a_data
	mov cx, POOL_A_SIZE
	call near PoolInit ; (ds:dx, cx)
	call near PoolPrint ; (ds:dx)

	SetDsDx seg_pool_b, pool_b_data
	mov cx, POOL_B_SIZE
	call near PoolInit ; (ds:dx, cx)
	call near PoolPrint ; (ds:dx)

	; Clean buffer memory
	mov ax, seg_buffer_data
	mov es, ax
	mov di, buffer_data
	mov cx, BUFFER_DATA_SIZE

	call near MemoryClean

	; Modules initialization
	call near TimeInit
	call near KeyboardInit
	call near RenderInit
	call near IntFDInit

	; Measure how much took a copy of an entry
	; segment into the VGA memory (ps: a lot)
	mov dx, seg_data
	mov ds, dx

	call near TimeGet ; (ax = return, ds implicit)
	mov bx, ax ; Start time

		mov dx, seg_buffer_data
		mov ds, dx
		mov si, bkg_data

		mov dx, VGA_SEGMENT
		mov es, dx
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
	mov dx, seg_data ; From here no call should change this (TODO)
	mov ds, dx

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
		dec byte [keyboard_state + 0x01]
		jz near Main_bye

		; Provide input to game logic
		mov dx, [keyboard_state + 0x2C] ; X
		push dx
		mov dx, [keyboard_state + 0x2D] ; Y
		push dx
		mov dx, [keyboard_state + 0x1E] ; A
		push dx
		mov dx, [keyboard_state + 0x1F] ; B
		push dx
		mov dx, [keyboard_state + 0x01] ; Up (TODO)
		push dx
		mov dx, [keyboard_state + 0x01] ; Down (TODO)
		push dx
		mov dx, [keyboard_state + 0x01] ; Left (TODO)
		push dx
		mov dx, [keyboard_state + 0x01] ; Right (TODO)
		push dx
		mov dx, [keyboard_state + 0x39] ; Select
		push dx
		mov dx, [keyboard_state + 0x1C] ; Start
		push dx

		mov dx, seg_game_data
		mov ds, dx

			pop dx
			mov byte [input_start], dl
			pop dx
			mov byte [input_select], dl
			pop dx
			mov byte [input_right], dl
			pop dx
			mov byte [input_left], dl
			pop dx
			mov byte [input_down], dl
			pop dx
			mov byte [input_up], dl
			pop dx
			mov byte [input_b], dl
			pop dx
			mov byte [input_a], dl
			pop dx
			mov byte [input_y], dl
			pop dx
			mov byte [input_x], dl

		mov dx, seg_data
		mov ds, dx

		; Game logic frame
		; We do a call near into spooky far lands
		call far seg_game_data:GameFrame
		call near KeyboardClean ; (ds implicit)

		; Iterate draw commands table and do
		; what is required by the game logic
		push bx
		push ds

		mov dx, seg_game_data
		mov ds, dx
		mov si, 0x0000

Main_loop_commands_table:
		mov eax, [si] ; Code, Color, Width, Height, Filename
		mov ebx, [si + 4] ; X, Y

		; call near PrintLogNumber

		cmp al, 0x00 ; CODE_HALT
		je near Main_loop_commands_table_break

		cmp al, 0x01 ; CODE_DRAW_BKG
		je near GameDrawBkg
		cmp al, 0x02 ; CODE_DRAW_PIXEL
		je near GameDrawPixel
		cmp al, 0x04 ; CODE_DRAW_RECTANGLE
		je near GameDrawRect
		cmp al, 0x05 ; CODE_DRAW_RECTANGLE_BKG
		je near GameDrawRectBkg
		cmp al, 0x06 ; CODE_DRAW_RECTANGLE_PRECISE
		je near GameDrawRectPrecise
		cmp al, 0x07 ; CODE_DRAW_SPRITE
		je near GameDrawSprite
		cmp al, 0x08 ; CODE_DRAW_TEXT
		je near GameDrawText

		; Next command
Main_loop_commands_table_continue:
		add si, 8 ; Draw command size
		cmp si, COMMANDS_TABLE_SIZE
		jb Main_loop_commands_table

Main_loop_commands_table_break:

		; Copy from buffer to VGA memory
		mov dx, seg_buffer_data
		mov ds, dx
		mov si, buffer_data

		mov dx, VGA_SEGMENT
		mov es, dx
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
	call near KeyboardStop
	call near TimeStop

	mov al, EXIT_SUCCESS
	call near Exit ; (al)


;==============================
IntFDInit:
	push ax
	push ds
	push dx

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
	SetDsDx seg_code, _IntFDVector
	mov ah, 0x25
	mov al, 0xFD ; Interrupt number
	int 0x21

	; Bye!
	pop dx
	pop ds
	pop ax
	ret


;==============================
_IntFDVector:
; http://www.ctyme.com/intr/rb-8735.htm

	push ax
	push dx
	push ds

	mov ax, seg_game_data
	mov ds, ax

	mov ax, [ifd_arg1]

	cmp ax, 0x01
	je near GamePrintString
	cmp ax, 0x02
	je near GamePrintNumber
	cmp ax, 0x03
	je near GameLoadBackground
	cmp ax, 0x04
	je near GameLoadSprite
	cmp ax, 0x05
	je near GameUnloadEverything

	; Notify PIC to end this interruption? (TODO)
	; http://stanislavs.org/helppc/8259.html
_IntFDVector_bye:
	mov dx, 0x20
	mov al, 0x20
	out dx, al

	; Bye!
	pop ds
	pop dx
	pop ax
	iret


;==============================
IntFDStop:
	push ax
	push ds
	push dx

	mov ax, seg_data
	mov ds, ax

	; DOS 1+ - SET INTERRUPT VECTOR
	; http://www.ctyme.com/intr/rb-2602.htm
	mov ax, [previous_ifd_vector_segment]
	mov dx, [previous_ifd_vector_offset]
	mov ds, ax
	mov ah, 0x25
	mov al, 0xFD ; Interrupt number
	int 0x21

	; Bye!
	pop dx
	pop ds
	pop ax
	ret


;==============================
include "shared.inc"
