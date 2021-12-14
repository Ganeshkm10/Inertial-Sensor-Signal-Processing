/*
 * statemachine.c
 *
 *  Created on: 05-Dec-2021
 *      Author: Ganesh KM
 */

#include <LOG_defs.h>
#include "bme.h"
#include "assert.h"
#include "systick.h"
#include "delay.h"
#include "math.h"

#include "i2c.h"
#include "i2carbiter.h"
#include "led.h"
#include "mma8451q.h"
#include "statemachine.h"

/* Flag for breaking between transition */
extern volatile uint8_t flag;

/* Enum definition for statemachine*/
typedef enum {
	s_ORIENTATION,
	s_ACCELERATION
} state_t;

/* Timeout Period for flashing in case of jerk detection */
#define ACCEL_TIMEOUT 7500

#define MMA_ISR_PIN 14

/* Structure for State Handling */
struct mma_state_t{
	state_t state;
} mma_t  = {
	.state = s_ORIENTATION
};

static volatile uint8_t poll_mma8451q = 1;
volatile uint8_t flag;


/* Function for PORTA Interrupt handler
 *
 * Parameter : None
 *
 * Return : None
 *
 */
void PORTA_IRQHandler()
{
   register uint32_t isfr_mma = MMA8451Q_INT_PORT->ISFR;
   register uint32_t fromMMA8451Q 	= (isfr_mma & ((1 << MMA8451Q_INT1_PIN) | (1 << MMA8451Q_INT2_PIN)));

		if (fromMMA8451Q) {
		PORTA->PCR[MMA_ISR_PIN] |= PORT_PCR_ISF_MASK; //No accel is detected
		uint8_t Int_SourceTrans = I2C_ReadRegister(MMA8451Q_I2CADDR, 0x16);
		if(Int_SourceTrans == 0){
			flag = 1;
		}
		PORTA->ISFR |= (1 << MMA8451Q_INT1_PIN) | (1 << MMA8451Q_INT2_PIN);
	}
		flag = 1;
}


/**
 * state machine function to transit between orientation state and acceleration state​
 * Initial state : Orientation (because of standstill state)
 *
 * ​ Parameters: ​ none
​ *
​ * ​ ​Return: ​ none
 */
void state_machine(void) {

	state_t new_state = mma_t.state;
	mma8451q_acc_t acc;
	MMA8451Q_InitializeData(&acc);
	int readMMA;
	LOG("\n\r");
	LOG("\n\r Entering State Machine");
	LOG("\n\r Initializing Inertial Sensor State Machine");
	LOG("\n\r There are only 2 states");
	LOG("\n\r 1. ORIENTATION STATE ");
	LOG("\n\r 2. ACCELERATION STATE ");
	while(1) {
		switch(new_state) {
		case s_ORIENTATION:

			reset_timer();
			flag = 0;
			LOG("\n\r");
			LOG("\n\r CURRENT STATE --> ORIENTATION STATE ");
			LOG("\n\r Description : System is in standstill, Pitch and Roll is displayed based on the orientation ");
			while(flag != 1) {
				LED_RedOn();
				readMMA = 1;
				if (readMMA) {
					Control_RGB_LEDs(&acc);
				}
			}
			if(flag == 1) { //if sudden acceleration is detected, change to new state
				new_state = s_ACCELERATION;
			}
			break;

		case s_ACCELERATION: // Jerk Detected State

			reset_timer();
			LOG("\n\r");
			LOG("\n\r CURRENT STATE --> ACCELARATION STATE ");
			LOG("\n\r Description : Sudden acceleration detected, LEDs flashing");
			while(get_timer() < ACCEL_TIMEOUT) { // LEDs flashing until Parameter timeout
				Control_RGB_LEDs(&acc);
				delay_ms(100);
				GREEN_PWM = 0;
				BLUE_PWM = 0;
				delay_ms(100);
			}
			flag = 0;
			new_state = s_ORIENTATION;
		}
	}

}
