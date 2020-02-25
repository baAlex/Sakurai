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


BUFFER_DATA_SIZE = 64000 ; Bytes
BKG_DATA_SIZE = 64000    ; " (TODO, using a file)
SPR_DATA_SIZE = 65025    ; "

PALETTE_LEN = 255  ; Colors (in a file)
PALETTE_SIZE = 768 ; Bytes (in a file)

VGA_SEGMENT = 0xA000
VGA_OFFSET = 0x0000

EXIT_SUCCESS = 0x00
EXIT_FAILURE = 0x01

UNIX_NL = 0x0A ; The log messages uses it


segment seg_code


;==============================
Main:
	; Welcome message
	mov ax, seg_data
	mov ds, ax

	mov cx, (str_separator_end - str_separator)
	mov dx, str_separator
	call PrintLogString ; (ds:dx, cx)

	mov cx, (str_hello_end - str_hello)
	mov dx, str_hello
	call PrintLogString ; (ds:dx, cx)

	; Clean secondary data segments
	; mov ax, seg_buffer_data
	; mov ds, ax
	; mov di, buffer_data
	; mov cx, BUFFER_DATA_SIZE
	; call MemoryClean ; (ds:di, cx)

	; mov ax, seg_bkg_data
	; mov ds, ax
	; mov di, bkg_data
	; mov cx, BKG_DATA_SIZE
	; call MemoryClean ; (ds:di, cx)

	; mov ax, seg_spr_data
	; mov ds, ax
	; mov di, spr_data
	; mov cx, SPR_DATA_SIZE
	; call MemoryClean ; (ds:di, cx)

	; Modules initialization
	call TimerInit
	call RenderInit

	; Copy bkg as a test, mesuring it
	call TimerGet ; (ax = return, ds implicit)
	mov bx, ax    ; Start time

		mov ax, seg_bkg_data
		mov es, ax
		mov si, bkg_data

		mov ax, VGA_SEGMENT
		mov ds, ax
		mov di, VGA_OFFSET

		mov cx, BKG_DATA_SIZE
		call MemoryCopy ; (es:si = source, ds:di = destination, cx)

	mov ax, seg_data ; TimerGet() requires it
	mov ds, ax

	call TimerGet ; (ax = return, ds implicit)
	sub ax, bx
	call PrintLogNumber ; (ax)

	; Main loop
	mov ax, seg_data ; From here no call should change this (TODO)
	mov ds, ax

	mov cx, (str_main_loop_end - str_main_loop)
	mov dx, str_main_loop
	call PrintLogString ; (ds:dx, cx)

Main_loop:
	;;;; MAIN LOOP HERE

		mov ax, 3000    ; Three seconds
		call TimerSleep ; (ax, ds implicit)

		; Read from stdin (Int 21/AH=07h)
		; http://www.ctyme.com/intr/rb-2560.htm
		; mov ah, 0x07
		; int 0x21

	;;;; MAIN LOOP HERE

	; Bye!
	call RenderStop
	call TimerStop

	mov al, EXIT_SUCCESS
	call Exit ; (al)


;==============================
TimerInit:
; http://retired.beyondlogic.org/interrupts/interupt.htm#2
; http://www.osdever.net/bkerndev/Docs/pit.htm
; http://www.intel-assembler.it/portale/5/Programming-the-Intel-8253-8354-pit/howto-program-intel-pit-8253-8254.asp

	push ax
	push bx
	push cx
	push dx
	push ds
	push es ; TODO, pusha?

	mov ax, seg_data ; The messages and previous vector lives here
	mov ds, ax

	mov cx, (str_timer_init_end - str_timer_init)
	mov dx, str_timer_init
	call PrintLogString ; (ds:dx, cx)

	; Get current vector (Int 21/AH=35h)
	; http://www.ctyme.com/intr/rb-2740.htm
	mov ah, 0x35
	mov al, 0x08 ; Interrupt number
	int 0x21

	mov [timer_previous_vector_sector], es
	mov [timer_previous_vector_offset], bx

	; Set new vector (Int 21/AH=25h)
	; http://www.ctyme.com/intr/rb-2602.htm
	mov ax, seg_code
	mov ds, ax
	mov dx, _TimerVector

	mov al, 0x08 ; Interrupt number
	mov ah, 0x25
	int 0x21

	; Set mode
	; https://en.wikipedia.org/wiki/Intel_8253
	; http://stanislavs.org/helppc/ports.html
	; http://stanislavs.org/helppc/8253.html
	mov dx, 0x43 ; Mode Control Register
	mov al, 00110110b ; Mode
	        ; bits 7-6: '00' = Counter 0
	        ; bits 5-4: '11' = Write low byte then high byte
	        ; bits 3-1: '011' = Square wave generator
	        ;                   (the clock reset itself without cpu indication)
	        ; bit 0: '0' = Binary counter
	out dx, al

	; Set frequency
	mov dx, 0x40 ; Counter 0
	mov ax, 1193 ; PIT frequency / 1000... an aproximation
	out dx, al
	mov al, ah
	out dx, al

	; Bye!
	pop es
	pop ds
	pop dx
	pop cx
	pop bx
	pop ax
	ret


;==============================
_TimerVector:

	push ax
	push dx
	push ds

	mov ax, seg_data ; The milisecond counter lives here
	mov ds, ax

	inc word [timer_miliseconds]

	; Notify PIC to end this interruption? (TODO)
	; http://stanislavs.org/helppc/8259.html
	mov dx, 0x20
	mov al, 0x20
	out dx, al

	; Bye!
	pop ds
	pop dx
	pop ax
	iret


;==============================
TimerStop:
	push ax
	push dx
	push cx
	push ds

	mov ax, seg_data ; To retrieve previous vector
	mov ds, ax

	mov cx, (str_timer_stop_end - str_timer_stop)
	mov dx, str_timer_stop
	call PrintLogString ; (ds:dx, cx)

	; Restore previous vector (Int 21/AH=25h)
	; http://www.ctyme.com/intr/rb-2602.htm
	mov dx, [timer_previous_vector_offset] ; Protip: next one changes ds
	mov ax, [timer_previous_vector_sector]
	mov ds, ax
	mov al, 0x08 ; Interrupt number
	mov ah, 0x25
	int 0x21

	; Restore default mode, frequency (18.2)
	mov dx, 0x43      ; Mode Control Register
	mov al, 00110110b ; This is the default mode? (FIXME)
	out dx, al

	mov dx, 0x40 ; Counter 0
	mov ax, 0    ; Default is 'no divisor'
	out dx, al
	mov al, ah
	out dx, al

	; Bye!
	pop ds
	pop cx
	pop dx
	pop ax
	ret


;==============================
TimerSleep:
; ax - Miliseconds to sleep
; ds - Implicit

	add ax, [timer_miliseconds]

TimerSleep_loop:
	nop
	nop
	nop
	nop

	hlt ; Sleep until the PIC wakeup us (the CPU)
	    ; This should happend at new keyboard input or
	    ; by the timer counter

	cmp [timer_miliseconds], ax
	jb TimerSleep_loop ; Jump if Below

	; Bye!
	ret


;==============================
TimerGet:
; ax - Returns miliseconds since initialization
; ds - Implicit

	mov ax, [timer_miliseconds]
	ret


;==============================
RenderInit:
	push ax
	push bx
	push cx
	push ds

	mov ax, seg_data ; The messages, previous mode and palette lives here
	mov ds, ax

	mov cx, (str_render_init_end - str_render_init)
	mov dx, str_render_init
	call PrintLogString ; (ds:dx, cx)

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
	jnz RenderInit_failure

	; Load palette
	; http://stanislavs.org/helppc/ports.html

	mov dx, 0x03C8 ; VGA video DAC PEL address
	mov al, 0x00   ; Color index
	out dx, al

	mov ah, PALETTE_LEN
	mov bx, 0      ; Indexing purposes
	mov dx, 0x03C9 ; VGA video DAC

RenderInit_palette_loop:
	mov al, [render_palette_data + bx]
	out dx, al
	inc bx

	mov al, [render_palette_data + bx]
	out dx, al
	inc bx

	mov al, [render_palette_data + bx]
	out dx, al
	inc bx

	dec ah
	jnz RenderInit_palette_loop

	; Bye!
	pop ds
	pop cx
	pop bx
	pop ax
	ret

RenderInit_failure:
	mov dx, str_vga_error
	call PrintOut ; (ds:dx)

	mov al, EXIT_FAILURE
	call Exit ; (al)


;==============================
RenderStop:
	push ax
	push ds

	mov ax, seg_data ; To retrieve previous mode
	mov ds, ax

	mov cx, (str_render_stop_end - str_render_stop)
	mov dx, str_render_stop
	call PrintLogString ; (ds:dx, cx)

	; Set previous mode (Int 10/AH=00h)
	; http://www.ctyme.com/intr/rb-0069.htm
	mov ah, 0x00
	mov al, [render_previous_mode]
	int 0x10

	; Bye!
	pop ds
	pop ax
	ret


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

	; Bye!
	pop eax
	ret


;==============================
PrintLogString:
; ds:dx - String to print
; cx    - Length

	push ax
	push bx
	push cx
	push dx
	push ds

	mov ax, seg_data ; Log filename lives here
	mov ds, ax

	; Open existing file (Int 21/AH=3Dh)
	; http://www.ctyme.com/intr/rb-2779.htm
	mov ah, 0x3D
	mov al, 0x01 ; Write mode
	mov dx, str_log_filename
	int 0x21

	jnc PrintLogString_file_exists

	; Create file (Int 21/AH=3Ch)
	; http://www.ctyme.com/intr/rb-2778.htm
	mov ah, 0x3C
	mov cx, 0x0000 ; Attributes, all bits to zero
	mov dx, str_log_filename
	int 0x21

	jc PrintLogString_failure1

PrintLogString_file_exists:

	; Seek (Int 21/AH=42h)
	; http://www.ctyme.com/intr/rb-2799.htm
	mov bx, ax ; File handle, both Open() and Create() returns it on AX
	mov ah, 0x42
	mov al, 0x02 ; From EOF
	mov cx, 0x0000 ; Origin (signed) HI
	mov dx, 0x0000 ; Origin (signed) LO
	int 0x21

	jc PrintLogString_failure1

	; Write to file (Int 21/AH=40h)
	; http://www.ctyme.com/intr/rb-2791.htm
	mov ah, 0x40
	pop ds
	pop dx
	pop cx
	int 0x21 ; File handle still on BX

	jc PrintLogString_failure2

	; Close file (Int 21/AH=3Eh)
	; http://www.ctyme.com/intr/rb-2782.htm
	mov ah, 0x3E
	int 0x21 ; File handle still on BX

	; Bye!
	pop bx
	pop ax
	ret

PrintLogString_failure1:
	pop ds
	pop dx
	pop cx

PrintLogString_failure2:
	mov al, EXIT_FAILURE
	call Exit ; (al)


;==============================
PrintLogNumber:
; ax - Number to print

	push bx
	push cx
	push dx
	push ds

	mov bx, seg_data ; HEX table
	mov ds, bx

	; Create the string termination
	mov cx, 0x000A ; Unix NL + NULL
	push cx        ; Push A

	; Create the number string
	mov bh, 0x00

	mov bl, al ; Bits 0-3
	and bl, 00001111b
	mov ch, [hex_table + bx]

	mov bl, al ; Bits 4-7
	shr bl, 4
	mov cl, [hex_table + bx]

		push cx ; Push B

	mov bl, ah ; Bits 8-11
	and bl, 00001111b
	mov ch, [hex_table + bx]

	mov bl, ah ; Bits 12-15
	shr bl, 4
	mov cl, [hex_table + bx]

		push cx ; Push C

	; Point DS to SS, and DX to SP
	mov ax, ss
	mov ds, ax
	mov dx, sp

	; Print it
	mov cx, 5 ; Length not counting NULL
	call PrintLogString

	pop cx ; Pushes' A, B and C
	pop cx
	pop cx

	; Bye!
	pop ds
	pop dx
	pop cx
	pop bx
	ret


;==============================
PrintOut:
; ds:dx - Text to print ('$' terminated)

	push ax

	; Print an error message to stdout (Int 21/AH=09h)
	; http://www.ctyme.com/intr/rb-2562.htm
	mov ah, 0x09
	int 0x21

	; Bye!
	pop ax
	ret


;==============================
Exit:
; al - Exit status

	; Terminate program (Int 21/AH=4Ch)
	; http://www.ctyme.com/intr/rb-2974.htm
	mov ah, 0x4C
	int 0x21


;==============================
segment seg_data

	str_test1: db "Haaiii!", "$" ; Used as "beep" - "boop"
	str_test2: db "Eehhh?!", "$"

	hex_table: db "0", "1", "2", "3", "4", "5", "6", "7"
	           db "8", "9", "A", "B", "C", "D", "E", "F"

	; Timer
	timer_previous_vector_sector: dw 0x0000
	timer_previous_vector_offset: dw 0x0000
	timer_miliseconds: dw 0x0000

	; Render
	render_palette_data: file "./assets/palette.dat"
	render_previous_mode: db 0x00

	; Console
	str_vga_error: db "You need a VGA adapter", "$"

	; Log
	str_separator: db UNIX_NL, "===============================", UNIX_NL
	str_separator_end:

	str_hello: db "Sakurai v0.1", UNIX_NL
	str_hello_end:

	str_render_init: db "Initializating render module...", UNIX_NL
	str_render_init_end:

	str_render_stop: db "Stopping render module...", UNIX_NL
	str_render_stop_end:

	str_main_loop: db UNIX_NL, "Entering main loop...", UNIX_NL, UNIX_NL
	str_main_loop_end:

	str_timer_init: db "Initializating timer module...", UNIX_NL
	str_timer_init_end:

	str_timer_stop: db "Stopping timer module...", UNIX_NL
	str_timer_stop_end:

	str_log_filename: db "sakurai.log", 0x00


;==============================
segment seg_buffer_data

	db "Haaiii!!!", "$" ; Strategically positionated at the begining...
	db "Eehhh?!!!", "$"
	buffer_data: times BUFFER_DATA_SIZE db 0x00


;==============================
segment seg_bkg_data

	db "Haaiii!!!!!", "$" ; -of every segment
	db "Eehhh?!!!!!", "$"
	;bkg_data: times BKG_DATA_SIZE db 0x00
	bkg_data: file "./assets/test.dat"


;==============================
segment seg_spr_data

	db "Haaiii!!!!!!!", "$"
	db "Eehhh?!!!!!!!", "$"
	spr_data: times SPR_DATA_SIZE db 0x00
