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

#endif /* SCRAP_H_ */
