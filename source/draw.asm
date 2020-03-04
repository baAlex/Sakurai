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


; [draw.asm]
; - Alexander Brandt 2020


; Following routines are not functions ('functions' meaning the correct
; care of the fast call convention), rather they are part of a table
; executed in a 'switch' fashion (C lingua)...

; So, the only requirement here is to preserve registers DS and SI, and
; always jump to 'Main_loop_draw_table_continue'. And with the only
; exception of AX, BX and CX is better to assume that other registers
; just contain trash.


;==============================
DrawBkg: ; CODE_DRAW_BKG
; ax - unused
; bx - unused
; cx - unused

	push ds
	push si

	mov ax, seg_bkg_data
	mov ds, ax
	mov si, 0x0000

	mov ax, seg_buffer_data
	mov es, ax
	mov di, 0x0000

	mov cx, BKG_DATA_SIZE
	call MemoryCopy ; (ds:si = source, es:di = destination, cx)

	; Bye!
	pop si
	pop ds
	jmp Main_loop_draw_table_continue


;==============================
DrawPixel: ; CODE_DRAW_PIXEL
; ah - color
; bx - X
; cx - Y

	push ds
	push di

	mov di, 0x0000

	; Calculate offset
DrawPixel_vertical:
	add di, 320 ; Y (TODO?)
	dec cx
	jnz DrawPixel_vertical

	add di, bx ; X

	; Draw it!
	mov cx, seg_buffer_data
	mov ds, cx
	mov [di], ah

	; Bye!
	pop di
	pop ds
	jmp Main_loop_draw_table_continue


;==============================
LoadBkg: ; CODE_LOAD_BKG
; ax - unused
; bx - Filename
; cx - unused

	push ds

	; Open file
	mov ax, seg_game_data
	mov ds, ax
	mov dx, bx
	call FileOpen

	; Read into bkg data
	mov bx, seg_bkg_data
	mov ds, bx
	mov dx, 0x0000
	mov cx, BKG_DATA_SIZE
	call FileRead

	; Bye!
	call FileClose

	pop ds
	jmp Main_loop_draw_table_continue
