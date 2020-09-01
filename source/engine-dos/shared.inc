
; [sakurai.inc]
; - Alexander Brandt 2020


BUFFER_DATA_SIZE = 64000 ; Bytes
BKG_DATA_SIZE = 64000    ; "
VGA_DATA_SIZE = 64000    ; "

POOL_A_SIZE = 64000 ; Bytes
POOL_B_SIZE = 64000 ; "

PALETTE_LEN = 128  ; Colors
PALETTE_SIZE = 384 ; Bytes

VGA_SEGMENT = 0xA000
VGA_OFFSET = 0x0000

EXIT_SUCCESS = 0x00
EXIT_FAILURE = 0x01

UNIX_NL = 0x0A ; Log messages uses it

KEYBOARD_STATE_LEN = 84 ; Number of keys, modify this value
                        ; requires changes on the code

COMMAND_SIZE = 8
COMMANDS_TABLE_LEN = 28
COMMANDS_TABLE_SIZE = 224 ; In bytes

SPRITE_SLOT_SIZE = 4
SPRITE_LOAD_HEADER_SIZE = 8
SPRITE_INDIRECTION_SIZE = 128 ; 32 slots

SELF_MODIFYING_RETURN_SIZE = 2 ; In bytes (space for a "jmp near bx")


;==============================
segment seg_data

	str_test1: db "Haaiii!", "$" ; Used as "beep/boop"
	str_test2: db "Eehhh?!", "$"

	hex_table: db "0", "1", "2", "3", "4", "5", "6", "7"
	           db "8", "9", "A", "B", "C", "D", "E", "F"

	str_game_filename: db "game.dll", 0x00
	str_palette_filename: db "assets\palette.raw", 0x00

	; Console
	str_vga_error: db "You need a VGA adapter", "$"

	; Log
	str_separator: db UNIX_NL, "===============================", UNIX_NL, 0x00
	str_hello: db "Sakurai v0.3-alpha", UNIX_NL, 0x00

	str_time_init: db "Initializing time module...", UNIX_NL, 0x00
	str_keyboard_init: db "Initializing keyboard module...", UNIX_NL, 0x00
	str_render_init: db "Initializing render module...", UNIX_NL, 0x00

	str_copy_speed: db "Copy speed is (ms): ", 0x00
	str_main_loop: db UNIX_NL, "Entering main loop...", UNIX_NL, UNIX_NL, 0x00

	str_render_stop: db "Stopping render module...", UNIX_NL, 0x00
	str_keyboard_stop: db "Stopping keyboard module...", UNIX_NL, 0x00
	str_time_stop: db "Stopping time module...", UNIX_NL, 0x00

	str_vector_segment: db " - Vector segment: ", 0x00
	str_vector_offset: db " - Vector offset: ", 0x00

	str_mem_pool: db "Memory pool:", UNIX_NL, 0x00
	str_spr_load: db "Loading sprite...", UNIX_NL, 0x00
	str_mem_separator: db " ----", UNIX_NL, 0x00

	str_segment: db " - Segment: ", 0x00
	str_offset: db " - Offset: ", 0x00
	str_flags: db " - Flags: ", 0x00
	str_size: db " - Size: ", 0x00
	str_pool_a_full: db " - Pool A full", UNIX_NL, 0x00
	str_pool_b_full: db " - Pool B full", UNIX_NL, 0x00

	str_file_open_error: db "FileOpen() error, code: ", 0x00
	str_log_filename: db "sakurai.log", 0x00

	; Time
	time_previous_vector_sector: dw 0x0000
	time_previous_vector_offset: dw 0x0000
	time_milliseconds: dw 0x0000

	; Keyboard
	keyboard_previous_vector_sector: dw 0x0000
	keyboard_previous_vector_offset: dw 0x0000
	keyboard_state: times KEYBOARD_STATE_LEN db 0x00

	; Render
	render_palette_data: times PALETTE_SIZE db 0x00
	render_previous_mode: db 0x00

	; IntFD
	previous_ifd_vector_segment: dw 0x0000
	previous_ifd_vector_offset: dw 0x0000
	exit_request: dw 0x0000

	; Game
	sprite_load_header: times SPRITE_LOAD_HEADER_SIZE db 0x00
	sprite_indirection_table: times SPRITE_INDIRECTION_SIZE db 0x00


;==============================
segment seg_game_data
; https://en.wikipedia.org/wiki/Program_Segment_Prefix

	empty_word: dw 0x0000               ; 0x0000
	frame_counter: dw 0x0000            ; 0x0002
	ms_counter: dw 0x0000               ; 0x0004
	max_commands: dw COMMANDS_TABLE_LEN ; 0x0006
	ifd_arg1: dw 0x0000                 ; 0x0008
	ifd_arg2: dw 0x0000                 ; 0x000A
	ifd_arg3: dw 0x0000                 ; 0x000C
	ifd_arg4: dw 0x0000                 ; 0x000E
	                                    ; ---- 16 bytes

	input_x: db 0x00      ; 0x0010
	input_y: db 0x00      ; 0x0011
	input_a: db 0x00      ; 0x0012
	input_b: db 0x00      ; 0x0013
	input_u: db 0x00      ; 0x0014
	input_d: db 0x00      ; 0x0015
	input_l: db 0x00      ; 0x0016
	input_r: db 0x00      ; 0x0017
	input_select: db 0x00 ; 0x0018
	input_start: db 0x00  ; 0x0019
	unused1: db 0x00      ; 0x001A
	unused2: db 0x00      ; 0x001B
	unused3: db 0x00      ; 0x001C
	unused4: db 0x00      ; 0x001D
	unused5: db 0x00      ; 0x001E
	unused6: db 0x00      ; 0x001F
	                      ; ---- 16 bytes

	commands_table: times COMMANDS_TABLE_SIZE db 0x00 ; 0x0020, 224 bytes

	; --- PSP ends here ---

	org 256
	game_data: file "game.dll"
	times 512 db 0x00 ; FIXME, BUG. Some code is writting/reading in this area :(

GameFrame:
	call near 256 ; Near call
	retf ; Far return (CPU sets CS accordingly)


;==============================
segment seg_pool_a
	pool_a_data: rb POOL_A_SIZE

spr_a_draw:
	rb SELF_MODIFYING_RETURN_SIZE


;==============================
segment seg_pool_b
	pool_b_data: rb POOL_B_SIZE

spr_b_draw:
	rb SELF_MODIFYING_RETURN_SIZE


;==============================
segment seg_buffer_data
	buffer_data: rb BUFFER_DATA_SIZE


;==============================
segment seg_bkg_data
	bkg_data: rb BKG_DATA_SIZE
