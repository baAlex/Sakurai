
; [sakurai.inc]
; - Alexander Brandt 2020


BUFFER_DATA_SIZE = 64000 ; Bytes
BKG_DATA_SIZE = 64000    ; "
SPR_DATA_SIZE = 65025    ; "

PALETTE_LEN = 255  ; Colors
PALETTE_SIZE = 768 ; Bytes (TODO: load at runtime)

VGA_SEGMENT = 0xA000
VGA_OFFSET = 0x0000

EXIT_SUCCESS = 0x00
EXIT_FAILURE = 0x01

UNIX_NL = 0x0A ; Log messages uses it

INPUT_STATE_LEN = 84 ; Number of keys, modify this value
                     ; requires changes on the code

INSTRUCTIONS_TABLE_LEN = 16
INSTRUCTIONS_TABLE_SIZE = 128 ; In bytes


;==============================
segment seg_data

	str_test1: db "Haaiii!", "$" ; Used as "beep/boop"
	str_test2: db "Eehhh?!", "$"

	hex_table: db "0", "1", "2", "3", "4", "5", "6", "7"
	           db "8", "9", "A", "B", "C", "D", "E", "F"

	; Console
	str_vga_error: db "You need a VGA adapter", "$"

	; Log
	str_separator: db UNIX_NL, "===============================", UNIX_NL
	str_separator_end:

	str_hello: db "Sakurai v0.1", UNIX_NL
	str_hello_end:

	str_time_init: db "Initializing time module...", UNIX_NL
	str_time_init_end:

	str_input_init: db "Initializing input module...", UNIX_NL
	str_input_init_end:

	str_render_init: db "Initializing render module...", UNIX_NL
	str_render_init_end:

	str_copy_speed: db "Copy speed is (ms): "
	str_copy_speed_end:

	str_main_loop: db UNIX_NL, "Entering main loop...", UNIX_NL, UNIX_NL
	str_main_loop_end:

	str_render_stop: db "Stopping render module...", UNIX_NL
	str_render_stop_end:

	str_input_stop: db "Stopping input module...", UNIX_NL
	str_input_stop_end:

	str_time_stop: db "Stopping time module...", UNIX_NL
	str_time_stop_end:

	str_segment: db " - Vector segment: "
	str_segment_end:

	str_offset: db " - Vector offset: "
	str_offset_end:

	str_file_open_error: db "FileOpen() error, code: "
	str_file_open_error_end:

	str_scream: db "Iyadaaa!!!", UNIX_NL ; TODO: yabai?
	str_scream_end:

	str_log_filename: db "sakurai.log", 0x00

	; Time
	time_previous_vector_sector: dw 0x0000
	time_previous_vector_offset: dw 0x0000
	time_milliseconds: dw 0x0000

	; Input
	input_previous_vector_sector: dw 0x0000
	input_previous_vector_offset: dw 0x0000

	input_state: times INPUT_STATE_LEN db 0x00

	; Render
	render_palette_data: file "./source/palette.dat"
	render_previous_mode: db 0x00


;==============================
segment seg_game_data
; https://en.wikipedia.org/wiki/Program_Segment_Prefix

	instructions_table: times INSTRUCTIONS_TABLE_SIZE db 0xFF
	blank_psp: times 128 db 0xBF
	game_data: file "./game.com"

GameFrame:
	push ax
	push bx
	push cx
	push ds

	mov ax, seg_game_data
	mov ds, ax

	call game_data ; Near call

	pop ds
	pop cx
	pop bx
	pop ax
	retf ; Far return (CPU sets CS accordingly)


;==============================
segment seg_buffer_data
	buffer_data: times BUFFER_DATA_SIZE db 0x00


;==============================
segment seg_bkg_data
	bkg_data: times BKG_DATA_SIZE db 0x00


;==============================
segment seg_spr_data
	spr_data: times SPR_DATA_SIZE db 0x00
