// define control registers (with address)
XBR2			equ		0e3h
P1MDIN			equ		0adh
P2MDOUT			equ		0a6h
WDTCN			equ		0ffh
SFRPAGE			equ		084h
P1				equ		090h
P2				equ		0a0h

// define control registers for timer control
TMOD		equ		089h
TCON		equ		088h
CKCON		equ		08eh
IE			equ		0a8h
TL0			equ		08ah
TH0			equ		08ch

// define control words
CONFIG_PAGE		equ		0fh
LEGACY_PAGE		equ		00h
				
				org		0h
				ljmp	main

				org		0bh
				ljmp	Timer0_ISR

				org		0100h

main:
		lcall	configP
		lcall	ConfigT
		mov		R2, #4			// the ISR will be called every 4 times

Loop:
		lcall handleOnclick
		ljmp Loop

Loop_Begin:
				
				// R0 is for LED so it cant be used at other place
				mov 	R1,	20h

				// check the status sign
				mov		A,	R1
				rotateRightFunc: // right shift
					cjne	A,	#1,	rotateLeftFunc
					mov 	A,	R0
					rr    A
					mov		P2, A
					mov		R0,	A
				  ret 
				rotateLeftFunc: // left shift
					cjne	A,	#2,	blinkEvenFunc
					mov 	A,	R0
					rl    A
					mov		P2, A
					mov		R0,	A
					ret
				blinkEvenFunc: // blink even
					cjne	A,	#3,	blinkOddEven
					xrl   B  ,#01010101b
					mov		P2, B
					ret
				blinkOddEven: // blink odd
					cjne	A,	#4,	Jump
					xrl   B  ,#10101010b
					mov		P2, B
					ret
				Jump:
					ret 
// setting timmer 
configP:
		// turn-off the watch-dog timer
		mov		WDTCN, #0deh
		mov		WDTCN, #0adh

		// setup port configuration
		mov		SFRPAGE, #CONFIG_PAGE
		mov		XBR2, #0c0h
		mov		P1MDIN, #0ffh
		mov		P2MDOUT, #0ffh
		mov		SFRPAGE, #LEGACY_PAGE
				
		mov 	P2, #00000000h	// output LED

		mov 	R0,	#10000000b // init LED ,R0 is for LED so it cant be used at other place 
		mov		B,	#01010101b // odd flash		
				
		//detect button and display
		mov   20h, #0 // init status sign
		mov   P1, #0
		ret

ConfigT: // setting timer
		mov		TMOD, #01h
		mov		TCON, #010h
		mov		CKCON, #010h
		mov		IE, #082h
		mov		TL0, #0
		mov		TH0, #0
		ret

handleOnclick:
				mov		R1,	P1
				mov 	A,  R1	
	rotateRight: // RR
				cjne	A,	#10000000b,	rotateLeft
				mov   20h,	#1
				ret  

	rotateLeft://	RL
				cjne	A,	#01000000b,	blinkEven
				mov 	20h,	#2
				ret
	blinkEven:// 	BlinkEven
				cjne	A,	#00100000b,	blinkOdd
				mov 	20h,	#3
				mov		B,	#0
				ret
	blinkOdd:// 	BlinkOdd
				cjne	A,	#00010000b,	return
				mov 	20h,	#4
				mov		B,	#0
				ret
	return:
				ret

Timer0_ISR:
		DJNZ	R2, ResetT
		mov R2, #4 
		lcall Loop_Begin

ResetT:
		mov		TL0, #0
		mov		TH0, #0
		reti

end