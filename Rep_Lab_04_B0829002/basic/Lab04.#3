#include "C8051F040.h"
int N = 100;
void Port_Configuration () {
	XBR2 = 0xc0;
	P1MDIN = 0xff;
	P2MDOUT = 0xff;
}//end of function Port_Configuration

void Default_Config () {
	//turn-off watch-dog timer
	// disable watchdog timer
	WDTCN = 0xde;
	WDTCN = 0xad;

	//initialize SFR setup page
	SFRPAGE = CONFIG_PAGE;                 // Switch to configuration page

	Port_Configuration ();

	//set to normal mode
	SFRPAGE = LEGACY_PAGE;
}//end of function Default_Config

int button_detect () {
	int key_hold, key_release, count, button_state;
	do {
		key_hold = P1;
		button_state = P1 ;     
	} while (!key_hold);

	//Stage 2: wait for key released
	key_release = 0;
	count = N;
	while (!key_release) {
		key_hold = P1; 
		key_hold ? count = N : (--count == 0 ? key_release = 1 : 1);
	}//Stage 2: wait for key released
	return button_state;
}//end of function button_detect ()

int main () {
	int status;
	int button ;
	Default_Config ();
	P1 = 0;
	status = 1;
	P2 = status;
	while (1) {
		button = button_detect();
		if (button == 1){
			if (status == 1){
				status = 128;
			}
			else{
				status = status >> 1;
			}		
		}
		if (button == 2){
			if (status == 128){
				status = 1;
			}
			else{
				status = status << 1;
			}	
		}
		P2 = status	;	
	}//end while (1)
}//end of function main