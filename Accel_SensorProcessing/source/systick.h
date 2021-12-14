/*
 * systick.h
 *
 *  Created on: 05-Dec-2021
 *      Author: Ganesh KM
 */

#ifndef SYSTICK_H_
#define SYSTICK_H_

extern int flag_log;
extern int flag_velocity;

/**
* @brief Defines for the system tick behaviour
*/
typedef uint32_t ticktime_t;



/**
* @brief Function to initialize the SysTick interrupt
*/
void Init_SysTick();

/**
​ * ​ ​ @brief​ ​  This functions returns the time in ms since the power on.
                Max time=0xffffffff ms after that it rolls back to 0.
​ *
​ * ​ ​ @param​ ​ none
​ * ​ ​ @return​ ​ none
​ */
ticktime_t now(); // returns time since startup


/**
​ * ​ ​ @brief​ ​ Resets the Flags and Trans_tick to DEFAULT(0)
 *           Doesn't affect now() values
​ *
​ * ​ ​ @param​ ​ none
​ * ​ ​ @return​ ​ none
​ */
void reset_timer();


/**
​ * ​ ​ @brief​ ​ Returns the number of ticks from reset
​ *
​ * ​ ​ @param​ ​ none
​ * ​ ​ @return​ ​ Integer - Number of Ticks
​ */
ticktime_t get_timer();


#endif /* SYSTICK_H_ */
