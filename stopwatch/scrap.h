/*
 * scrap.h
 *
 * This file contains all the unorganized method headers. These should be eventually
 * moved to source files with descriptive names (just copy paste).
 *
 *  Created on: Jul 24, 2020
 *      Author: Jenny Cho
 */
#include <msp430.h>
#include <stdio.h>
#include <string.h>
#include <math.h>


#ifndef SCRAP_H_
#define SCRAP_H_

// Functions
//void Clock_Init_1MHz(void);  // in lcd_screen.c


void Timer0_A5_Init(void);
void delayMS(int msecs);
void startTimer0_A5(void);
void stopTimer0_A5(void);
void getSec(int tics);

void int2strXX(int num, char*str);
void int2str(int num, char* str);

// from rotary_encoder.c or whatever the name is
void displayFlowRate(float *value, char* buf);  // prev name: display_value
void ftoa(float *value, char* str, int afterP);
void reverse(char* str, int len);
int intToStr(int x, char str[], int d);

#endif /* SCRAP_H_ */
