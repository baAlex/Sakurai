
; [sakurai.inc]
; - Alexander Brandt 2020


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

INPUT_STATE_LEN = 84 ; Keys, the second keyboard iteration of IBM


;==============================
segment seg_data

	str_test1: db "Haaiii!", "$" ; Used as "beep" - "boop"
	str_test2: db "Eehhh?!", "$"

	hex_table: db "0", "1", "2", "3", "4", "5", "6", "7"
	           db "8", "9", "A", "B", "C", "D", "E", "F"

	; Time
	time_previous_vector_sector: dw 0x0000
	time_previous_vector_offset: dw 0x0000
	time_miliseconds: dw 0x0000

	; Input
	input_previous_vector_sector: dw 0x0000
	input_previous_vector_offset: dw 0x0000

	input_state: times INPUT_STATE_LEN db 0x00

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

	str_time_init: db "Initializating time module...", UNIX_NL
	str_time_init_end:

	str_input_init: db "Initializating input module...", UNIX_NL
	str_input_init_end:

	str_render_init: db "Initializating render module...", UNIX_NL
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
