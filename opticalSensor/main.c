#include <msp430.h> 

#define	NUM_RESULTS		8
#define	THRESHOLD		2600  // low "peak" value for drop detection

volatile unsigned int results[NUM_RESULTS];
                                            // Needs to be global in this
                                            // example. Otherwise, the
                                            // compiler removes it because it
                                            // is not used for anything.


// functions
void ADC12_0_Init(void);
void ADC12_0_Init2(void);

/*
 * main.c
 */
int main(void) {
    WDTCTL = WDTPW | WDTHOLD;					// Stop watchdog timer

    P1DIR |= BIT0;					// Configure P1.0 as output (for blinking debugging)

    ADC12_0_Init2();

    ADC12CTL0 &= ~ADC12SC;      				// Clear the start bit (precautionary)
    ADC12CTL0 |= ADC12SC;						// Start conversion

    __bis_SR_register(LPM4_bits + GIE);      	// Enter LPM4, Enable interrupts
    while(1) {

    }
}


/*
 * opticalSensor.c
 *
 * 		- Initialize ADC12_0 (P6.0)
 * 		-
 *
 * 	Not sure if this is the cleanest way to do things but it works for now :-)
 *
 *
 *  Created on: Jul 14, 2020
 *      Author: Jenny Cho
 */


/*
 * See p.750 of https://www.ti.com/lit/ug/slau208q/slau208q.pdf?ts=1594281016406
 * ADC12SHT0_x   -- sample-and-hold time. Defines num ADC12CLK cycles in the sampling period
 * 						 0b100 --> 64 cycles
 * ADC12ON       -- ADC12_A on
 * ADC12MSC      -- set multiple sample and conversion (valid for sequence or repeated modes)
 */
void ADC12_0_Init2() {
	P6SEL |= 0x01;									// Enable A/D channel A0
	REFCTL0 &= ~REFMSTR;  			// Reset REFMSTR to hand over control to ADC12_A ref control registers
//	ADC12CTL0 = ADC12SHT0_8 | ADC12REFON | ADC12REF2_5V | ADC12ON;
	ADC12CTL0 = ADC12ON + ADC12SHT0_8 + ADC12MSC;
													// Turn on ADC12, set sampling time
													// 	set multiple sample conversion
	ADC12CTL0 |= ADC12REFON;						// Must be set to use internal reference voltage (ADC12REF2_5V)
	ADC12CTL0 &= ~ADC12REF2_5V;  					// 0b, ref voltage = 1.5V
	ADC12CTL1 = ADC12SHP + ADC12CONSEQ_2;			// Use sampling timer, set mode
	ADC12CTL1 |= ADC12SSEL_0;						// ADC12OSC (MODCLK)
	ADC12CTL2 |= ADC12PDIV;							// Predivide by 4 (0b == prediv by 1)

	// Set conversion memory control register ADC12MCTL0
	// SREF = 001b => V_R+ = VREF+ and V_R- = AVSS (GND)
	// INCH = 000b => A0 (INput CHannel)
	// EOS  =   0  => End of sequence not set (not a multi-channel conversion, so ignore)
	ADC12MCTL0 = ADC12SREF_1 | ADC12INCH_0;
	__delay_cycles(100);							// delay to allow Ref to settle

	ADC12IE = 0x01;									// Enable ADC12IFG.0
	ADC12CTL0 |= ADC12ENC;							// Enable conversions

}

/******************************************************************************
 *
 * ADC12_0_Init()
 *
 * Initialize P6.0 as ADC12. Uses 0 ~ (internal) 1.5V as input range and internal
 * clock for timing.
 * There may be some ignored code (between ADC12CTL0 REFCTL0 registers)
 *
 *****************************************************************************/
void ADC12_0_Init() {

	REFCTL0 &= ~REFMSTR;  			// Reset REFMSTR to hand over control to ADC12_A ref control registers
	REFCTL0 |= REFON;				// enable
	REFCTL0 |= REFVSEL_0;			// 1.5V
//	REFCTL0 |= REFVSEL_1;			// 2.0V
//	REFCTL0 |= REFVSEL_2;			// 2.5V

	/*
	 * ADC12SHT0_x   -- sample-and-hold time. Defines num ADC12CLK cycles in the sampling period
	 * 						 0b100 --> 64 cycles
	 * ADC12ON       -- ADC12_A on
	 * ADC12MSC      -- set multiple sample and conversion (valid for sequence or repeated modes)
	 */
	ADC12CTL0 = ADC12ON + ADC12SHT0_8 + ADC12MSC;

//	/******************* UNUSED *******************
//	 * ADC12REFON    -- reference generator ON
//	 * ADC12REF2_5V  -- 0b->1.5V, 1b->2.5V (ADC12REFON must be set)
//	 */
//	ADC12CTL0 |= ADC12REFON;
//	ADC12CTL0 &= ~ADC12REF2_5V;  	// 0b, ref voltage = 1.5V
//	ADC12CTL0 |= ADC12REF2_5V;  	// 1b, ref voltage = 2.5V

	/*
	 * ADC12SHP      -- sample-and-hold-pulse-mode select (1->SAMPCON sourced from sampling timer)
	 * ADC12CONSEQ_x -- 2->repeat single channel
	 */
	ADC12CTL1 = ADC12SHP + ADC12CONSEQ_2;      	// Use sampling timer, set mode
	ADC12CTL1 |= ADC12SSEL_0;					// ADC12OSC (MODCLK)
	ADC12CTL2 |= ADC12PDIV;						// Predivide by 4 (0b == prediv by 1)

	ADC12IE = 0x01;                       	    // Enable ADC12IFG.0

	ADC12MCTL0 = ADC12SREF_1;            	    // Vr+ = Vref+ and Vr- = AVss
	volatile unsigned int i;
	for (i=0; i<0x30; i++);              	    // Delay for reference start-up

	ADC12CTL0 |= ADC12ENC;               	    // Enable conversions
	/*
	 * Enable A/D channel A0 (P6.0)
	 * If no ports connect to desired A##, then use ADC12MCTLx register (x == ##)
	 */
	P6SEL |= 0x01;
}





#if defined(__TI_COMPILER_VERSION__) || defined(__IAR_SYSTEMS_ICC__)
#pragma vector=ADC12_VECTOR
__interrupt void ADC12ISR (void)
#elif defined(__GNUC__)
void __attribute__ ((interrupt(ADC12_VECTOR))) ADC12ISR (void)
#else
#error Compiler not supported!
#endif
{
	static unsigned char index = 0;

	switch(__even_in_range(ADC12IV,34))
	{
	case  0: break;                           // Vector  0:  No interrupt
	case  2: break;                           // Vector  2:  ADC overflow
	case  4: break;                           // Vector  4:  ADC timing overflow
	case  6:                                  // Vector  6:  ADC12IFG0
		if (ADC12MEM0 < THRESHOLD) {
			P1OUT ^= BIT0;  // toggle LED
		} else {
			P1OUT &= ~BIT0;
		}
		results[index] = ADC12MEM0;             // Move results
		index++;                                // Increment results index, modulo; Set Breakpoint1 here

		if (index == 8)
		{
		  index = 0;
		}
	case  8: break;                           // Vector  8:  ADC12IFG1
	case 10: break;                           // Vector 10:  ADC12IFG2
	case 12: break;                           // Vector 12:  ADC12IFG3
	case 14: break;                           // Vector 14:  ADC12IFG4
	case 16: break;                           // Vector 16:  ADC12IFG5
	case 18: break;                           // Vector 18:  ADC12IFG6
	case 20: break;                           // Vector 20:  ADC12IFG7
	case 22: break;                           // Vector 22:  ADC12IFG8
	case 24: break;                           // Vector 24:  ADC12IFG9
	case 26: break;                           // Vector 26:  ADC12IFG10
	case 28: break;                           // Vector 28:  ADC12IFG11
	case 30: break;                           // Vector 30:  ADC12IFG12
	case 32: break;                           // Vector 32:  ADC12IFG13
	case 34: break;                           // Vector 34:  ADC12IFG14
	default: break;
	}
}
