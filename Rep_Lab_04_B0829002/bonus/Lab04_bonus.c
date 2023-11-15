#include "C8051F040.h"

int N = 100;
int status;
int count;
int tmp, tmp_status;
int type;
void Timer0_ISR ();

void Port_Configuration () {
	//initialize SFR setup page
	SFRPAGE = CONFIG_PAGE;                 // Switch to configuration page

	//setup the cross-bar and configure the I/O ports
	XBR2 = 0xc0;
	P2MDOUT = 0xff;

	//set to normal mode
	SFRPAGE = LEGACY_PAGE;
}//end of function Port_Configuration

void Timer_Configuration () {
	// SETTING THE TIMER WORKING MODE
	TMOD = 0x01;
	TCON = 0x10;
	CKCON = 0x10;

	IE = 0x82;
	TL0 = 0;
	TH0 = 0;
}//end of function Timer_Configuration

void Config () {
	//turn-off watch-dog timer
	WDTCN = 0xde;
	WDTCN = 0xad;

	OSCICN = 0x83;
	CLKSEL = 0x00;

	Port_Configuration ();

	Timer_Configuration ();
}//end of function Default_Config
int button_detect () {
	int key_hold, key_release;
	int count;
	int button_status;
	do {
		key_hold = P1;
		button_status = P1 ;     
	} while (!key_hold);

	//Stage 2: wait for key released
	key_release = 0;
	count = N;
	while (!key_release) {
		key_hold = P1;
		if (key_hold) {
			count = N;
		}
		else {
			--count == 0 ? key_release = 1 : 1;
		}
	}//Stage 2: wait for key released
	return button_status;
}

int main () {
	int button ; 
	Config (); // config setting
	status = 1;
	count = 0;
	type = 0;
	P1 = 0;
	P2 = status;
	while (1) {
		button = button_detect();
		if (button == 1) {
 			 type = type +1;
			 if(type ==4) {
			 	type =0;
				}
			}
	}//end while (1)
}//end of function main

void Timer0_ISR () interrupt 1{
	count++;
	tmp_status = status;
	if (count == 4) {
		count = 0;
		if (type == 0 ){
			if (status == 1){ 
				status=128;
			}
			else{
				status = status>>1;
			}
		} else if (type == 1) {
			if (status==128){ 
				status=1;
			}
			else{
				status = status<<1;
			}
		} else if (type == 2) {
			//event even bit blinck
			P2 = 170;
			TH0 = 0;
			TL0 = 0;
			count = 0;
			return;
		} else if (type == 3) {
			//event odd bit blinck
			P2 = 85;
			TH0 = 0;
			TL0 = 0;
			count = 0;
			return;
		}
		P2 = status;
	} else if (count == 2 && (type == 2 || type == 3)) {
		P2 = 00000000;
	}

	TH0 = 0;
	TL0 = 0;
}//end of function Timer0_ISR



