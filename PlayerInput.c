/***************************
 * PlayerInput.c
 * Drivers for player input
 *
 * Main fire button - PE0
 * Movement slide pot - PE2
 *
 * Runs on LM4F120/TM4C123
 * coded by Billy.Ljm
 * MIT License
 * 10 May 2014
 ****************************/

#include "PlayerInput.h"
#include "tm4c123gh6pm.h"

void Controls_Init(void){
// Initalise port E
	unsigned long volatile delay;
	SYSCTL_RCGC2_R |= 0x10; 					// activate clock for Port E
	delay = SYSCTL_RCGC2_R; 					// allow clock to start
	GPIO_PORTE_LOCK_R = 0x4C4F434B;   // unlock port E
	GPIO_PORTE_PCTL_R = 0x00000000;   // clear PCTL
	GPIO_PORTE_DIR_R &= ~0x05;        // PE0 & PE2 inputs
  GPIO_PORTE_AMSEL_R &= ~0x01;      // disable analog on PE0
  GPIO_PORTE_AFSEL_R &= ~0x01;      // disable alt funct on PE0
  GPIO_PORTE_DEN_R |= 0x01;         // enable digital I/O on PE0
	GPIO_PORTE_AMSEL_R |= 0x04;     	// enable analog on PE2	
	GPIO_PORTE_AFSEL_R |= 0X04;  			// enable alt funct on PE2
	GPIO_PORTE_DEN_R &= ~0X04;     		// disable digital I/O on PE2

	// Intialise ADC on PE2
	SYSCTL_RCGC0_R |= 0x00010000;   	// activate ADC0
	delay = SYSCTL_RCGC2_R;        		// allow clock to start
	SYSCTL_RCGC0_R &= ~0x00000300; 	 	// 125K samplig rate
	ADC0_SSPRI_R = 0x0123;          	// SS3 = highest priority
	ADC0_ACTSS_R &= ~0x0008;        	// disable SS3
	ADC0_EMUX_R &= ~0xF000;         	// software trigger
	ADC0_SSMUX3_R &= ~0x000F;      	 	// clear SS3 
	ADC0_SSMUX3_R += 1;             	// set channel to Ain1/PE2
	ADC0_SSCTL3_R = 0x0006;         	// no TS0 D0, yes IE0 END0
	ADC0_ACTSS_R |= 0x0008;        	  // enable SS3
}

unsigned long SlidePot(void){  
// Read slide pot (Busy-Wait)
// Returns: 12-bit ADC input (from 0 - 4094) 
	unsigned long result;
	ADC0_PSSI_R = 0x0008;
	while((ADC0_RIS_R&0x08)==0){};
	result = ADC0_SSFIFO3_R&0xFFF;
	ADC0_ISC_R = 0x0008;
	return result;
}
