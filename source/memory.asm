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
; ds:di - Destination
; es:si - Source
; cx    - Size

	push eax
	push ebx

	; Modulo operation
	mov ax, cx
	and ax, 15 ; Works with modulos power of 2

	jz near MemoryCopy_16 ; Lucky, no remainder

	; Copy the remainder in steps of 1 byte
	sub cx, ax ; AX = remainder value

MemoryCopy_1_loop:
	mov ah, [es:si] ; Remainder isn't bigger than 16,
	mov [ds:di], ah ; so lets recycle his high byte
	inc di
	inc si
	dec al
	jnz near MemoryCopy_1_loop

MemoryCopy_16:

	; Copy in steps of 16 bytes
	shr cx, 4 ; Because LOOP decrements by 1

MemoryCopy_16_loop:
	mov ebx, [es:si]
	mov [ds:di], ebx

	mov ebx, [es:si + 4]
	mov [ds:di + 4], ebx

	mov ebx, [es:si + 8]
	mov [ds:di + 8], ebx

	mov ebx, [es:si + 12]
	mov [ds:di + 12], ebx

	add di, 16
	add si, 16
	loop MemoryCopy_16_loop

	; Bye!
	pop ebx
	pop eax
	ret


;==============================
MemoryClean:
; https://stackoverflow.com/a/8022107
; ds:di - Destination
; cx    - Size

	push eax

	; Modulo operation
	mov ax, cx
	and ax, 3 ; Works with modulos power of 2

	jz MemoryClean_4 ; Lucky, no remainder

	; Set the remainder in steps of 1 byte
	sub cx, ax ; AX = remainder value

MemoryClean_1_loop:
	mov byte [di], 0x00
	inc di
	dec al
	jnz MemoryClean_1_loop

MemoryClean_4:

	; Set in steps of 4 bytes
	mov eax, 0x00000000

MemoryClean_4_loop:
	mov [di], eax
	add di, 4
	sub cx, 4
	jnz MemoryClean_4_loop

	; Bye!
	pop eax
	ret
