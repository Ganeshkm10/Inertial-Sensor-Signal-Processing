/*
 * delay.h
 *
 *  Created on: 05-Dec-2021
 *      Author: Ganesh KM
 */

#ifndef DELAY_H_
#define DELAY_H_

#include "MKL25Z4.h"

extern const volatile uint32_t SystemMilliseconds;

static inline uint32_t systemTime()
{
	return SystemMilliseconds;
}

/* delay Function which uses systick for delay */
static inline void delay_ms(const uint16_t ms)
{
	const uint32_t start_ticks = SystemMilliseconds;
	do {
		__DSB();
		__WFI();
	} while((SystemMilliseconds - start_ticks) < ms);
}

#endif /* DELAY_H_ */
