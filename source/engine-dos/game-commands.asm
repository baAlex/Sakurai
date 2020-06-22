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


; [game-commands.asm]
; - Alexander Brandt 2020

; http://www.brackeen.com/vga/basics.html



macro OffsetIn reg ; Requires X and Y in EBX, destroys CX
{
	mov reg, bx ; X

	shr ebx, 16 ; Y
	mov cx, bx
	shl bx, 8
	shl cx, 6

	; Combine Y
	add reg, bx
	add reg, cx
}



;==============================
IterateGameCommands:
; ds:si = Commands table
; es:0x0000 = Destination buffer

	; This is one of the two hot-spots of the engine
	push eax
	push ebx

	push di
	push cx
	push fs
	push dx

IterateGameCommands_loop:

	mov eax, [si] ; Code, Color, Width, Height, Filename
	mov ebx, [si + 4] ; X, Y

	cmp al, 0x00 ; CODE_HALT
	je near IterateGameCommands_bye
	dec al ; CODE_DRAW_BKG 0x01
	jz near GameDrawBkg
	dec al ; CODE_DRAW_RECTANGLE_BKG 0x02
	jz near GameDrawRectBkg
	dec al ; CODE_DRAW_SPRITE 0x03
	jz near GameDrawSprite
	dec al ; CODE_DRAW_RECTANGLE 0x04
	jz near GameDrawRect
	dec al ; CODE_DRAW_RECTANGLE_PRECISE 0x05
	jz near GameDrawRectPrecise
	dec al ; CODE_DRAW_TEXT 0x06
	jz near GameDrawText
	dec al ; CODE_DRAW_H_LINE 0x07
	jz near GameDrawHLine
	dec al ; CODE_DRAW_V_LINE 0x08
	jz near GameDrawVLine
	dec al ; CODE_DRAW_PIXEL 0x09
	jz near GameDrawPixel

	; Next command
IterateGameCommands_continue:

	mov byte [si], 0x00 ; Before continue lets clear the slot with CODE_HALT

	add si, COMMAND_SIZE
	cmp si, COMMANDS_TABLE_SIZE
	jb IterateGameCommands_loop

	; Bye!
IterateGameCommands_bye:

	pop dx
	pop fs
	pop cx
	pop di

	pop ebx
	pop eax
	ret


;==============================
GameDrawPixel: ; CODE_DRAW_PIXEL
; eax - Color (ah)
; ebx - X, Y (low 16 bits)

	; Calculate offset in DI
	OffsetIn di ; EBX = x, y, Destroys CX

	; Draw
	mov [es:di], ah

	; Bye!
	jmp near IterateGameCommands_continue


;==============================
GameDrawHLine: ; CODE_DRAW_H_LINE
; eax - Color (ah), Width (first high 8 bits)
; ebx - X, Y (low 16 bits)

	; Calculate offset in DI
	OffsetIn di ; EBX = x, y, Destroys CX

	; Save width in BL
	mov ebx, eax
	shr ebx, 16

	; Repeat color in the entire EAX register
	mov al, ah
	shl eax, 8
	mov al, ah
	shl eax, 8
	mov al, ah

	; Counter for LOOP
	xor ch, ch
	mov cl, bl ; Width

	; Draw loop
	mov bx, 16 ; To avoid an immediate instruction

GameDrawHLine_column:
	mov dword [es:di], eax
	mov dword [es:di + 4], eax
	mov dword [es:di + 8], eax
	mov dword [es:di + 12], eax
	add di, bx
	loop GameDrawHLine_column

	; Bye!
	jmp near IterateGameCommands_continue


;==============================
GameDrawVLine: ; CODE_DRAW_V_LINE
; eax - Color (ah), Height (first high 8 bits)
; ebx - X, Y (low 16 bits)

	; Calculate offset in DI
	OffsetIn di ; EBX = x, y, Destroys CX

	; Save height in AH, color in AL
	shr eax, 8
	shl ah, 3 ; Multiply by 8

	; Counter for LOOP
	xor ch, ch
	mov cl, ah ; Height

	; Draw loop
	mov bx, 640 ; To avoid an immediate instruction

GameDrawVLine_column:
	mov byte [es:di], al
	mov byte [es:di + 320], al
	add di, bx
	loop GameDrawVLine_column

	; Bye!
	jmp near IterateGameCommands_continue


;==============================
GameDrawBkg: ; CODE_DRAW_BKG
; eax - Unused
; ebx - Unused

	push ds
	push si

	; Set segments/data
	mov cx, seg_bkg_data
	mov ds, cx

	xor si, si
	xor di, di

	; Draw
	mov cx, BKG_DATA_SIZE
	call near MemoryCopy ; (ds:si = source, es:di = destination, cx)

	; Bye!
	pop si
	pop ds
	jmp near IterateGameCommands_continue


;==============================
GameDrawRectBkg: ; CODE_DRAW_RECTANGLE_BKG
; eax - Width, Height (high 16 bits)
; ebx - X, Y (low 16 bits)

	push ds
	push si

	; Calculate offset in DI
	OffsetIn di ; EBX = x, y, Destroys CX

	; Multiply height by 16 so it matches with width behavior
	shr eax, 16
	shl ah, 4

	; Set segments/data
	mov cx, seg_bkg_data
	mov ds, cx

	mov si, di
	mov bx, di

	; Counter for LOOP
	xor ch, ch
	mov cl, al ; Width

	; Draw loop
GameDrawRectBkg_row:
	GameDrawRectBkg_column:
		movsd
		movsd
		movsd
		movsd
		loop GameDrawRectBkg_column

	; Preparations for next step
	mov cl, al ; Width
	add bx, 320
	mov di, bx
	mov si, bx

	dec ah ; Height
	jnz near GameDrawRectBkg_row

	; Bye!
	pop si
	pop ds
	jmp near IterateGameCommands_continue


;==============================
GameDrawRect: ; CODE_DRAW_RECTANGLE
; eax - Color (ah), Width, Height (high 16 bits)
; ebx - X, Y (low 16 bits)

	push si

	; Calculate offset in DI
	OffsetIn di ; EBX = x, y, Destroys CX

	; Save width an height in BX
	mov ebx, eax
	shr ebx, 16

	shl bh, 4 ; Multiply height by 16

	; Repeat color in the entire EAX register
	mov al, ah
	shl eax, 8
	mov al, ah
	shl eax, 8
	mov al, ah

	; Counter for LOOP
	xor ch, ch
	mov cl, bl ; Width

	mov si, di

	; Draw loop
GameDrawRect_row:
	GameDrawRect_column:
		mov dword [es:di], eax
		mov dword [es:di + 4], eax
		mov dword [es:di + 8], eax
		mov dword [es:di + 12], eax

		add di, 16
		loop GameDrawRect_column

	; Preparations for next step
	mov cl, bl ; Width
	add si, 320
	mov di, si

	dec bh ; Height
	jnz near GameDrawRect_row

	; Bye!
	pop si
	jmp near IterateGameCommands_continue


;==============================
GameDrawRectPrecise: ; CODE_DRAW_RECTANGLE_PRECISE
; eax - Color (ah), Width, Height (high 16 bits)
; ebx - X, Y (low 16 bits)

	push si

	; Calculate offset in DI
	OffsetIn di ; EBX = x, y, Destroys CX

	; Save width an height in BX
	mov ebx, eax
	shr ebx, 16

	; Counter for LOOP
	xor ch, ch
	mov cl, bl ; Width

	mov si, di

	; Draw loop
GameDrawRectPrecise_row:
	GameDrawRectPrecise_column:
		mov byte [es:di], ah
		inc di
		loop GameDrawRectPrecise_column

	; Preparations for next step
	mov cl, bl ; Width
	add si, 320
	mov di, si

	dec bh ; Height
	jnz near GameDrawRectPrecise_row

	; Bye!
	pop si
	jmp near IterateGameCommands_continue


;==============================
GameDrawSprite: ; CODE_DRAW_SPRITE
; eax - Slot (ah), Frame, Mode (high 16 bits)
; ebx - X, Y (low 16 bits)

	push si
	push ds

	; Calculate offset in DI
	OffsetIn di ; EBX = x, y, Destroys CX

	; Load sprite offset from indirection table, in BX
	mov dx, seg_data
	mov ds, dx

	shr ax, 8 ; Slot (ah)
	mov si, ax
	shl si, 1 ; Multiply by the indirection table entry size (2)

	mov bx, word[spr_indirection_table + si]

	; Read SI and CX from sprite header
	mov dx, seg_pool_a
	mov ds, dx

	mov si, [bx + 2] ; Data offset in header
	mov cx, [bx + 4] ; Frame number in header
	add si, bx

	; Load frame number in AX (currently is on the higher EAX bytes)
	shr eax, 16
	and ax, 0x00FF

	inc cl
	div cl ; Modulo by frames number
	shr ax, 8

	; Read the frame offset table after header using AX, then
	; point BX into the desired frame code
	shl ax, 1 ; Multiply by the frame offsets entry size (2)
	add bx, 6 ; Header size (to skip it)
	add bx, ax
	add bx, [bx]

	; Draw!
	call far seg_pool_a:spr_draw
		; BX = Absolute offset (in the segment) pointing into a frame code
		; DS:SI = Source, specified in the header (also absolute)
		; ES:DI = Destination

	; Bye!
	pop ds
	pop si
	jmp near IterateGameCommands_continue


;==============================
GameDrawText: ; CODE_DRAW_TEXT
; eax - Slot (ah), Text (high 16 bits)
; ebx - X, Y (low 16 bits)

	push si
	push ds

	; Calculate offset in DI
	OffsetIn di ; EBX = x, y, Destroys CX


	push di ; TODO, here the push/pop is an incompressible mess!

	; Load sprite offset from indirection table, in BX
	mov dx, seg_data
	mov ds, dx

	shr ax, 8 ; Slot (ah)
	mov si, ax
	shl si, 1 ; Multiply by the indirection table entry size (2)

	mov bx, word[spr_indirection_table + si]

	; Read SI from sprite header
	mov dx, seg_pool_a
	mov ds, dx

	mov si, [bx + 2] ; Data offset in header
	add si, bx

	; Set segments to use
	mov dx, seg_game_data
	mov fs, dx

	; Load text address in CX (currently is on the higher EAX bytes)
	shr eax, 16
	mov cx, ax

	; Save BX in DX, as in every iteration we need to get
	; back to the beginning of the sprite code (offset)
	mov dx, bx

	; Read first character in AL, frame to draw
	mov bx, cx
	mov al, [fs:bx]
	xor ah, ah

	cmp al, 0x00
	jz GameDrawText_bye

GameDrawText_loop:

	mov bx, dx

	; Read the frame offset table after header using AX, then
	; point BX into the desired frame code
	shl ax, 1 ; Multiply by the frame offsets entry size (2)
	add bx, 6 ; Header size (to skip it)
	add bx, ax
	add bx, [bx]

	push di ; Destroyed by spr_draw()
	push si

	; Draw!
	call far seg_pool_a:spr_draw
		; BX = Absolute offset (in the segment) pointing into a frame code
		; DS:SI = Source, specified in the header (also absolute)
		; ES:DI = Destination

	pop si
	pop di

	; Add spacing to destination
	add di, ax

	; Preparations for next character
GameDrawText_next:
	inc cx

	mov bx, cx
	mov al, [fs:bx]
	xor ah, ah

	; Is a new line?
	cmp al, 0xA
	jnz is_null

	pop di
	add di, 3200 ; New line space (HARDCODED!)
	push di
	jmp GameDrawText_next

	; Is NULL?
is_null:
	cmp al, 0x00
	jnz GameDrawText_loop

GameDrawText_bye:

	; Bye!
	pop di
	pop ds
	pop si
	jmp near IterateGameCommands_continue
