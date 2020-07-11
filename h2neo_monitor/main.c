// Written by Jenny Cho
// Modified in May 11th, 2020

// Project h2neo

// The following project is for an electric flow rate monitor for gravity-assisted
// IV therapy equipment. A LCD screen is integrated into a MSP430F5529 Launchpad,
// interfaced via SPI communication. Other user interface features a rotary encoder
// that is used to adjust and input setting by the user.
// The flow rate sensing is done using an optical system consisting of an infrared
// LED and a photodiode.

#include <msp430f5529.h>
#include "nokia5110.h"
#include "rotary_encoder.h"
#include "test.h"

#define   Num_of_Results   10
volatile unsigned int results[Num_of_Results];


unsigned short ps;		// Grab BIT4 and BIT5
unsigned short ns;
float value = 0;

/********************************************************************************
 * main.c
 ********************************************************************************/
int main(void) {
	WDTCTL = WDTPW + WDTHOLD;				// stop watchdog timer
	Clock_Init_1MHz();
	SPI_Init();
	_delay_cycles(500000);

	LCD_Init();
	RotEnc_Init();
	clearLCD();

	setCursor(0, 0);

//	test_adc0_init();
	/*
	 * ADC12SHT0_x   -- sample-and-hold time. Defines num ADC12CLK cycles in the sampling period
	 * 						 0b100 --> 64 cycles
	 * ADC12ON       -- ADC12_A on
	 * ADC12MSC      -- set multiple sample and conversion (valid for sequence or repeated modes)
	 * ADC12REFON    -- reference generator ON
	 * ADC12REF2_5V  -- 0b->1.5V, 1b->2.5V (ADC12REFON must be set)
	 */
	REFCTL0 &= ~REFMSTR;
	ADC12CTL0 = ADC12ON + ADC12SHT0_8 + ADC12MSC;
	ADC12CTL0 |= ADC12REFON;
//	ADC12CTL0 &= ~ADC12REF2_5V;  // 0b, ref voltage = 1.5V
	ADC12CTL0 |= ADC12REF2_5V;  // 1b, ref voltage = 2.5V


	/*
	 * ADC12SHP      -- sample-and-hold-pulse-mode select (1->SAMPCON sourced from sampling timer)
	 * ADC12CONSEQ_x -- 2->repeat single channel
	 */
	ADC12CTL1 = ADC12SHP+ADC12CONSEQ_2;       // Use sampling timer, set mode
	ADC12IE = 0x01;                           // Enable ADC12IFG.0
	ADC12CTL0 |= ADC12ENC;                    // Enable conversions
	/*
	 * Enable A/D channel A0 (P6.0)
	 * If no ports connect to desired A##, then use ADC12MCTLx register (x == ##)
	 */
	P6SEL |= 0x01;
	ADC12MCTL0 =
	ADC12CTL0 |= ADC12SC;                     // Start conversion

	static unsigned char i = 0;



//	__bis_SR_register(GIE);					// enable interrupts
	__bis_SR_register(LPM4_bits + GIE);       // Enter LPM4, Enable interrupts (ADC example)

	__no_operation();						// For debugger

	while (1) {
//		test_adc0();
		// display value
		prints(results[i++]);
		if (i == 8)
		{
		  i = 0;
		}


//		ps = (P1IN>>4) & 0x3;

//		display_value(value);
//		prints("hello");
//		_delay_cycles(200000);
	}
}
