XBR2			equ		0e3h
P1MDIN			equ		0adh
P2MDOUT			equ		0a6h
WDTCN			equ		0ffh
SFRPAGE			equ		084h
P1				equ		090h
P2				equ		0a0h

CONFIG_PAGE		equ		0fh
LEGACY_PAGE		equ		00h

				// close timer
				mov		WDTCN, #0deh
				mov		WDTCN, #0adh

				// setup port config
				mov		SFRPAGE, #CONFIG_PAGE
				mov		XBR2, #0c0h
				mov		P1MDIN, #0ffh
				mov		P2MDOUT, #0ffh
				mov		SFRPAGE, #LEGACY_PAGE
				
				mov   P1, #00000000h // btn array reference to 090h
				mov 	P2, #00000000h	// output array reference to 0a0h

				mov 	R0,	#10000000b 
				mov		B,	#00000000b

				// initial signal, status, btn and LED
				mov   20h, #0
				mov   P1, #0
				mov		R1,	#0
				mov 	R2,	#0
Loop_Begin:
				lcall handleOnclick
				// after btn state check, put val. to A, in order to do operation
				mov 	R1,	20h
				mov		A,	R1

				rightRotate:
					cjne	A,	#1,	leftRotate // check event
					lcall	Delay
					mov		R3,	A
					mov 	A,	R0
					rr    A
					mov		P2, A
					mov		R0,	A
					ljmp	Loop_Begin	

				// R_L
				leftRotate:
					cjne	A,	#2,	evenBlink // check event
					lcall	Delay
					mov		R3,	A
					mov 	A,	R0
					rl    A
					mov		P2, A
					mov		R0,	A
					ljmp	Loop_Begin

				// Blink
				evenBlink:
					cjne	A,	#3,	oddBlink // check event
					lcall	Delay
					mov		R3,	A
					xrl   B  ,#01010101b
					mov		P2, B
					ljmp	Loop_Begin	
				
				oddBlink:
					cjne	A,	#4,	Jump // check event
					lcall	Delay
					mov		R3,	A
					xrl   B  ,#10101010b
					mov   P2, B
					ljmp	Loop_Begin	
				
				// if not 
				Jump:
					ljmp	Loop_Begin

// button even handler
handleOnclick:
				mov		R1,	P1  // set the input val to reg.
				mov 	A,  R1	
	event1: // right rotate
				cjne	A,	#10000000b,	event2
				mov   20h,	#1
				ret  

	event2://	left rotate
				cjne	A,	#01000000b,	event3
				mov 	20h,	#2
				ret
	event3:// 	even num blink
				cjne	A,	#00100000b,	event4
				mov 	20h,	#3
				mov		B,	#0 // this is quite important, because we need to inital the LED array statt
				ret
	event4:// 	blink odd num.
				cjne	A,	#00010000b,	return
				mov 	20h,	#4
				mov		B,	#0 // like the upper one, or it will leave strange state

	return:
				ret

; use the slow down the blick state, due to the cpu clock is quite fast
Delay:	mov R1,#50
Delay0: MOV R2, #30
Delay1: MOV R3, #249	
Delay2: DJNZ R3, Delay2
				DJNZ R2, Delay1
				djnz R1, Delay0
				ret
end