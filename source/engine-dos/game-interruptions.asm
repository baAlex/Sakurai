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


include "macros.asm"


;==============================
_IntFDVector:
; http://www.ctyme.com/intr/rb-8735.htm

	push ax
	push dx
	push ds

	mov ax, seg_game_data
	mov ds, ax

	mov ax, [ifd_arg1]

	cmp ax, 0x01
	je near GamePrintString
	cmp ax, 0x02
	je near GamePrintNumber
	cmp ax, 0x03
	je near GameLoadBackground
	cmp ax, 0x04
	je near GameLoadPalette
	cmp ax, 0x05
	je near GameUnloadEverything
	cmp ax, 0x06
	je near GameFlushCommands
	cmp ax, 0x07
	je near GameLoadSprite
	cmp ax, 0x08
	je near GameFreeSprite
	cmp ax, 0x09
	je near GameExitRequest

	; Notify PIC to end this interruption? (TODO)
	; http://stanislavs.org/helppc/8259.html
_IntFDVector_bye:
	mov dx, 0x20
	mov al, 0x20
	out dx, al

	; Bye!
	pop ds
	pop dx
	pop ax
	iret


;==============================
GamePrintString:
	mov dx, [ifd_arg2]
	call near PrintLogString ; (ds:dx)

	jmp near _IntFDVector_bye


;==============================
GamePrintNumber:
	mov ax, [ifd_arg2]
	call near PrintLogNumber ; (ax)

	jmp near _IntFDVector_bye


;==============================
GameLoadBackground:
	push cx

	mov dx, [ifd_arg2]
	call near FileOpen ; (ds:dx)

	SetDsDx seg_bkg_data, 0x0000
	mov cx, BKG_DATA_SIZE
	call near FileRead ; (ax = fp, ds:dx = dest, cx = size)

	; Bye!
	pop cx
	call near FileClose ; (ax)
	jmp near _IntFDVector_bye


;==============================
GameLoadPalette:
	push cx

	mov dx, [ifd_arg2]
	call near FileOpen ; (ds:dx)

	SetDsDx seg_pal_data, 0x0000
	mov cx, PAL_DATA_SIZE
	call near FileRead ; (ax = fp, ds:dx = dest, cx = size)
	call near FileClose ; (ax)

	; Set palette
	; http://stanislavs.org/helppc/ports.html

	mov dx, 0x03C8 ; VGA video DAC PEL address
	mov al, 0x00 ; Color index
	out dx, al

	mov dx, 0x03C9 ; VGA video DAC
	xor bx, bx

GameLoadPalette_loop:
	mov al, [bx]
	out dx, al
	mov al, [bx + 1]
	out dx, al
	mov al, [bx + 2]
	out dx, al

	add bx, 3
	cmp bx, PAL_DATA_SIZE
	jne near GameLoadPalette_loop

	; Bye!
	pop cx
	jmp near _IntFDVector_bye


;==============================
GameLoadSprite:

	;push ax ; Done by the caller
	;push dx ; Done by the caller
	;push ds ; Done by the caller
	push bx
	push cx
	push di
	push si

	mov dx, [ifd_arg2]
	call near FileOpen ; (ds:dx, ax = RETURN)

	; PROTIP: keep an eye on how many times I change segments :/
	; (in any case, this function don't need to be 'fast')
	SetDsDx seg_data, str_spr_load
	call near PrintLogString ; (ds:dx)

	; Read sprite load-header in 'sprite_load_header'
	mov dx, sprite_load_header
	mov cx, SPRITE_LOAD_HEADER_SIZE
	call near FileRead ; (ax = fp, ds:dx = dest, cx = size)

	push ax ; To keep fp

		mov dx, str_size
		call near PrintLogString ; (ds:dx)
		mov ax, [sprite_load_header] ; SpriteLoadHeader::file_size
		mov cx, ax
		call near PrintLogNumber ; (ax)

	pop ax

	; Try to allocate memory in 'seg_pool_a'
	SetDsDx seg_pool_a, pool_a_data
	call near PoolAllocate ; (ds:dx = pool, cx = size, di = RETURN)

	mov si, 1 ; To keep note of PoolA
	cmp di, 0x0000
	jnz GameLoadSprite_allocated

	SetDsDx seg_data, str_pool_a_full
	call near PrintLogString ; (ds:dx)

	; Try to allocate memory in 'seg_pool_b'
	SetDsDx seg_pool_b, pool_b_data
	call near PoolAllocate ; (ds:dx = pool, cx = size, di = RETURN)

	mov si, 2 ; To keep note of PoolB
	cmp di, 0x0000
	jnz GameLoadSprite_allocated

	SetDsDx seg_data, str_pool_b_full
	call near PrintLogString ; (ds:dx)

	; No free memory in any pool
	jmp GameLoadSprite_failure

GameLoadSprite_allocated:

	; Read the rest of the sprite file
	mov dx, di
	call near FileRead ; (ax = fp, ds:dx = dest, cx = size)
	call near FileClose ; (ax)

	; Finally, iterate the sprite indirection table searching
	; for a free slot, and save the sprite pool and offset there
	SetDsDx seg_data, sprite_indirection_table

	mov bx, dx
	add bx, SPRITE_SLOT_SIZE ; Keep the first slot unused, to use it as a NULL value

GameLoadSprite_iterate_table:
	mov ax, [bx] ; Slot::Offset
	cmp ax, 0x0000
	jz GameLoadSprite_slot_found

	; Next step
	add bx, SPRITE_SLOT_SIZE
	cmp bx, (sprite_indirection_table + SPRITE_INDIRECTION_SIZE)
	jb GameLoadSprite_iterate_table

	; No free slot
	jmp GameLoadSprite_failure

GameLoadSprite_slot_found:
	mov word [bx], di ; Slot::Offset
	mov word [bx + 2], si ; Slot::Where (on what pool)

	; Return value
	SetDsDx seg_game_data, 0x0000

	sub bx, sprite_indirection_table
	mov [ifd_arg1], bx ; Return indirection table slot

	; Bye
	pop si
	pop di
	pop cx
	pop bx

	jmp near _IntFDVector_bye

GameLoadSprite_failure:
	; Blow everything
	mov al, EXIT_FAILURE
	call near Exit ; (al)


;==============================
GameFreeSprite:
	jmp near _IntFDVector_bye


;==============================
GameUnloadEverything:
	push cx
	push es
	push di

	mov cx, seg_data
	mov es, cx
	mov di, sprite_indirection_table
	mov cx, SPRITE_INDIRECTION_SIZE
	call MemoryClean ; es:di = Destination, cx = Size

	SetDsDx seg_pool_a, pool_a_data
	mov cx, POOL_A_SIZE
	call PoolInit ; (ds:dx, cx = size)

	SetDsDx seg_pool_b, pool_b_data
	mov cx, POOL_B_SIZE
	call PoolInit ; (ds:dx, cx = size)

	pop di
	pop es
	pop cx
	jmp near _IntFDVector_bye


;==============================
GameFlushCommands:
	push si
	push es

	mov si, seg_game_data
	mov ds, si
	mov si, seg_buffer_data
	mov es, si

	mov si, commands_table

	call near IterateGameCommands ; (ds:si = commands table, es:0x0000 = destination buffer)

	pop es
	pop si

	jmp near _IntFDVector_bye


;==============================
GameExitRequest:

	;push ax ; Done by the caller
	;push dx ; Done by the caller
	;push ds ; Done by the caller

	mov dx, seg_data
	mov ds, dx

	mov word [exit_request], 0x0001

	jmp near _IntFDVector_bye
