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

	; Self modifying code ("jmp near bx")
	; http://ref.x86asm.net/coder32.html
	SetDsBx seg_pool_a, spr_a_draw
	mov byte [bx], 0xFF
	mov byte [bx + 1], 0xE3

	SetDsBx seg_pool_b, spr_b_draw
	mov byte [bx], 0xFF
	mov byte [bx + 1], 0xE3

	; Memory pool initialization
	SetDsDx seg_pool_a, pool_a_data
	mov cx, POOL_A_SIZE
	call near PoolInit ; (ds:dx, cx)
	call near PoolPrint ; (ds:dx)

	SetDsDx seg_pool_b, pool_b_data
	mov cx, POOL_B_SIZE
	call near PoolInit ; (ds:dx, cx)
	call near PoolPrint ; (ds:dx)

	; Modules initialization
	call near TimeInit
	call near KeyboardInit
	call near RenderInit
	call near IntFDInit

	; Main loop
	mov dx, seg_data ; From here no call should change this (TODO)
	mov ds, dx

	mov dx, str_main_loop
	call near PrintLogString ; (ds:dx)

	jmp near Main_loop_no_sleep ; To avoid the first sleep

Main_loop:

		; After the previous frame we sleep
		mov bl, 41

		cmp al, bl ; We did it before the 41 ms?
		jae near Main_loop_time ; No, we don't

		sub bl, al
		mov al, bl

		call near TimeSleep ; (ax, ds implicit)
		;jmp near Main_loop_no_sleep

Main_loop_time:
		;call near PrintLogNumber ; (ax)

Main_loop_no_sleep:

		; Start time
		call near TimeGet ; (ax = return, ds implicit)
		mov bx, ax

		; Is a good bye?
		dec word [exit_request]
		jz near Main_bye

		; Provide input to game logic
		; Aka: Copy things from 'seg_data' to 'seg_game_data'
		push ax ; Has the TimeGet() return

		mov dl, byte [keyboard_state + 0x2C] ; X, 0x2C = 'Z'
		mov dh, byte [keyboard_state + 0x1C] ; X, 0x1C = Enter
			or dl, dh
			xor dh, dh
			push dx
		mov dl, [keyboard_state + 0x2D] ; Y, 0x2D = 'X'
		mov dh, [keyboard_state + 0x0E] ; Y, 0x0E = Backspace
			or dl, dh
			xor dh, dh
			push dx
		mov dx, [keyboard_state + 0x48] ; Up, 0x48 = Arrow
			push dx
		mov dx, [keyboard_state + 0x50] ; Down, 0x50 = Arrow
			push dx
		mov dx, [keyboard_state + 0x4B] ; Left, 0x4B = Arrow
			push dx
		mov dx, [keyboard_state + 0x4D] ; Right, 0x4D = Arrow
			push dx
		mov dx, [keyboard_state + 0x39] ; Select, 0x39 = Space
			push dx
		mov dx, [keyboard_state + 0x01] ; Start, 0x01 = Escape
			push dx

		; Welcome to 'seg_game_data'
		mov dx, seg_game_data
		mov ds, dx

		pop dx
		mov byte [input_start], dl
		pop dx
		mov byte [input_select], dl
		pop dx
		mov byte [input_pad_r], dl
		pop dx
		mov byte [input_pad_l], dl
		pop dx
		mov byte [input_pad_d], dl
		pop dx
		mov byte [input_pad_u], dl
		pop dx
		mov byte [input_y], dl
		pop dx
		mov byte [input_x], dl
		pop dx
		mov word [ms_counter], dx

		; Call a game frame
		pusha
		call far seg_game_data:GameFrame
		popa

		inc word [frame_counter]

		; Iterate commands table, drawing in a buffer
		mov si, commands_table
		mov dx, seg_buffer_data
		mov es, dx

		call near IterateGameCommands ; (ds:si = commands table, es:0x0000 = destination buffer)

		; Copy from buffer to VGA memory
		mov dx, seg_buffer_data
		mov ds, dx
		mov si, buffer_data

		mov dx, VGA_SEGMENT
		mov es, dx
		mov di, VGA_OFFSET

		mov cx, BUFFER_DATA_SIZE
		call near MemoryCopy ; (ds:si = source, es:di = destination, cx)

		; Welcome back to 'seg_data' (where everything is near)
		mov dx, seg_data
		mov ds, dx

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
