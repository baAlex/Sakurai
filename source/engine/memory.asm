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

include "macros.asm"


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

	jz near MemoryClean_16 ; Lucky, no remainder

	; Clean the remainder in steps of 1 byte
	sub cx, ax ; AX = remainder value

MemoryClean_1_loop:
	mov byte [es:di], 0x00
	inc di
	dec ax
	jnz near MemoryClean_1_loop

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


;==============================
CHUNK_HEADER_SIZE = 8
; u8  used
; u8  last
; u16 size
; u16 previous_size
; u16 unused2


;==============================
PoolInit:
; ds:dx - Address to initialize
; cx    - Size

	push si

	sub cx, CHUNK_HEADER_SIZE
	jz near PoolInit_failure ; TODO
	jc near PoolInit_failure ; TODO

	mov si, dx

	; Chunk header
	mov byte [si], 0      ; Header::used
	mov byte [si + 1], 1  ; Header::last
	mov word [si + 2], cx ; Header::size

	; Bye!
	pop si
	ret

PoolInit_failure:
	mov al, EXIT_FAILURE
	call near Exit ; (al)


;==============================
PoolPrint:
; ds:dx - Address

	push ax
	push bx
	push si

	mov si, dx

	push ds
	push dx
		SetDsDx seg_data, str_mem_pool
		call near PrintLogString ; (ds:dx)
	pop dx
	pop ds

PoolPrint_loop:

	; Used/last
	push ds
	push dx
		SetDsDx seg_data, str_flags
		call near PrintLogString ; (ds:dx)
	pop dx
	pop ds

	mov word ax, [si] ; Header::used / Header::last
	mov bx, ax
	call near PrintLogNumber

	; Size
	push ds
	push dx
		SetDsDx seg_data, str_size
		call near PrintLogString ; (ds:dx)
	pop dx
	pop ds

	mov word ax, [si + 2] ; Header::size
	call near PrintLogNumber

	; Chunk is marked as the last one?
	dec bh ; Header::last (bh)
	jz near PoolPrint_bye

	; Nope, advance to the next one
	push ds
	push dx
		SetDsDx seg_data, str_mem_separator
		call near PrintLogString ; (ds:dx)
	pop dx
	pop ds

	add si, CHUNK_HEADER_SIZE
	add si, ax
	jmp near PoolPrint_loop

	; Bye!
PoolPrint_bye:
	pop si
	pop bx
	pop ax
	ret


;==============================
PoolAllocate:
; ds:dx - Pool
; cx    - Size

	push ax
	push bx
	push si

	mov si, dx

	; Iterate chunks finding a free one
	; Yes, inefficient as hell, but is not like
	; a few 64 kb of memory require much more
PoolAllocate_loop:

	; Is used?
	mov word bx, [si] ; Header::used (bl) / Header::last (bh)
	cmp bl, 0x01 ; Header::used (bl)
	jz near PoolAllocate_continue ; Yes, used

	; Has space?
	mov word ax, [si + 2] ; Header::size
	cmp ax, cx
	jz near PoolAllocate_continue_size_set ; No space here
	jc near PoolAllocate_continue_size_set

	; Found, lets subdivide the chunk
	; ax - Chunk size
	; bx - Chunk used (bl) / last (bh)
	; cx - New chunk required size
	; si - Chunk offset
	sub ax, cx
	sub ax, CHUNK_HEADER_SIZE
	jz near PoolAllocate_no_remainder
	jc near PoolAllocate_no_remainder

	mov di, si
	add di, cx ; Advance to the remainder chunk
	add di, CHUNK_HEADER_SIZE

	mov byte [di], 0      ; Remainder Header::used
	mov byte [di + 1], bh ; Remainder Header::last
	mov word [di + 2], ax ; Remainder Header::size
	mov word [di + 4], cx ; Remainder Header::previous_size

	mov byte [si + 1], 0  ; Header::last
	mov word [si + 2], cx ; Header::size

PoolAllocate_no_remainder:
	mov byte [si], 1 ; Header::used

	; Bye!
	add si, CHUNK_HEADER_SIZE
	mov di, si ; Return value
	pop si
	pop bx
	pop ax
	ret

	; Advance to the next one, if any
PoolAllocate_continue:
	mov word ax, [si + 2] ; Header::size
PoolAllocate_continue_size_set:
	dec bh ; Header::last
	jz near PoolAllocate_failure ; Last one :(

	add si, CHUNK_HEADER_SIZE
	add si, ax
	jmp near PoolAllocate_loop

PoolAllocate_failure:
	mov al, EXIT_FAILURE
	call near Exit ; (al)
