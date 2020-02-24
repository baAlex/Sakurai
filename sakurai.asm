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


; [sakurai.asm]
; - Alexander Brandt 2020

; https://en.wikipedia.org/wiki/Sakurai%2C_Nara
; https://flatassembler.net/docs.php?article=manual#2.4.1
; https://www.tutorialspoint.com/assembly_programming/assembly_conditions.htm
; https://groups.google.com/forum/#!topic/comp.lang.asm.x86/1rfHRzQy9xk

; $ fasm ./sakurai.asm

format MZ
entry seg_code:Main
heap 0

SK_DEGUB = 1

segment seg_code


;==============================
Main:
	; Welcome message
	mov ax, seg_data
	mov ds, ax

	mov cx, (str_separator_end - str_separator)
	mov si, str_separator
	call PrintLog ; (ds:si, cx)

	mov cx, (str_hello_end - str_hello)
	mov si, str_hello
	call PrintLog ; (ds:si, cx)

	; Clean secondary data segments
	mov ax, seg_buffer_data
	mov ds, ax
	mov di, buffer_data
	mov cx, 64000
	call MemoryClean ; (ds:di, cx)

	;mov ax, seg_bkg_data
	;mov ds, ax
	;mov di, bkg_data
	;mov cx, 64000
	;call MemoryClean ; (ds:di, cx)

	mov ax, seg_spr_data
	mov ds, ax
	mov di, spr_data
	mov cx, 65025
	call MemoryClean ; (ds:di, cx)

	; Modules initialization
	call RenderInit

	; Copy bkg as a test
	mov ax, seg_bkg_data
	mov es, ax
	mov si, bkg_data
	mov ax, 0xA000
	mov ds, ax
	mov di, 0x0000
	mov cx, 64000
	call MemoryCopy ; (es:si -src-, ds:di -dest-, cx)

	;;;; GAME LOOP HERE

		; Read from stdin (Int 21/AH=07h)
		; http://www.ctyme.com/intr/rb-2560.htm
		mov ah, 0x07
		int 0x21

	;;;; GAME LOOP HERE

	; Bye!
	call RenderStop

	; Terminate program (Int 21/AH=4Ch)
	; http://www.ctyme.com/intr/rb-2974.htm
	mov ah, 0x4C
	int 0x21


;==============================
RenderInit:

	push ax
	push cx
	push si
	push ds
	push bx ; Interruptors return here

	mov ax, seg_data ; Messages to print
	mov ds, ax

	mov cx, (str_render_init_end - str_render_init)
	mov si, str_render_init
	call PrintLog ; (ds:si, cx)

	; Get current video mode (Int 10/AH=0Fh)
	; http://www.ctyme.com/intr/rb-0108.htm
	mov ah, 0x0F
	int 0x10

	mov [render_previous_mode], al

	; Set VGA mode 13 (Int 10/AH=00h)
	; http://www.ctyme.com/intr/rb-0069.htm
	mov ah, 0x00
	mov al, 0x13
	int 0x10

	; Get current video mode, again, to check (Int 10/AH=0Fh)
	; - Looks like different chips, return different values when
	;   the mode is set (above section). So we need to call this
	;   interruptor again.
	mov ah, 0x0F
	int 0x10

	sub al, 0x13
	jz RenderInit_success

	; Failure :(
	mov si, str_vga_error
	call PrintOut ; (ds:si)

RenderInit_success:

	; Load palette
	; http://stanislavs.org/helppc/ports.html

	mov dx, 0x03C8 ; VGA video DAC PEL address
	mov al, 0x00   ; Color index
	out dx, al

	mov dx, 0x03C9 ; VGA video DAC
	mov bx, 0
	mov ah, 255

RenderInit_palette_loop:
	mov al, [pal_data + bx]
	out dx, al
	inc bx

	mov al, [pal_data + bx]
	out dx, al
	inc bx

	mov al, [pal_data + bx]
	out dx, al
	inc bx

	dec ah
	jnz RenderInit_palette_loop


	pop bx
	pop ds
	pop si
	pop cx
	pop ax
	ret


;==============================
RenderStop:

	push ax
	push ds

	mov ax, seg_data ; To retrieve previous mode
	mov ds, ax

	; Set previous mode (Int 10/AH=00h)
	; http://www.ctyme.com/intr/rb-0069.htm
	mov ah, 0x00
	mov al, [render_previous_mode]
	int 0x10

	pop ds
	pop ax
	ret


;==============================
MemoryCopy:
; https://stackoverflow.com/a/8022107
; ds:di - Destination
; es:si - Source
; cx    - Size

	push eax ; 386 thing
	push ebx

	; Modulo operation
	mov ax, cx
	and ax, 3 ; Works with modulos power of 2

	jz MemoryCopy_4_loop ; Lucky, no remainder

	; Copy the remainder in steps of 1 byte
	sub cx, ax ; ax = remainder value

MemoryCopy_1_loop:
	mov ah, [es:si]  ; Remainder isn't bigger than 4,
	mov [ds:di], ah  ; so lets recycle his high byte
	inc di
	inc si
	dec al
	jnz MemoryCopy_1_loop

	; Copy in steps of 4 bytes

MemoryCopy_4_loop:
	mov ebx, [es: si]
	mov [ds:di], ebx
	add di, 4
	add si, 4
	sub cx, 4
	jnz MemoryCopy_4_loop

	pop ebx
	pop eax
	ret


;==============================
MemoryClean:
; https://stackoverflow.com/a/8022107
; ds:di - Destination
; cx    - Size

	push eax ; 386 thing

	; Modulo operation
	mov ax, cx
	and ax, 3 ; Works with modulos power of 2

	jz MemoryClean_4 ; Lucky, no remainder

	; Set the remainder in steps of 1 byte
	sub cx, ax ; ax = remainder value

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

	pop eax
	ret


;==============================
PrintOut:
; ds:si - Text to print ('$' terminated)

	push ax
	push dx

	; Print an error message to stdout (Int 21/AH=09h)
	; http://www.ctyme.com/intr/rb-2562.htm
	mov ah, 0x09
	mov dx, si
	int 0x21

	pop dx
	pop ax
	ret


;==============================
PrintLog:
; ds:si - Text to print
; cx    - Length

	push ax
	push bx
	push dx
	push cx

	; Open existing file (Int 21/AH=3Dh)
	; http://www.ctyme.com/intr/rb-2779.htm
	mov ah, 0x3D
	mov al, 0x01 ; Write mode
	mov dx, str_log_filename
	int 0x21

	jnc PrintLog_write ; Success

	; Create file (Int 21/AH=3Ch)
	; http://www.ctyme.com/intr/rb-2778.htm
	mov ah, 0x3C
	mov cx, 0x0000 ; Attributes, all bits to zero
	mov dx, str_log_filename
	int 0x21

	jc PrintLog_failure1

PrintLog_write:

	; Seek (Int 21/AH=42h)
	; http://www.ctyme.com/intr/rb-2799.htm
	mov bx, ax ; File handle, both Open() and Create() returns it on AX
	mov ah, 0x42
	mov al, 0x02 ; From EOF
	mov cx, 0x0000 ; Origin (signed) HI
	mov dx, 0x0000 ; Origin (signed) LO
	int 0x21

	jc PrintLog_failure1

	; Write to file (Int 21/AH=40h)
	; http://www.ctyme.com/intr/rb-2791.htm
	mov ah, 0x40
	mov dx, si
	pop cx
	int 0x21 ; File handle still on BX

	jc PrintLog_failure2

	; Close file (Int 21/AH=3Eh)
	; http://www.ctyme.com/intr/rb-2782.htm
	mov ah, 0x3E
	int 0x21 ; File handle still on BX

	pop dx
	pop bx
	pop ax
	ret

PrintLog_failure1:
	pop cx

PrintLog_failure2:

	mov si, str_io_error
	call PrintOut ; (ds:si)

	pop dx
	pop bx
	pop ax
	ret


;==============================
segment seg_data

	render_previous_mode: db 0x00

	str_test1: db "Haaiii!", "$" ; Used as "beep" - "boop"
	str_test2: db "Eehhh?!", "$"

	str_io_error: db "IO error", "$"
	str_vga_error: db "You need a VGA adapter", "$"

	; Log
	str_separator: db 0x0A, "===============================", 0x0A
	str_separator_end:

	str_hello: db "Sakurai v0.1", 0x0A
	str_hello_end:

	str_render_init: db "Initializating render module...", 0x0A
	str_render_init_end:

	str_log_filename: db "sakurai.log", 0x00

	pal_data: file "./assets/palette.dat" ; 768 bytes


;==============================
segment seg_buffer_data

	db "Haaiii!!!", "$" ; Strategically positionated at the begining...
	db "Eehhh?!!!", "$"
	buffer_data: times 64000 db 0x00 ; 64000 = 320x200, less than a x86 segment


;==============================
segment seg_bkg_data

	db "Haaiii!!!!!", "$" ; -of every segment
	db "Eehhh?!!!!!", "$"

	bkg_data: file "./assets/test.dat" ; 64000 bytes


;==============================
segment seg_spr_data

	db "Haaiii!!!!!!!", "$"
	db "Eehhh?!!!!!!!", "$"
	spr_data: times 65025 db 0x00 ; 65025 = 255x255
