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
CHUNK_HEADER_SIZE = 4
; u8  used
; u8  last_one
; u16 size


;==============================
PoolInit:
; ds:dx - Address to initialize
; cx    - Size

	push di

	sub cx, CHUNK_HEADER_SIZE
	jz near PoolInit_failure
	jc near PoolInit_failure ; If carry

	mov di, dx

	; Chunk header
	mov byte [di], 0      ; ChunkHeader::used
	mov byte [di + 1], 1  ; ChunkHeader::last_one
	mov word [di + 2], cx ; ChunkHeader::size

	; Bye!
	pop di
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

	push ds
	push dx
		SetDsDx seg_data, str_flags
		call near PrintLogString ; (ds:dx)
	pop dx
	pop ds

	mov word ax, [si] ; ChunkHeader::used / ChunkHeader::last_one
	mov bh, ah ; Used to stop the loop
	call near PrintLogNumber

	push ds
	push dx
		SetDsDx seg_data, str_size
		call near PrintLogString ; (ds:dx)
	pop dx
	pop ds

	mov word ax, [si + 2] ; ChunkHeader::size
	call near PrintLogNumber

	; Preparations for the next step
	dec bh ; ChunkHeader::last_one
	jz near PoolPrint_bye

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
	mov word bx, [si] ; ChunkHeader::used / ChunkHeader::last_one
	cmp bl, 0x01 ; ChunkHeader::used
	jz near PoolAllocate_continue1 ; Used, try with next chunk

	; Has space?
	mov word ax, [si + 2] ; ChunkHeader::size
	cmp ax, cx
	jz near PoolAllocate_continue2 ; No space here, try with next chunk
	jc near PoolAllocate_continue2

	; Found, lets split the chunk
	; Registers at this point:
	; ax - Chunk size
	; bx - Chunk used (bl) / last (bh)
	; cx - New chunk required size
	; si - Chunk offset
	sub ax, cx
	sub ax, CHUNK_HEADER_SIZE ; Header for the new subdivision
	jz near PoolAllocate_no_remainder
	jc near PoolAllocate_no_remainder

	mov di, si
	add di, cx ; Advance to the remainder chunk
	add di, CHUNK_HEADER_SIZE

	mov byte [di], 0      ; Remainder ChunkHeader::used
	mov byte [di + 1], bh ; Remainder ChunkHeader::last_one
	mov word [di + 2], ax ; Remainder ChunkHeader::size

	mov byte [si + 1], 0  ; ChunkHeader::last_one
	mov word [si + 2], cx ; ChunkHeader::size

PoolAllocate_no_remainder:
	mov byte [si], 1 ; ChunkHeader::used

	; Bye!
	add si, CHUNK_HEADER_SIZE
	mov di, si ; Return value

	pop si
	pop bx
	pop ax
	ret

	; Advance to the next one, if any
PoolAllocate_continue1:
	mov word ax, [si + 2] ; ChunkHeader::size
PoolAllocate_continue2:
	dec bh ; ChunkHeader::last_one
	jz near PoolAllocate_failure ; :(

	add si, CHUNK_HEADER_SIZE
	add si, ax
	jmp near PoolAllocate_loop

PoolAllocate_failure:
	mov di, 0x0000 ; Return value

	pop si
	pop bx
	pop ax
	ret
