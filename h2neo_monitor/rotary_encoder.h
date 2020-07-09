#include <msp430.h>

#ifndef ROTARY_ENCODER_INCLUDE
#define ROTARY_ENCODER_INCLUDE

/********************************************************************************
 *
 * Functions
 *
 *******************************************************************************/
void RotEnc_Init(void);
void get_direction(void);
void display_value(float *value);
void ftoa(float *value, char* str, int afterP);
void reverse(char* str, int len);
int intToStr(int x, char str[], int d);

#define CH_A						BIT4  // P1.4
#define CH_B						BIT5  // P1.5
//#define RE_BUTTON					BIT4  // P2.4
#define RE_BUTTON					BIT7  // P2.2
#define TEST_LED1					BIT0  // P1.0 (on-board LED1)
#define TEST_LED2					BIT7  // P4.7 (on-board LED2)
#define BRD_BUTTON1					BIT1  // P2.1 (on-board S1)
#define BRD_BUTTON2					BIT1  // P1.1 (on-board S2)

static const signed short rot_enc_table[] = {
								 0,  1, -1,  0,
								-1,  0,  0,  1,
								 1,  0,  0, -1,
								 0, -1,  1,  0};

#endif
