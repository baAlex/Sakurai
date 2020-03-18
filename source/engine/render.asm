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


; [render.asm]
; - Alexander Brandt 2020


;==============================
RenderInit:
	push ax
	push bx
	push ds

	mov ax, seg_data ; Messages, previous mode and palette...
	mov ds, ax

	mov dx, str_render_init
	call PrintLogString ; (ds:dx)

	; Load palette
	mov dx, str_palette_filename
	call FileOpen ; (ds:dx)

	mov dx, render_palette_data
	mov cx, PALETTE_SIZE
	call FileRead ; (ax = fp, ds:dx = dest, cx = size)
	call FileClose ; (ax)

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
