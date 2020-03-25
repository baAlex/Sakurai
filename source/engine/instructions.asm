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


; [instructions.asm]
; - Alexander Brandt 2020


; http://www.brackeen.com/vga/basics.html


; Following routines are not functions ('functions' meaning the correct
; care of the fast call convention), rather they are part of a table
; executed in a 'switch' fashion (C lingua)...

; So, the only requirement here is to preserve registers DS and SI, and
; always jump to 'Main_loop_instructions_table_continue'. And with the only
; exception of AX, BX and CX is better to assume that other registers
; just contain trash.


;==============================
DrawBkg: ; CODE_DRAW_BKG
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
	call MemoryCopy ; (ds:si = source, es:di = destination, cx)

	; Bye!
	pop si
	pop ds
	jmp Main_loop_instructions_table_continue


;==============================
DrawPixel: ; CODE_DRAW_PIXEL
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
	jmp Main_loop_instructions_table_continue


;==============================
LoadBkg: ; CODE_LOAD_BKG
; eax - Filename (high 16 bits)
; ebx - Unused

	push ds

	shr eax, 16

	; Open file
	mov bx, seg_game_data
	mov ds, bx
	mov dx, ax
	call FileOpen ; (ds:dx)

	; Read into bkg data
	mov bx, seg_bkg_data
	mov ds, bx
	mov dx, 0x0000
	mov cx, BKG_DATA_SIZE
	call FileRead ; (ax = fp, ds:dx = dest, cx = size)

	; Bye!
	call FileClose ; (ax)

	pop ds
	jmp Main_loop_instructions_table_continue


;==============================
DrawRect: ; CODE_DRAW_RECTANGLE
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
DrawRect_row:
	DrawRect_column:
		mov dword [es:di], eax
		mov dword [es:di + 4], eax
		mov dword [es:di + 8], eax
		mov dword [es:di + 12], eax

		add di, 16
		loop DrawRect_column

	; Preparations for next step
	mov cl, bl ; Width
	add si, 320
	mov di, si

	dec bh ; Height
	jnz DrawRect_row

	; Bye!
	pop si
	jmp Main_loop_instructions_table_continue


;==============================
DrawRectBkg: ; CODE_DRAW_RECTANGLE_BKG
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
DrawRectBkg_row:
	DrawRectBkg_column:
		movsd
		movsd
		movsd
		movsd
		loop DrawRectBkg_column

	; Preparations for next step
	mov cl, al ; Width
	add bx, 320
	mov di, bx
	mov si, bx

	dec ah ; Height
	jnz DrawRectBkg_row

	; Bye!
	pop si
	pop ds
	jmp Main_loop_instructions_table_continue


;==============================
DrawRectPrecise: ; CODE_DRAW_RECTANGLE_PRECISE
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
DrawRectPrecise_row:
	DrawRectPrecise_column:
		mov byte [es:di], ah
		inc di
		loop DrawRectPrecise_column

	; Preparations for next step
	mov cl, bl ; Width
	add si, 320
	mov di, si

	dec bh ; Height
	jnz DrawRectPrecise_row

	; Bye!
	pop si
	jmp Main_loop_instructions_table_continue


;==============================
DrawSprite: ; CODE_DRAW_SPRITE
; eax - Slot (ah), Frame, Mode (high 16 bits)
; ebx - X, Y (low 16 bits)

	push si
	push ds

	mov cx, seg_buffer_data
	mov es, cx

	mov cx, seg_spr_data
	mov ds, cx

	; Calculate offset in DI
	mov di, 0x0000
	add di, bx ; X

	shr ebx, 16
	mov cx, bx
	shl bx, 8
	shl cx, 6
	add di, bx
	add di, cx

	; Draw!
	call seg_spr_data:0x0000

	; Bye!
	pop ds
	pop si
	jmp Main_loop_instructions_table_continue
