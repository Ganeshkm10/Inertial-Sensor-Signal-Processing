/*
 * Main file
 * Author : Ganesh KM
 * Created on : 04-DEC-2021
 * Description:
 * To implement MMA8451Q , on-board inertial sensor over I2C for orientation and motion detection(acceleration) on KL25Z.
 *  State machine is being implemented for the state orientation and acceleration.
 *  As per the orientation of KL25Z, Corresponding Pitch, Roll values are displayed over UART along with
 *  the corresponding LEDs which changes brightness corresponding to the values (PWM)
 *  Corresponding Ax,Ay,Az are also displayed over UART.
 *  Skid detection function is implemented wherein the Values coming from speed sensor is configured to be always 0
 *  for detection purposes.
 *  Test function for I2C and UART is written to test each module as a part of testing strategy.
 *  Manual testing for MMA8451Q sensor :
 *  1. On Flat surface in standstill position:
 *  Ideal values : Pitch - 0
 *                 Roll  - 0
 *                 Ax,Ay - 0
 *                 Az    - 1 (in terms of g)
 *                 LEDs behavior : Ideally all LEDs should be in OFF state.(SMALL level of brightness can be seen due to uncalibrated sensor)
 *  2. Tilt the board in horizontal direction :
 *  Expected values : Pitch - 0
 *                  Roll - depends on the tilt
 *                  LEDs : Green LED PWM can be seen
 *  3. Tilt  the board in vertical direction :
 *  Expected values : Pitch - depends on the tilt
 *                  Roll - 0
 *                  LEDs : Blue LED PWM can be seen
 *  4. Sudden Acceleration :
 *   Expected behavior : LEDs start flashing
 *
 * Reference : https://github.com/sunsided/frdm-kl25z-marg-fusion/blob/master/frdm-kl25z-acc-uart/Sources/i2c
 *             https://github.com/alexander-g-dean/ESF/blob/master/NXP/Code/Chapter_8/I2C-Demo/src
 *             https://github.com/sunsided/frdm-kl25z-marg-fusion/blob/master/frdm-kl25z-acc-uart/Sources
 */
 
/**
 * @file    Sensor_signal_processing.c
 * @brief   Application entry point.
 */
#include <LOG_defs.h>
#include "MKL25Z4.h"
#include "bme.h"
#include "assert.h"
#include "systick.h"
#include "delay.h"
#include "math.h"
#include "sysclock.h"
#include "i2c.h"
#include "i2carbiter.h"
#include "uart.h"
#include "led.h"
#include "mma8451q.h"
#include "statemachine.h"
#include "test_i2c.h"
#include "test_queue.h"

//Macros
#define UART_BAUDRATE	115200 //UART Baudrate setting
#define I2CARBITER_COUNT 	(1)		//number of slaves connected

i2carbiter_entry_t i2carbiter_entries[I2CARBITER_COUNT];

void Init_I2CArbiter()
{
    // Clock Enable
	SIM->SCGC5 |= SIM_SCGC5_PORTB_MASK | SIM_SCGC5_PORTE_MASK;

    I2CArbiter_PrepareEntry(&i2carbiter_entries[0], MMA8451Q_I2CADDR, MMA8451_SCL, MMA8451Q_I2C_MUX, MMA8451Q_SDA, MMA8451Q_I2C_MUX);
    I2CArbiter_Configure(i2carbiter_entries, I2CARBITER_COUNT);
    LOG("\r\n Interrupt is configured if sudden acceleration is detected ");
}

/*
 * @brief   Application entry point.
 */
int main(void) {

    /* Initialization of each module*/

    Init_SysClock(); //System Clock
    Init_UART0(UART_BAUDRATE); // UART0
   	Init_SysTick(); // Systick timer
   	delay_ms(500);

   	Init_TPM(); //TPM
   	delay_ms(500);

   	Init_LED(); //LED module
   	delay_ms(500);

   	DoubleFlash();
   	Led_Down();

   	I2C_Init(); // I2C Module
   	delay_ms(500);

	Init_I2CArbiter(); //I2C Arbitrator
	delay_ms(500);

	Init_MMA8451Q(); // MMA8451Q accelerometer module
	delay_ms(500);

/* Starting Testing of UART0 and I2C */
    test_queue(); // Testing of UART0
   	test_i2c_mma();//testing I2C
   	delay_ms(500);

	Led_Down();
	/* After every module is initialized control is transferred to state machine */
	state_machine();
    return 0 ;
}
