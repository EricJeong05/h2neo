/*
 * convertNprint.h
 *
 *  Created on: Aug 19, 2020
 *      Author: Jenny Cho
 */

#ifndef CONVERTNPRINT_H_
#define CONVERTNPRINT_H_

void int2strXX(int num, char*str);
void int2str(int num, char* str);

void displayFlowRate(float *value, char* buf);
void ftoa(float *value, char* str, int afterP);
void reverse(char* str, int len);
int intToStr(int x, char str[], int d);

#endif /* CONVERTNPRINT_H_ */
