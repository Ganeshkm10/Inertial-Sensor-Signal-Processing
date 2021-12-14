/*
 * led.c
 *
 *  Created on: 05-Dec-2021
 *      Author: Ganesh KM
 */
#include <LOG_defs.h>
#include "led.h"
#include "MKL25Z4.h"
#include "delay.h"
#include "systick.h"

#define XTAL_FREQ			(8000000u) 	/* Hz, FRDM-KL25Z has an on-board 8 MHz xtal */
#define XTAL_PEE_DIVIDE		(4u)		/* divide by 4 (8 MHz --> 2 MHz) */
#define XTAL_PEE_UPSCALE	(24u)		/* scale up by 24 (2 MHz --> 48 MHz) */

#define CORE_CLOCK			(XTAL_FREQ/XTAL_PEE_DIVIDE*XTAL_PEE_UPSCALE) /* Hz */

#define PWM_PERIOD (48000)	/* PWM Brighness Period */
#define FULL_ON (PWM_PERIOD-1)
#define FULL_OFF (0)

int flag_log = 0;

void Init_LED() {
	LOG("\r\n");
	LOG("\n\r LEDs Initializing .... ");

	//Enable clock for PORTB and PORTD in Bitfield 10 & 12 in System Clock Gating Control Register 5 (SIM_SCGC5)
		SIM->SCGC5 |= SIM_SCGC5_PORTB_MASK | SIM_SCGC5_PORTD_MASK;;

		/*Configure RGB LEDs */
		PORTB->PCR[RED_LED_POS] &= ~PORT_PCR_MUX_MASK;
		PORTB->PCR[RED_LED_POS] |= PORT_PCR_MUX(3);
		PORTB->PCR[GREEN_LED_POS] &= ~PORT_PCR_MUX_MASK;
		PORTB->PCR[GREEN_LED_POS] |= PORT_PCR_MUX(3);
		PORTD->PCR[BLUE_LED_POS] &= ~PORT_PCR_MUX_MASK;
		PORTD->PCR[BLUE_LED_POS] |= PORT_PCR_MUX(4);

		PTB->PDDR |= MASK(RED_LED_POS) | MASK(GREEN_LED_POS);
		PTD->PDDR |= MASK(BLUE_LED_POS);

		RED_PWM,GREEN_PWM,BLUE_PWM = 0;
     LOG("\n\r --- LEDs Initialization is Complete --- ");
}

void Init_TPM() {
	LOG("\r\n");
	LOG("\n\r PWM Initializing .... ");
	// CLock Gating TPM
	SIM->SCGC6 |= SIM_SCGC6_TPM0_MASK | SIM_SCGC6_TPM2_MASK;

	//set clock source for tpm: 48 MHz
	SIM->SOPT2 |= SIM_SOPT2_TPMSRC(1);

	//load the counter and mod
	TPM0->MOD = PWM_PERIOD-1;
	TPM2->MOD = PWM_PERIOD-1;

	// Prescalar set to 1, no division
	TPM0->SC |= (TPM_SC_CPWMS(0)| TPM_SC_CMOD(1));
	TPM2->SC |= (TPM_SC_CPWMS(0)| TPM_SC_CMOD(1));

	// Set channel 1 to edge-aligned low-true PWM
	TPM0->CONTROLS[1].CnSC = TPM_CnSC_MSB_MASK | TPM_CnSC_ELSA_MASK;
	TPM2->CONTROLS[1].CnSC = TPM_CnSC_MSB_MASK | TPM_CnSC_ELSA_MASK;
	TPM2->CONTROLS[0].CnSC = TPM_CnSC_MSB_MASK | TPM_CnSC_ELSA_MASK;

	// Clock prescaler is 7 (PWM clock devided by 128)
	// This makes PWM clock as 48000000/128 = 375000 Hz (375Khz)
	TPM0->SC |= TPM_SC_PS(7);
	TPM2->SC |= TPM_SC_PS(7);

	// Setting Initial Duty cycle to 0
	TPM2->CONTROLS[0].CnV = 0;
	TPM2->CONTROLS[1].CnV = 0;
	TPM0->CONTROLS[1].CnV = 0;

	LOG("\n\r --- PWM Initialization Complete --- ");

}



void LED_Red()
{
	LED_RedOn();
	LED_GreenOff();
	LED_BlueOff();
}

void LED_Green()
{
	LED_RedOff();
	LED_GreenOn();
	LED_BlueOff();
}

void LED_Blue()
{
	LED_RedOff();
	LED_GreenOn();
	LED_BlueOff();
}


void LED_White()
{
	/* Turn on All LEd */
	LED_RedOn();
	LED_GreenOn();
	LED_BlueOn();
}

void LED_Off()
{
	/* Turn off LEd */
	LED_RedOff();
	LED_GreenOff();
	LED_BlueOff();
}

void Led_Down()
{
	/* Turn off all Led's to counter noisy
	 * readings */
	LED_Off();
	LED_Off();
	LED_Off();
	LED_Off();
}

void DoubleFlash()
{
	/* Double Flash all Leds to begin*/
	for(int i =0; i<2; i++) {
		LED_White();
		delay_ms(50);
		Led_Down();
		delay_ms(50);
		LED_White();
		delay_ms(50);
		Led_Down();
	}


	Led_Down();
	delay_ms(50);
}

void Control_RGB_LEDs(mma8451q_acc_t *acc) {

	// Initialize few variable
	float roll = 0.0,  pitch = 0.0;
	int PWM_Green=0, PWM_Blue = 0;
	float new_Accelvalue_ax=0.0,new_Accelvalue_ay=0.0,new_Accelvalue_az=0.0;
	int skid_detected = 0;
	int velocity=0;

	// Read Accletation Data from MMA8451Q
	read_full_xyz(acc);
	//LOG("\r\n acc: %d  ", acc);
	get_axValue(acc, &new_Accelvalue_ax);
	get_ayValue(acc, &new_Accelvalue_ay);
	get_azValue(acc, &new_Accelvalue_az);
	velocitycalulation(new_Accelvalue_ax, &velocity);
	skiddetection(new_Accelvalue_ax, &skid_detected);

	// Convert acc to Roll and Pitch
	convert_xyz_to_roll_pitch(acc, &roll, &pitch);

	/* Convert 0-90 degree (Pitch and Roll) to PWM
	 * Range of (0-48000) */
	PWM_Green = (((int)roll * NEWRANGE ) / OLDRANGE);
	PWM_Blue = (((int)pitch * NEWRANGE ) / OLDRANGE);

	/* Orientation can be negative
	 * That should not concern LED lighting*/
	if(PWM_Blue < 533) {
		PWM_Blue = PWM_Blue *-1;
	}

	// Necessary to prevent loopback in atan2
	if((int) pitch > 80) {
		PWM_Green = 0;
	}

	// Set the PWM to appropriate brightness
	GREEN_PWM = PWM_Green;
	BLUE_PWM = PWM_Blue;

//	// Debug Prints of Roll and Pitch
	if(flag_log == 1) {
		LOG("\r\n roll: %d , pitch: %d ", (int)roll, (int)pitch);
		printf("\r\n ax: %d",(int)new_Accelvalue_ax);
		printf("\r\n ay: %d",(int)new_Accelvalue_ay);
		printf("\r\n az: %d",(int)new_Accelvalue_az);
		if(skid_detected==1)
		{
			printf("\r\n Vehicle is skidding, activate abs");
		}
		printf("\r\n velocity: %d",velocity);

		flag_log = 0;
	}
}

