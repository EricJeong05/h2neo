/*
 * test.h
 *
 *  Created on: Jul 9, 2020
 *      Author: Jenny Cho
 */

#ifndef TEST_H_
#define TEST_H_

// main.c
void active_monitor(void);

// opticalSensor.c
void ADC12_0_Init(void);

// stopwatch.c
void Timer0_A5_Init(void);
void delayMS(int msecs);
void startTimer0_A5(void);
void stopTimer0_A5(void);
void getSec(int tics);


#endif /* TEST_H_ */
