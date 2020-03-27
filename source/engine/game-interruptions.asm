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


; [game-interruptions.asm]
; - Alexander Brandt 2020


; Following routines are not functions ('functions' meaning the correct
; care of the fast call convention), rather they are part of a table
; executed in a 'switch' fashion (C lingua)...

; All registers must remain intact!!!, the caller already push some of them.

; Executed by the caller:
;	push ax
;	push dx
;	push ds
;	ds = seg_game_data
;	ax = [ifd_arg1]


;==============================
GamePrintString:
	push dx

	mov dx, [ifd_arg2]
	call near PrintLogString ; (ds:dx)

	pop dx
	jmp near _IntFDVector_bye


;==============================
GamePrintNumber:
	mov ax, [ifd_arg2]
	call near PrintLogNumber ; (ax)

	jmp near _IntFDVector_bye


;==============================
GameLoadBackground:
	push dx
	push bx
	push cx

	mov dx, [ifd_arg2]
	call near FileOpen ; (ds:dx)

	mov bx, seg_bkg_data
	mov ds, bx
	mov dx, 0x0000
	mov cx, BKG_DATA_SIZE
	call near FileRead ; (ax = fp, ds:dx = dest, cx = size)

	call near FileClose ; (ax)

	pop cx
	pop bx
	pop dx
	jmp near _IntFDVector_bye


;==============================
GameLoadSprite:
	push dx

	mov dx, [ifd_arg2]
	call near FileOpen ; (ds:dx)
	call near FileClose ; (ax)

	pop dx
	jmp near _IntFDVector_bye
