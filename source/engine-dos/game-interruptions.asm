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


include "macros.asm"

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
GameLoadSprite:
	push bx
	push cx
	push si

	mov dx, [ifd_arg2]
	call near FileOpen ; (ds:dx, ax = RETURN)

	mov si, [ifd_arg3]
	; (TODO: an PoolFree() here, some day :) )

	; From here everything happens on 'seg_data'
	SetDsDx seg_data, str_spr_load
	call near PrintLogString ; (ds:dx)

	; Read sprite header in 'spr_header'
	mov dx, spr_header
	mov cx, 4 ; TODO, use a define (SPRITE_HEADER_SIZE)
	call near FileRead ; (ax = fp, ds:dx = dest, cx = size)

	push ax ; To keep the fp

		mov dx, str_size
		call near PrintLogString ; (ds:dx)
		mov ax, [spr_header] ; SprHeader::file_size
		mov cx, ax
		call near PrintLogNumber ; (ax)

		mov dx, str_offset
		call near PrintLogString ; (ds:dx)
		mov ax, [spr_header + 2] ; SprHeader::data_offset
		mov bx, ax
		call near PrintLogNumber ; (ax)

	pop ax

	; Allocate memory accordingly
	; And from here welcome to to 'seg_pool_a'
	SetDsDx seg_pool_a, pool_a_data
	call near PoolAllocate ; (ds:dx = pool, cx = size, di = RETURN)

	; Fill the already read sprite header
	mov [di], cx     ; SprHeader::file_size
	mov [di + 2], bx ; SprHeader::data_offset

	; Read the rest of the sprite file
	mov dx, di

	add dx, 4 ; TODO: SPRITE_HEADER_SIZE
	sub cx, 4 ; TODO: SPRITE_HEADER_SIZE

	call near FileRead ; (ax = fp, ds:dx = dest, cx = size)

	; Lazily lets write in the indirection table (TODO)
	; ... once again in 'seg_data'!!!
	mov dx, seg_data
	mov ds, dx

	shl si, 1 ; Multiply by the indirection table entry size (2)
	mov word [spr_indirection_table + si], di

	; Bye!
	pop si
	pop cx
	pop bx

	call near FileClose ; (ax)
	jmp near _IntFDVector_bye


;==============================
GameUnloadEverything:
	push cx

	SetDsDx seg_pool_a, pool_a_data
	mov cx, POOL_A_SIZE
	call PoolInit ; (ds:dx, cx = size)

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

