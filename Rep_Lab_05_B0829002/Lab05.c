#include "C8051F040.h"
#include "LCD.h"
#include <stdio.h>
#include <string.h>

char LCD_status;

void
LCD_PortConfig ()
{
	//initialize SFR setup page
	SFRPAGE = CONFIG_PAGE;                 // Switch to configuration page

	//setup the cross-bar and configure the I/O ports
	XBR2 = 0xc0;
	P3MDOUT = 0x3f;
	P1MDIN = 0xff;

	//set to normal mode
	SFRPAGE = LEGACY_PAGE;
}//end of function LCD_PortConfig ()

/****************
The delay_lcd seem to be not long enough. 
Try to change the default value as 10000...
****************/
unsigned int delay_lcd=1000;

void
LCD_Delay ()
{
	int i;
	for (i=0;i<delay_lcd;i++); // wait for a long enough time...
}

void
LCD_SendCommand (char cmd);

void
LCD_Init ()
{
	LCD_SendCommand(0x02);        // Initialize as 4-bit mode
	LCD_SendCommand (0x28);		//Display function: 2 rows for 4-bit data, small 
	LCD_SendCommand (0x0e);		//display and curson ON, curson blink off
	LCD_SendCommand (0x01);		//clear display, cursor to home
	//LCD_SendCommand (0x10);		//cursor shift left
	//LCD_SendCommand (0x06);		//cursor increment, shift off
}

void
LCD_Status_SetRS ()
{
	LCD_status = LCD_status | 1;
}

void
LCD_Status_ClearRS ()
{
	LCD_status = LCD_status & 0xfe;
}

void
LCD_Status_SetWord (char word)
{
	word = word & 0x0f;
	LCD_status = LCD_status & 0x03;
	LCD_status = LCD_status | (word<<2);
}

void
LCD_Status_SetEnable ()
{
	LCD_status = LCD_status | 0x02;
}


void
LCD_Status_ClearEnable ()
{
	LCD_status = LCD_status & 0xfd;
}


void
LCD_SendCommand (char cmd)
{
	LCD_Status_ClearRS ();

	///send the higher half
	LCD_Status_SetWord ((cmd>>4) & 0x0f);
	LCD_Status_SetEnable ();
	P3 = LCD_status;
	LCD_Delay ();
	LCD_Status_ClearEnable ();
	P3 = LCD_status;
	LCD_Delay ();

	///send the lower half
	LCD_Status_SetWord (cmd&0x0f);
	LCD_Status_SetEnable ();
	P3 = LCD_status;
	LCD_Delay ();
	LCD_Status_ClearEnable ();
	P3 = LCD_status;
	LCD_Delay ();
}

void
LCD_SendData (char dat)
{
	LCD_Status_SetRS ();

	///send the higher half
	LCD_Status_SetWord ((dat>>4) & 0x0f);
	LCD_Status_SetEnable ();
	P3 = LCD_status;
	LCD_Delay ();
	LCD_Status_ClearEnable ();
	P3 = LCD_status;
	LCD_Delay ();

	///send the lower half
	LCD_Status_SetWord (dat&0x0f);
	LCD_Status_SetEnable ();
	P3 = LCD_status;
	LCD_Delay ();
	LCD_Status_ClearEnable ();
	P3 = LCD_status;
	LCD_Delay ();
}

void
LCD_ClearScreen ()
{
	LCD_SendCommand (0x01);
}

void
LCD_CursorLeft()
{
	LCD_SendCommand (0x10);

}

void
LCD_CursorRight()
{
	LCD_SendCommand (0x14);

}

void 
LCD_CursorTop()
{
	LCD_SendCommand (0x02);
}

void
LCD_CursorDown()
{
	LCD_SendCommand (0xC0);	
}

void
Shutup_WatchDog ()
{
	WDTCN = 0xde;
	WDTCN = 0xad;
}//end of function Shutup_WatchDog
int 
button_detect ()
{
	int key_hold;
	int key_release;
	int count;
	int button_state;
	do {
,		key_hold = P1;
		//button_state = key_hold;
		button_state = P1 ;     
	} while (!key_hold);

	//Stage 2: wait for key released
	key_release = 0;
	count = 100;
	while (!key_release) {
		key_hold = P1; 
		if (key_hold) {
			count = 10;
		}
		else {
			count--;
			if (count==0) {
				key_release = 1;
			}
		}
	}//Stage 2: wait for key released
	return button_state;
}//end of function button_detect ()

void
main ()
{
	int i;     // for loop    
	int button_push; 
	int line = 1;// 1 and 2
	int CursorPos = 0;
	
	char str[16] = "";
	char str2[16] = "";
	char strTmp[16] = "";
	
	Shutup_WatchDog ();
	LCD_PortConfig ();
	LCD_Init ();
	P1 = 0;
	LCD_ClearScreen ();

	for (i=0 ; i<16 ; i++){
		str[i] = ' ';
		str2[i] = ' ';
	}
	
	while (1){
		button_push = button_detect(); 

		// print string
		if (button_push == 1 && CursorPos < 16){
			if (line == 1){       // judge which line
				for (i = 15 ; i>= CursorPos ; i--){
					str[i] = str[i-1];
				} 
				str[CursorPos] = 'A';
			}
			else{
				for (i = 15 ; i>= CursorPos ; i--){
					str2[i] = str2[i-1];
				}
				str2[CursorPos] = 'A';
			}
			LCD_SendData ('A');
			CursorPos += 1;
			for (i = CursorPos ; i<16 ; i++){
				if (line == 1)
					LCD_SendData(str[i]);
				else
					LCD_SendData(str2[i]);
			}
			for (i = 15 ; i >= CursorPos ; i--){
				LCD_CursorLeft();
			}
		}
		else if (button_push == 2 && CursorPos < 16){
			if (line == 1){       // judge which line
				for (i = 15 ; i>= CursorPos ; i--){
					str[i] = str[i-1];
				} 
				str[CursorPos] = 'B';
			}
			else{
				for (i = 15 ; i>= CursorPos ; i--){
					str2[i] = str2[i-1];
				}
				str2[CursorPos] = 'B';
			}
			LCD_SendData ('B');
			CursorPos += 1;
			for (i = CursorPos ; i<16 ; i++){
				if (line == 1)
					LCD_SendData(str[i]);
				else
					LCD_SendData(str2[i]);
			}
			for (i = 15 ; i >= CursorPos ; i--){
				LCD_CursorLeft();
			}
		}
		else if (button_push == 4 && CursorPos < 16){
			if (line == 1){       // judge which line
				for (i = 15 ; i>= CursorPos ; i--){
					str[i] = str[i-1];
				} 
				str[CursorPos] = 'C';
			}
			else{
				for (i = 15 ; i>= CursorPos ; i--){
					str2[i] = str2[i-1];
				}
				str2[CursorPos] = 'C';
			}
			LCD_SendData ('C');
			CursorPos += 1;
			for (i = CursorPos ; i<16 ; i++){
				if (line == 1)
					LCD_SendData(str[i]);
				else
					LCD_SendData(str2[i]);
			}
			for (i = 15 ; i >= CursorPos ; i--){
				LCD_CursorLeft();
			}
		}
		else if (button_push == 8){
			/*
				Implement the `new-line key
				Change to the next line if a new-line button is pressed at Line 1
				Scroll the screen if a new-line button is pressed at Line 2
			*/
			if(line == 1){
				LCD_CursorDown();
				for (i = 0 ; i < CursorPos ; i++){
					LCD_CursorRight();
				}
				line = 2;
				LCD_SendCommand (0xC0);
			}
			else{
				// line 2 scroll down, replace the first line with the second line and clear the second line
				for (i = 0 ; i < 16 ; i++){
					str[i] = str2[i];
					str2[i] = ' ';
				}
				LCD_ClearScreen();
				CursorPos = 0;
				LCD_CursorTop();
				for (i = 0 ; i < 16 ; i++){
					LCD_SendData(str[i]);
				}
				LCD_CursorDown();
				for (i = 0 ; i < 16 ; i++){
					LCD_SendData(str2[i]);
				}
				LCD_SendCommand (0xC0);
			}
		}


		// move cursor 

		else if (button_push == 16){
			if (CursorPos<16){
				LCD_CursorRight();
				CursorPos += 1;
			}
		}
		else if (button_push == 32){
			if (CursorPos>-1){
				LCD_CursorLeft();
				CursorPos -= 1;
			}
		}
		else if (button_push == 64){
			if(line == 2){
				LCD_CursorTop();
				for (i = 0 ; i < CursorPos ; i++){
					LCD_CursorRight();
				}
				line = 1;
			}
		}
		else if (button_push == 128){
			if(line == 1){
				LCD_CursorDown();
				for (i = 0 ; i < CursorPos ; i++){
					LCD_CursorRight();
				}
				line = 2;
			}
		}
	};

}