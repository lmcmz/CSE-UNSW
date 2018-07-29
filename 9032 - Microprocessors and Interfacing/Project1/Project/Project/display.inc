/*
 * display.inc
 *
 *  Created: 18/10/2017 3:00:18 AM
 *   Author: lmcmz
 */ 

 //////////////////////////////////////////

 	
 .def temp1	=r20	

.macro do_lcd_command
	ldi r16, @0
	rcall lcd_command
	rcall lcd_wait
.endmacro

.macro do_lcd_data
	mov r16, @0
	rcall lcd_data
	rcall lcd_wait
.endmacro

.macro lcd_set
	sbi LCD_CTRL_PORT, @0
.endmacro

.macro lcd_clr
	cbi LCD_CTRL_PORT, @0
.endmacro

.equ F_CPU = 16000000
.equ DELAY_1MS = F_CPU / 4 / 1000 - 4
; 4 cycles per iteration - setup/call-return overhead

sleep_1ms:			;delay
	push r24
	push r25
	ldi r25, high(DELAY_1MS)
	ldi r24, low(DELAY_1MS)
delayloop_1ms:
	sbiw r25:r24, 1
	brne delayloop_1ms
	pop r25
	pop r24
	ret

sleep_5ms:
	rcall sleep_1ms
	rcall sleep_1ms
	rcall sleep_1ms
	rcall sleep_1ms
	rcall sleep_1ms
	ret

sleep_50ms:
	rcall sleep_5ms
	rcall sleep_5ms
	rcall sleep_5ms
	rcall sleep_5ms
	rcall sleep_5ms
	rcall sleep_5ms
	rcall sleep_5ms
	rcall sleep_5ms
	rcall sleep_5ms
	rcall sleep_5ms
	ret

sleep_300ms:
	rcall sleep_50ms
	rcall sleep_50ms
	rcall sleep_50ms
	rcall sleep_50ms
	rcall sleep_50ms
	rcall sleep_50ms
	ret

display_welcome:
	ldi	temp1,'W'
	do_lcd_data temp1

	ldi	temp1,'e'
	do_lcd_data temp1

	ldi	temp1,'l'
	do_lcd_data temp1

	ldi	temp1,'c'
	do_lcd_data temp1
	
	ldi	temp1,'o'
	do_lcd_data temp1

	ldi	temp1,'m'
	do_lcd_data temp1

	ldi	temp1,'e'
	do_lcd_data temp1

	ldi	temp1,'!'
	do_lcd_data temp1

	do_lcd_command 0b11000000

	ldi	temp1,'E'
	do_lcd_data temp1

	ldi	temp1,'n'
	do_lcd_data temp1

	ldi	temp1,'t'
	do_lcd_data temp1

	ldi	temp1,'e'
	do_lcd_data temp1
	
	ldi	temp1,'r'
	do_lcd_data temp1

	ldi	temp1,':'
	do_lcd_data temp1

	ldi	temp1,'X'
	do_lcd_data temp1

	ldi	temp1,'&'
	do_lcd_data temp1

	ldi	temp1,'Y'
	do_lcd_data temp1
	ret

display_input_x:
	ldi	temp1,'E'
	do_lcd_data temp1

	ldi	temp1,'n'
	do_lcd_data temp1

	ldi	temp1,'t'
	do_lcd_data temp1

	ldi	temp1,'e'
	do_lcd_data temp1
	
	ldi	temp1,'r'
	do_lcd_data temp1

	ldi	temp1,' '
	do_lcd_data temp1

	ldi	temp1,'X'
	do_lcd_data temp1

	ldi	temp1,' '
	do_lcd_data temp1

	ldi	temp1,'('
	do_lcd_data temp1
	
	ldi	temp1,'0'
	do_lcd_data temp1

	ldi	temp1,'-'
	do_lcd_data temp1

	ldi	temp1,'6'
	do_lcd_data temp1

	ldi	temp1,'4'
	do_lcd_data temp1

	ldi	temp1,')'
	do_lcd_data temp1

	do_lcd_command 0b11000000

	ldi	temp1,'X'
	do_lcd_data temp1

	ldi	temp1,'='
	do_lcd_data temp1
	ret


display_input_y:
	ldi	temp1,'E'
	do_lcd_data temp1

	ldi	temp1,'n'
	do_lcd_data temp1

	ldi	temp1,'t'
	do_lcd_data temp1

	ldi	temp1,'e'
	do_lcd_data temp1
	
	ldi	temp1,'r'
	do_lcd_data temp1

	ldi	temp1,' '
	do_lcd_data temp1

	ldi	temp1,'Y'
	do_lcd_data temp1

	ldi	temp1,' '
	do_lcd_data temp1

	ldi	temp1,'('
	do_lcd_data temp1
	
	ldi	temp1,'0'
	do_lcd_data temp1

	ldi	temp1,'-'
	do_lcd_data temp1

	ldi	temp1,'6'
	do_lcd_data temp1

	ldi	temp1,'4'
	do_lcd_data temp1

	ldi	temp1,')'
	do_lcd_data temp1

	do_lcd_command 0b11000000

	ldi	temp1,'Y'
	do_lcd_data temp1

	ldi	temp1,'='
	do_lcd_data temp1
	ret


display_start:
	ldi	temp1,'P'
	do_lcd_data temp1

	ldi	temp1,'u'
	do_lcd_data temp1

	ldi	temp1,'s'
	do_lcd_data temp1

	ldi	temp1,'h'
	do_lcd_data temp1

	ldi	temp1,' '
	do_lcd_data temp1

	ldi	temp1,'B'
	do_lcd_data temp1

	ldi	temp1,'u'
	do_lcd_data temp1

	ldi	temp1,'t'
	do_lcd_data temp1
	
	ldi	temp1,'t'
	do_lcd_data temp1

	ldi	temp1,'o'
	do_lcd_data temp1

	ldi	temp1,'n'
	do_lcd_data temp1

	do_lcd_command 0b11000000

	ldi	temp1,'T'
	do_lcd_data temp1

	ldi	temp1,'o'
	do_lcd_data temp1

	ldi	temp1,' '
	do_lcd_data temp1

	ldi	temp1,'S'
	do_lcd_data temp1

	ldi	temp1,'t'
	do_lcd_data temp1

	ldi	temp1,'a'
	do_lcd_data temp1

	ldi	temp1,'r'
	do_lcd_data temp1
	
	ldi	temp1,'t'
	do_lcd_data temp1

	ret


display_position:

	do_lcd_command 0b00000001

	ldi	temp1,'('
	do_lcd_data temp1

	mov	reader,	X_cursor
	rcall	LCD_DISPLAY_NUMBER

	ldi	temp1,','
	do_lcd_data temp1

	mov	reader,	Y_cursor
	rcall	LCD_DISPLAY_NUMBER

	ldi	temp1,','
	do_lcd_data temp1

	mov	reader,	Z_cursor
	rcall	LCD_DISPLAY_NUMBER

	ldi	temp1,')'
	do_lcd_data temp1

	do_lcd_command 0b11000000
	ldi	temp1,'S'
	do_lcd_data temp1

	ret