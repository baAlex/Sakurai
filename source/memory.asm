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


; [memory.asm]
; - Alexander Brandt 2020


;==============================
MemoryCopy:
; https://stackoverflow.com/a/8022107
; es:di - Destination
; ds:si - Source
; cx    - Size

	push ax

	; Modulo operation
	mov ax, cx
	and ax, 15 ; Works with modulos power of 2

	jz near MemoryCopy_16 ; Lucky, no remainder

	; Copy the remainder in steps of 1 byte
	sub cx, ax ; AX = remainder value

MemoryCopy_1_loop:
	movsb
	dec ax
	jnz near MemoryCopy_1_loop

MemoryCopy_16:

	; Copy in steps of 16 bytes
	shr cx, 4 ; Because LOOP decrements by 1

MemoryCopy_16_loop:
	movsd
	movsd
	movsd
	movsd
	loop MemoryCopy_16_loop

	; Bye!
	pop ax
	ret


;==============================
MemoryClean:
; https://stackoverflow.com/a/8022107
; es:di - Destination
; cx    - Size

	push eax
	push di
	push cx

	; Modulo operation
	mov ax, cx
	and ax, 15 ; Works with modulos power of 2

	jz MemoryClean_16 ; Lucky, no remainder

	; Clean the remainder in steps of 1 byte
	sub cx, ax ; AX = remainder value

MemoryClean_1_loop:
	mov byte [es:di], 0x00
	inc di
	dec ax
	jnz MemoryClean_1_loop

MemoryClean_16:

	; Clean in steps of 16 bytes
	shr cx, 4 ; Because LOOP decrements by 1
	mov eax, 0x00000000

MemoryClean_16_loop:
	mov dword [es:di], eax
	mov dword [es:di + 4], eax
	mov dword [es:di + 8], eax
	mov dword [es:di + 12], eax

	add di, 16
	loop MemoryClean_16_loop

	; Bye!
	pop cx
	pop di
	pop eax
	ret
