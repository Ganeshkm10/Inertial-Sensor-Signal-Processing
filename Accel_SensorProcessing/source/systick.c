/*
 * systick.c
 *
 *  Created on: 05-Dec-2021
 *      Author: Ganesh KM
 */

#include <LOG_defs.h>
#include "MKL25Z4.h"
#include "sysclock.h"
#include "systick.h"

#define SYSTICK_FREQUENCY		(4000u) /* Hz */

#define SYSTEM_CLOCK_FREQ      48000000UL  // 48 Mhz
#define SYTICK_TIME_FREQ       (4000U) // 1000 Khz
#define SYSTICK_TMR_RELOAD_VAL ((SYSTEM_CLOCK_FREQ / SYTICK_TIME_FREQ) - 1UL) // 48000 - 1
/**
 * @brief The system tick counter
 */
volatile uint32_t SystemMilliseconds = 0;
volatile ticktime_t trans_tick;
volatile ticktime_t Timer_U32;

volatile ticktime_t g_program_start;
volatile ticktime_t g_timer_start;

/**
 * @brief 250µs Counter
 */
static uint32_t freeRunner = 0;


/**
​ * ​ ​ @Description: Initialization of Systick Timer
​ *
​ * ​ ​ @param​ ​ none
​ * ​ ​ @return​ ​ none
​ */
void Init_SysTick() {
	LOG("\r\n");
	LOG("\n\r Systick Initializing....... ");
	SysTick->LOAD = (SYSTICK_TMR_RELOAD_VAL);  // 1000 Hz
	NVIC_SetPriority(SysTick_IRQn, 3); // Set the interrupt priority to least urgency
	NVIC_ClearPendingIRQ(SysTick_IRQn); // Clear Pending IRq's
	NVIC_EnableIRQ(SysTick_IRQn);
	SysTick->VAL = 0; // Clear the current value register
	SysTick->CTRL = SysTick_CTRL_TICKINT_Msk | SysTick_CTRL_ENABLE_Msk | SysTick_CTRL_CLKSOURCE_Msk ;  // Enable SysTick, Enable SysTick Exceptions, Use CPU Clock

	Timer_U32 = 0; // Variable initialization
	g_program_start = g_timer_start = 0;
	LOG("\n\r --- Systick Initialization Complete --- ");
}

/**
​ * ​ ​ @brief​ ​  This functions returns the time in ms since the power on.
                Max time=0xffffffff ms after that it rolls back to 0.
​ *
​ * ​ ​ @param​ ​ none
​ * ​ ​ @return​ ​ none
​ */
ticktime_t now() {
	return Timer_U32 - g_program_start;
}


/**
​ * ​ ​ @brief​ ​ Resets the Flags and Trans_tick to DEFAULT(0)
​ *
​ * ​ ​ @param​ ​ none
​ * ​ ​ @return​ ​ none
​ */
void reset_timer() {
	g_timer_start = Timer_U32;
}


/**
​ * ​ ​ @brief​ ​ Returns the number of ticks from reset
​ *
​ * ​ ​ @param​ ​ none
​ * ​ ​ @return​ ​ Integer - Number of Ticks
​ */
ticktime_t get_timer() {

	return (Timer_U32 - g_timer_start);
}


/**
​ * ​ ​ @brief​ ​  TIRQ Handler to Update Time.
​ *
​ * ​ ​ @param​ ​ none
​ * ​ ​ @return​ ​ none
​ */
void SysTick_Handler() {
	SystemMilliseconds += ((++freeRunner) & 0b100) >> 2;
	freeRunner &= 0b11;

	Timer_U32++; // Keep Track of the total timer

	if(Timer_U32 % 1000 == 0) {
		flag_log = 1;
		flag_velocity=1;
	}
	else {
		flag_log = 0;
		flag_velocity=0;
	}
}

