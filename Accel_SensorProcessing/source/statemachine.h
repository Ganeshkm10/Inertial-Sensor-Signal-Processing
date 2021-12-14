/*
 * statemachine.h
 *
 *  Created on: 05-Dec-2021
 *      Author: Ganesh KM
 */

#ifndef STATEMACHINE_H_
#define STATEMACHINE_H_


#include "stdint.h"
#include "stdbool.h"
#include "MKL25Z4.h"
#include "fsl_debug_console.h"
/**
 * state machine function to transit between orientation state and acceleration state​
 * Initial state : Orientation (because of standstill state)
 *
 * ​ Parameters: ​ none
​ *
​ * ​ ​Return: ​ none
 */
void state_machine(void);



#endif /* STATEMACHINE_H_ */
