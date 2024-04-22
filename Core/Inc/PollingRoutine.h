/*
 * PollingRoutine.h
 *
 *  Created on: Oct 24, 2023
 *      Author: karl.yamashita
 *
 *
 *      Template
 */

#ifndef INC_POLLINGROUTINE_H_
#define INC_POLLINGROUTINE_H_


/*

#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include <errno.h>
#include <ctype.h>
#define Nop() asm(" NOP ")

*/


void PollingInit(void);
void PollingRoutine(void);

void FDCAN_CheckFifoRdy(FDCAN_Struct *msg);

void FDCAN1_Parse(FDCAN_Struct *msg);
void FDCAN2_Parse(FDCAN_Struct *msg);

#endif /* INC_POLLINGROUTINE_H_ */
