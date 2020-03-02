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


; [utilities.asm]
; - Alexander Brandt 2020


;==============================
PrintLogString:
; ds:dx - String to print
; cx    - Length

	push ax
	push bx
	push cx
	push dx
	push ds

	mov ax, seg_data ; Log filename lives here
	mov ds, ax

	; Open existing file (Int 21/AH=3Dh)
	; http://www.ctyme.com/intr/rb-2779.htm
	mov ah, 0x3D
	mov al, 0x01 ; Write mode
	mov dx, str_log_filename
	int 0x21

	jnc PrintLogString_file_exists

	; Create file (Int 21/AH=3Ch)
	; http://www.ctyme.com/intr/rb-2778.htm
	mov ah, 0x3C
	mov cx, 0x0000 ; Attributes, all bits to zero
	mov dx, str_log_filename
	int 0x21

	jc PrintLogString_failure1

PrintLogString_file_exists:

	; Seek (Int 21/AH=42h)
	; http://www.ctyme.com/intr/rb-2799.htm
	mov bx, ax ; File handle, both Open() and Create() returns it on AX
	mov ah, 0x42
	mov al, 0x02 ; From EOF
	mov cx, 0x0000 ; Origin (signed) HI
	mov dx, 0x0000 ; Origin (signed) LO
	int 0x21

	jc PrintLogString_failure1

	; Write to file (Int 21/AH=40h)
	; http://www.ctyme.com/intr/rb-2791.htm
	mov ah, 0x40
	pop ds
	pop dx
	pop cx
	int 0x21 ; File handle still on BX

	jc PrintLogString_failure2

	; Close file (Int 21/AH=3Eh)
	; http://www.ctyme.com/intr/rb-2782.htm
	mov ah, 0x3E
	int 0x21 ; File handle still on BX

	; Bye!
	pop bx
	pop ax
	ret

PrintLogString_failure1:
	pop ds
	pop dx
	pop cx

PrintLogString_failure2:
	mov al, EXIT_FAILURE
	call Exit ; (al)


;==============================
PrintLogNumber:
; ax - Number to print

	push bx
	push cx
	push dx
	push ds

	mov bx, seg_data ; HEX table
	mov ds, bx

	; Create the string termination
	mov cx, 0x000A ; Unix NL + NULL
	push cx ; Push Alpha

	; Create the number string
	mov bh, 0x00

	mov bl, al ; Bits 0-3
	and bl, 00001111b
	mov ch, [hex_table + bx]

	mov bl, al ; Bits 4-7
	shr bl, 4
	mov cl, [hex_table + bx]

		push cx ; Push Beta

	mov bl, ah ; Bits 8-11
	and bl, 00001111b
	mov ch, [hex_table + bx]

	mov bl, ah ; Bits 12-15
	shr bl, 4
	mov cl, [hex_table + bx]

		push cx ; Push Gamma

	; Point DS to SS, and DX to SP
	mov bx, ss
	mov ds, bx
	mov dx, sp

	; Print it
	mov cx, 5 ; Length not counting NULL
	call PrintLogString

	pop cx ; Pushes' Alpha, Beta and Gamma
	pop cx
	pop cx

	; Bye!
	pop ds
	pop dx
	pop cx
	pop bx
	ret


;==============================
PrintOut:
; ds:dx - Text to print ('$' terminated)

	push ax

	; Print an error message to stdout (Int 21/AH=09h)
	; http://www.ctyme.com/intr/rb-2562.htm
	mov ah, 0x09
	int 0x21

	; Bye!
	pop ax
	ret


;==============================
Exit:
; al - Exit status

	; Terminate program (Int 21/AH=4Ch)
	; http://www.ctyme.com/intr/rb-2974.htm
	mov ah, 0x4C
	int 0x21
