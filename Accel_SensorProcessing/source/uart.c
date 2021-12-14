/*
 * uart.c
 *  : This file handles the UART module initialization routine and Baud rate selection along with handling Tx and Rx
 *  Created on: 13-Dec-2021
 *      Author: Ganesh KM
 */


#include <LOG_defs.h>
#include "uart.h"
#include "MKL25Z4.h"
#include "assert.h"
#include "sysclock.h"
#include "queue.h"

Q_T TxQ, RxQ;

//Macros for UART Parameters
#define USE_UART_INTERRUPTS 	(0)
#define UART_OVERSAMPLE_RATE 	(16)
#define BUS_CLOCK 				(24e6)
#define SYS_CLOCK				(48e6)

// Macros for handling Critical section
#define START_CRITICAL()	__disable_irq()
#define END_CRITICAL(x)	__set_PRIMASK(x)


int __sys_write(int handle, char* buffer, int count) {
	if(buffer == NULL) {
		return -1;
	}
	while(Q_Full(&TxQ)) {
		; // Wait for the space to openup
	}

	if(Q_Enqueue(&TxQ, buffer, count) != count) {
		return -1;
	}

	if(!(UART0->C2 & UART0_C2_TIE_MASK)) {
		UART0->C2 |= UART0_C2_TIE(1);
	}

	return 0;
}

int __sys_readc(void) {
	char ch;
	if (Q_Dequeue(&RxQ, &ch, 1) != 1){
		return -1;
	}
	return ch;
}


/*
 * Initialization of UART
 * Baud rate : 115200
 *
 * Parameters:
 *   baud_rate: UART_BAUDRATE
 *
 * Returns:
 *   void
 */
void Init_UART0(uint32_t baud_rate) {
	uint16_t BR;

	// Enable PORT A CLOCK AND UART0
	SIM->SCGC4 |= SIM_SCGC4_UART0_MASK;
	SIM->SCGC5 |= SIM_SCGC5_PORTA_MASK;

	// Make sure transmitter and receiver are disabled before init
	UART0->C2 &= ~UART0_C2_TE_MASK & ~UART0_C2_RE_MASK;

	// Clocking UART0 using SIM to 48MHz frequency
	SIM->SOPT2 |= SIM_SOPT2_UART0SRC(1);

	// Configuring MUX(10-8) of PORTA PCR1 & PCR2 with 010 value.
	PORTA->PCR[1] = PORT_PCR_ISF_MASK | PORT_PCR_MUX(2); /* Rx : PTA1 (UART0) */
	PORTA->PCR[2] = PORT_PCR_ISF_MASK | PORT_PCR_MUX(2); // Tx : PTA2 (UART0)

	//section 39.2.1 in the reference manual
	//Selection of Baud rate, Baud clock & oversampling rate(default 16) for UART Communication
	BR = (uint16_t)((SYS_CLOCK)/(baud_rate * UART_OVERSAMPLE_RATE));
	UART0->BDH &= ~UART0_BDH_SBR_MASK;
	UART0->BDH |= UART0_BDH_SBR(BR>>8);//SBR[4:0]
	UART0->BDL = UART0_BDL_SBR(BR); //SBR[7:0]
	UART0->C4 |= UART0_C4_OSR(UART_OVERSAMPLE_RATE-1);

	UART0->BDH |= UART0_BDH_RXEDGIE(0) | UART0_BDH_SBNS(0) | UART0_BDH_LBKDIE(0);// RXEDGIE -set to 0 for polling, SBNS - 0: 1 stop bit, LBKDIE - 0: selected polling
	UART0->C1 = UART0_C1_LOOPS(0) | UART0_C1_M(0) | UART0_C1_PE(0) | UART0_C1_PT(0); //8-bit, No parity
	UART0->C3 = UART0_C3_TXINV(0) | UART0_C3_ORIE(0)| UART0_C3_NEIE(0)| UART0_C3_FEIE(0) | UART0_C3_PEIE(0); // No need to invert since BDH and BDL are within limits.
	UART0->S1 = UART0_S1_OR(1) | UART0_S1_NF(1) | UART0_S1_FE(1) | UART0_S1_PF(1);// Clear error flags
	UART0->S2 = UART0_S2_MSBF(0) | UART0_S2_RXINV(0);	// No need to do reverse since BDH and BDL are within limits.

	Q_Init(&TxQ);//enable Interrupts
	Q_Init(&RxQ);

	NVIC_SetPriority(UART0_IRQn, 2);
	NVIC_ClearPendingIRQ(UART0_IRQn);
	NVIC_EnableIRQ(UART0_IRQn); //Enabling UART0 Interrupt

	UART0->C2 |= UART_C2_RIE(1);//Rx Interrupt enabled.
	UART0->C2 |= UART0_C2_RE(1) | UART0_C2_TE(1);// Enable Tx and Rx of UART
	LOG("\n\r Initializing UART0 ..... ");
	LOG("\n\r Configured BAUD RATE : 115200 ");
	LOG("\n\r --- UART0 Initialization Complete --- ");
}


/*
 * Function to send string over UART using Enqueue
 *
 * Parameters:
 *   str: String to be sent
 *	 count: length of the string
 *
 * Returns:
 *   void
 */
void Send_String(const void* str, size_t count){
	Q_Enqueue(&TxQ, str, count);
	if (!(UART0->C2 & UART0_C2_TIE_MASK)) {
		UART0->C2 |= UART0_C2_TIE(1);//Hardware interrupt requested
	}
}


/*
 * Function to Receive the Data from UART
 *
 * Parameters:
 *   str: String to be received from UART
 *	 count: String length
 *
 * Returns:
 *   void
 */
size_t Receive_String(void* str, size_t count) {
	return Q_Dequeue(&RxQ, str, count);
}


/*
 * Function to clear Receiver over run flag, Noise flag, Framing error flag and parity error flag.
 *
 * Parameters:
 *   void
 * Returns:
 *   void
 */
static void clearUARTErrors(void) {
	UART0->S1 = UART0_S1_OR(1) | UART0_S1_NF(1) | UART0_S1_FE(1) | UART0_S1_PF(1);
}

/*
 * IRQ Handler Function
 *
 * Parameters:
 *   void
 * Returns:
 *   void
 */
void UART0_IRQHandler(void) {

	uint8_t ch;

	if (UART0->S1 & (UART_S1_OR_MASK |UART_S1_NF_MASK | UART_S1_FE_MASK | UART_S1_PF_MASK)) {
		clearUARTErrors();
		ch = UART0->D;
	}

	if (UART0->S1 & UART0_S1_RDRF_MASK) { //Get to receive Input
			ch = UART0->D;
			Q_Enqueue(&RxQ, &ch, 1);
	}

	if ( (UART0->C2 & UART0_C2_TIE_MASK) && (UART0->S1 & UART0_S1_TDRE_MASK) ) {//enable tx interrupt
		if(Q_Dequeue(&TxQ, &ch, 1)) {
			UART0->D = ch;
		}
		else {
			UART0->C2 &= ~UART0_C2_TIE_MASK; //Disable tx interrupt
			}
		}
		}
