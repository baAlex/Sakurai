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


; [io.asm]
; - Alexander Brandt 2020


;==============================
PrintLogString:
; ds:dx - String to print

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

	jc PrintLogString_failure

PrintLogString_file_exists:

	; Seek (Int 21/AH=42h)
	; http://www.ctyme.com/intr/rb-2799.htm
	mov bx, ax ; File handle, both Open() and Create() returns it on AX
	mov ah, 0x42
	mov al, 0x02 ; From EOF
	mov cx, 0x0000 ; Origin (signed) HI
	mov dx, 0x0000 ; Origin (signed) LO
	int 0x21

	jc PrintLogString_failure

	; Calculate length in CX
	pop ds
	pop dx
	push bx ; File handler is here and we need the register

	mov cx, 0
	mov bx, dx

	mov al, [bx]
	cmp al, 0x00
	jz PrintLogString_write

PrintLog_len:
	inc cx
	inc bx

	mov al, [bx]
	cmp al, 0x00
	jnz PrintLog_len

	; Write to file (Int 21/AH=40h)
	; http://www.ctyme.com/intr/rb-2791.htm
PrintLogString_write:

	mov ah, 0x40
	pop bx
	int 0x21

	jc PrintLogString_failure

	; Close file (Int 21/AH=3Eh)
	; http://www.ctyme.com/intr/rb-2782.htm
	mov ah, 0x3E
	int 0x21 ; File handle still on BX

	; Bye!
	pop cx
	pop bx
	pop ax
	ret

PrintLogString_failure:
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
FileOpen:
; ds:dx - Filename

	push cx ; Following interrupt uses it

	; Open existing file (Int 21/AH=3Dh)
	; http://www.ctyme.com/intr/rb-2779.htm
	mov ah, 0x3D
	mov al, 0x00 ; Read mode
	int 0x21

	jc FileOpen_failure

	; Bye!
	pop cx
	ret

FileOpen_failure:

	; Print error in the log
	push ds
	push dx
	push bx

		mov bx, ax ; Open() error code

		mov ax, seg_data
		mov ds, ax
		mov dx, str_file_open_error

		call PrintLogString ; (ds:dx)

		mov ax, bx
		call PrintLogNumber ; (ax)

	pop bx
	pop dx
	pop ds

	pop cx
	mov ax, 0x0000
	ret


;==============================
FileClose:
; ax - File handler

	cmp ax, 0x0000
	jz FileClose_invalid

	push bx
	mov bx, ax

	; Close file (Int 21/AH=3Eh)
	; http://www.ctyme.com/intr/rb-2782.htm
	mov ah, 0x3E
	int 0x21

	; Bye!
	pop bx
	ret

FileClose_invalid:
	ret


;==============================
FileRead:
; ax    - File handler
; ds:dx - Destination
; cx    - Size

	cmp ax, 0x0000
	jz FileRead_invalid

	push bx
	push ax

	mov bx, ax

	; Read From File or Device (Int 21/AH=3Fh)
	; http://www.ctyme.com/intr/rb-2783.htm
	mov ah, 0x3F
	int 0x21

	; Bye!
	pop ax
	pop bx
	ret

FileRead_invalid:
	ret


;==============================
Exit:
; al - Exit status

	; Terminate program (Int 21/AH=4Ch)
	; http://www.ctyme.com/intr/rb-2974.htm
	mov ah, 0x4C
	int 0x21
