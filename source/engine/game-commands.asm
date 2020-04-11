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


; Following routines are not functions ('functions' meaning the correct
; care of the fast call convention), rather they are part of a table
; executed in a 'switch' fashion (C lingua)...

; So, the only requirement here is to preserve registers DS and SI, and
; always jump to 'Main_loop_commands_table_continue'. And with the only
; exception of AX, BX and CX is better to assume that other registers
; just contain trash.


;==============================
GameDrawBkg: ; CODE_DRAW_BKG
; eax - Unused
; ebx - Unused

	push ds
	push si

	mov ax, seg_bkg_data
	mov ds, ax
	mov si, 0x0000

	mov ax, seg_buffer_data
	mov es, ax
	mov di, 0x0000

	mov cx, BKG_DATA_SIZE
	call near MemoryCopy ; (ds:si = source, es:di = destination, cx)

	; Bye!
	pop si
	pop ds
	jmp near Main_loop_commands_table_continue


;==============================
GameDrawPixel: ; CODE_DRAW_PIXEL
; eax - Color (ah)
; ebx - X, Y (low 16 bits)

	; Calculate offset in DI
	mov di, 0x0000
	add di, bx ; X

	shr ebx, 16
	mov cx, bx
	shl bx, 8
	shl cx, 6
	add di, bx
	add di, cx

	; Draw it!
	mov cx, seg_buffer_data
	mov es, cx
	mov [es:di], ah

	; Bye!
	jmp near Main_loop_commands_table_continue


;==============================
GameDrawRect: ; CODE_DRAW_RECTANGLE
; eax - Color (ah), Width, Height (high 16 bits)
; ebx - X, Y (low 16 bits)

	push si

	; Calculate offset in DI
	mov di, 0x0000
	add di, bx ; X

	shr ebx, 16
	mov cx, bx
	shl bx, 8
	shl cx, 6
	add di, bx
	add di, cx

	; Save width an height in BX
	mov ebx, eax
	shr ebx, 16

	shl bh, 4 ; Multiply height by 16 so it matches with width behavior

	; Save color in EAX
	mov al, ah
	shl eax, 8
	mov al, ah
	shl eax, 8
	mov al, ah

	; Set segment
	mov cx, seg_buffer_data
	mov es, cx

	; Counter for LOOP
	mov ch, 0x00
	mov cl, bl ; Width

	mov si, di

	; Draw loop
	; (a call to some similar to MemorySet(), requires an
	; unnecessary amount of pops', pushes' and other calculations)
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
	jmp near Main_loop_commands_table_continue


;==============================
GameDrawRectBkg: ; CODE_DRAW_RECTANGLE_BKG
; eax - Width, Height (high 16 bits)
; ebx - X, Y (low 16 bits)

	push ds
	push si

	; Calculate offset in DI
	mov di, 0x0000
	add di, bx ; X

	shr ebx, 16
	mov cx, bx
	shl bx, 8
	shl cx, 6
	add di, bx
	add di, cx

	; Multiply height by 16 so it matches with width behavior
	shr eax, 16
	shl ah, 4

	; Set segments
	mov cx, seg_buffer_data
	mov es, cx

	mov cx, seg_bkg_data
	mov ds, cx

	mov si, di

	; Counter for LOOP
	mov ch, 0x00
	mov cl, al ; Width

	mov bx, di ; BX is unused at this point

	; Draw loop
	; (a call to some similar to MemorySet(), requires an
	; unnecessary amount of pops', pushes' and other calculations)
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
	jmp near Main_loop_commands_table_continue


;==============================
GameDrawRectPrecise: ; CODE_DRAW_RECTANGLE_PRECISE
; eax - Color (ah), Width, Height (high 16 bits)
; ebx - X, Y (low 16 bits)

	push si

	; Calculate offset in DI
	mov di, 0x0000
	add di, bx ; X

	shr ebx, 16
	mov cx, bx
	shl bx, 8
	shl cx, 6
	add di, bx
	add di, cx

	; Save width an height in BX
	mov ebx, eax
	shr ebx, 16

	; Set segment
	mov cx, seg_buffer_data
	mov es, cx

	; Counter for LOOP
	mov ch, 0x00
	mov cl, bl ; Width

	mov si, di

	; Draw loop
	; (a call to some similar to MemorySet(), requires an
	; unnecessary amount of pops', pushes' and other calculations)
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
	jmp near Main_loop_commands_table_continue


;==============================
GameDrawSprite: ; CODE_DRAW_SPRITE
; eax - Slot (ah), Frame, Mode (high 16 bits)
; ebx - X, Y (low 16 bits)

	push si
	push ds

	; Calculate offset in DI
	mov di, 0x0000
	add di, bx ; X

	shr ebx, 16
	mov cx, bx
	shl bx, 8
	shl cx, 6
	add di, bx
	add di, cx

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

 	div cl ; Modulo by frames number
 	shr ax, 8

	; Read the frame offset table after header using AX, then 
	; point BX into the desired frame code
	shl ax, 1 ; Multiply by the frame offsets entry size (2)
	add bx, 6 ; Header size (to skip it)
	add bx, ax
	add bx, [bx]

	; Draw!
	mov dx, seg_buffer_data
	mov es, dx

	call far seg_pool_a:spr_draw
		; BX = Absolute offset (in the segment) pointing into a frame code 
		; DS:SI = Source, specified in the header (also absolute)
		; ES:DI = Destination

	; Bye!
	pop ds
	pop si
	jmp near Main_loop_commands_table_continue
