/*
 * sysclock.c
 *
 *  Created on: 05-Dec-2021
 *      Author: Ganesh KM
 *
 *   This file deals with the Multipurpose clock generator module initialization
 */

#include "MKL25Z4.h"
#include "sysclock.h"

#define SYSCLOCK_FREQUENCY (48000000U)

void Init_SysClock()
{
	/* selecting clock source for MCGOUTCLK
	 Bit fields (7-6) in MCG Control 1 Register (MCG_C1) */
	MCG->C1 |= MCG_C1_CLKS(0);  //Encoding 0 selects output of FLL or PLL

	//  Bit field (2) in MCG Control 1 Register (MCG_C1)
	MCG->C1 |= MCG_C1_IREFS(1); // Selects the reference clock source for FLL

    // Bit fields (6-5) in MCG Control 4 Register (MCG_C4)
	MCG->C4 |= MCG_C4_DRST_DRS(1); // DCO Range Select - Mid range (01)

	// Bit fields (7) in MCG Control 4 Register (MCG_C4)
	MCG->C4 |= MCG_C4_DMX32(1);    //  DMX32 bit selects DCO frequency : 48MHZ
}
