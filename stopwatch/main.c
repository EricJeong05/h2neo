#include <msp430.h>
#include <stdio.h>
#include <string.h>
#include "scrap.h"
#include "nokia5110.h"

#define MEMSIZE			5  // size of memory buffer used for flow rate calculations

// tic - number of times the Timer ISR is entered after x clock cycles
//			tic will be programmed to be 1ms long
// sec - seconds (tic * clock cycles)
// min - minutes (sec / 60)
unsigned long int tic = 0;  // data type short can only go up to 65,535 ms which is only ~1m5sec
unsigned short int msec = 0, sec = 0, min = 0; //Example of Global variables.
unsigned short int oMsec = 0, oSec = 0, oMin = 0; // old sec; old min
unsigned char butFLG = 0;  // button flag

// save last 5 time interval values and average to find more accurate flow rate
unsigned long int ticMem[MEMSIZE];  // global var auto initialized to 0
unsigned short int index = 0;
char str[6]; // used to convert each integer to string

/*
 * main.c
 */
int main (void)
{
	//Setup
	WDTCTL = WDTPW + WDTHOLD; //Stop Watchdog Timer
	P1DIR |= BIT0;					// Configure P1.0 as output

	//Setup Buttons
	P1DIR &= ~BIT1;					// P1.1 input
	P1REN |= BIT1; 					// Enable pullup resistor of P1.1 (default: GND)
	P1OUT |= BIT1;					// Set pullup resistor to active (+3.3V) mode

	Clock_Init_1MHz();				// used for TimerA and LCD
	// Initialize Timer A0
	Timer0_A5_Init();
	
	SPI_Init();						// for LCD screen connection
	_delay_cycles(50000);

	LCD_Init();
	clearLCD();
	setCursor(0, 0);
	prints("test:");
	setCursor(36, 0);  // each character is 6wide 8tall
	prints("00:00:00");

	// set up display of memory buffer
	short i = 0, yCursor = 1;  // yCursor = 0 is taken by the stopwatch display
	for (i = 0; i < MEMSIZE; i++) {
		int2str(ticMem[i], str);
		setCursor(0, yCursor++);
		prints(str);
	}
	yCursor = 1;

	P1IE |= BIT1;					// P1.1 interrupt enabled
	P1IFG &= ~BIT1;					// P1.1 interrupt flag cleared
	_enable_interrupts(); //Enable General Interrupts. Best to do this last.


	while (1) {
		//Poll Buttons here. Control the Timer. Update LCD Display.
		if (butFLG) {
			if (!TA0CCR0) { // TIMER IS OFF if !; else not 0, aka timer is ON
				startTimer0_A5();
			} else {
				stopTimer0_A5();

				ticMem[index] = tic;		// save measured time to ticMem buffer

				// print to screen (for debugging)
				int2str(ticMem[index++], str);
				setCursor(0, yCursor);
				prints("      ");  // 6 blank to clear screen
				setCursor(0, yCursor++);
				prints(str);
				if (index > MEMSIZE-1) {
					index = 0;  // index wraparound
					yCursor = 1;
				}

				startTimer0_A5();
			}
			butFLG = 0;
		}
		msec = tic;
		sec = tic / 1000;
		min = tic / 60000;

		if (msec != oMsec) {  // if different
			char str[2];
			msec = msec % 100;
			int2strXX(msec, str);
			setCursor(72, 0);
			prints(str);
		}
		oMsec = msec;

		if (sec != oSec) {  // if different
			char str[2];
			int2strXX(sec%60, str);
			setCursor(54, 0);
			prints(str);
		}
		oSec = sec;

		if (min != oMin) {
			char str[2];
			int2strXX(min, str);
			setCursor(36, 0);
			prints(str);
		}
		oMin = min;

		/* CODE FOR BLINKING ON-BOARD LED EXAMPLE, works well... dont touch :-(
		P1OUT |= BIT0; //Drive P1.0 HIGH - LED1 ON
		delayMS(500); //Wait 0.5 Secs

		P1OUT &= ~BIT0; //Drive P1.0 LOW - LED1 OFF
		delayMS(500); //Wait 0.5 Secs
		*/

	}
 }


/* TIMER_A Operation Notes:
 *
 * 		Clock source select and divider
 * 			Options - ACLK, SMCLK, or externally from TAxCLK or INCLK
 * 			Selected with TASSEL bits
 * 			Can be divided by 2,4,8 using ID bits
 * 			Can be further divided by 2,3,4,5,6,7,8 using TAIDEX bits
 * 			Timer clock divider is reset when TACLR is set (but restored when TACLR is cleared)
 *
 * 		When the timer mode is UP or UP/DOWN...
 * 			Timer may be stopped by writing 0 to TAxCCR0.
 * 			It
 * 			 may then be restarted by writing a nonzero value to TAxCCR0.
 *
 * Debouncing in firmware
 * 		You can use a timer that generates an interrupt every 10ms, for example.
 * 		In this interrupt you read your inputs and compare it to the state of
 * 		the previous interrupt. If it has changed you can wait for the next
 * 		interrupt and check if this changed state still exists - if so, you can
 * 		register this event as a debounced button press and set a flag to signal
 * 		the press to the main program. With the right timer interval you can
 * 		debounce even the worst buttons. And for a human, pressing a button
 * 		shorter than 10 or 20ms is almost impossible, so a detection is guaranteed.
 */
