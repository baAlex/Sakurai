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


; [test-spr.asm]
; - Alexander Brandt 2020

; fasm ./source/engine/test-spr.asm ./test.exe

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

	; Modules initialization
	call TimeInit
	call InputInit
	call RenderInit

	; Clean buffer memory
	mov ax, seg_buffer_data
	mov es, ax
	mov di, buffer_data
	mov cx, BUFFER_DATA_SIZE

	call MemoryClean

	; Start time
	mov ax, seg_data ; From here no call should change this (TODO)
	mov ds, ax

	call TimeGet ; (ax = return, ds implicit)
	mov bx, ax

	; ########################
	; Test Test Test

	; Destination
	push ds

	mov bx, seg_buffer_data
	mov es, bx
	mov di, buffer_data + (320 * 100 + 160)

	mov bx, seg_test
	mov ds, bx
	mov si, [0]

	call seg_test:test_data + 2

	pop ds

	; ########################

Main_loop_instructions_table_continue:
	push ds

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

	; End time
	call TimeGet ; (ax = return, ds implicit)
	sub ax, bx

	; Developers, Developers, Developers...
	call PrintLogNumber ; (ax)

	; Check ESC key (0x01)
Main_exit_loop:
	mov al, byte [input_state + 0x01]
	cmp al, 0
	jz Main_exit_loop

Main_bye:

	; Bye!
	call RenderStop
	call InputStop
	call TimeStop

	mov al, EXIT_SUCCESS
	call Exit ; (al)


;==============================
include "shared.inc"


;==============================
segment seg_test
test_data: file "../../assets/sprite2.jvn"
