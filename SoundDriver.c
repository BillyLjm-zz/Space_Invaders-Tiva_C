/****************************************************
 * SoundDriver.c
 * Functions to play sounds for Space Invader games
 * Requires DAC on PB3-PB0 & timer0
 *
 * Runs on LM4F120/TM4C123
 * coded by Billy.Ljm
 * MIT License
 * 10 May 2014
 ****************************************************/

#include "SoundDriver.h"

#include "tm4c123gh6pm.h"
#include "SoundFiles.c"

const unsigned char* CurrSound = 0;
unsigned long SoundIndex = 0;
unsigned long SoundCount = 0;

void DAC_Init(void){
	// Initialise DAC (PB3-PB0)
	unsigned long volatile delay;
	SYSCTL_RCGC2_R |= 0x02; 					// activate port B
  delay = SYSCTL_RCGC2_R;
  GPIO_PORTB_DIR_R |= 0X0F;     		// make PB0-PB5 out
	GPIO_PORTB_AFSEL_R &= ~0X0F;  		// disable alt funct on PB0-PB5
	GPIO_PORTB_DEN_R |= 0X0F;     		// enable digital I/O on PB0-PB5
	GPIO_PORTB_AMSEL_R &= ~0x20;      // no analog
  GPIO_PORTB_PCTL_R = 0x00; 			// regular function
	GPIO_PORTB_DR8R_R |= 0x0F;				// drive up to 8mA out
}

void Timer0_Init(void){
// Initialise timer0 (11.025kHz)
	unsigned long volatile delay;
	SYSCTL_RCGCTIMER_R |= 0x01;
	delay = SYSCTL_RCGCTIMER_R;
	TIMER0_CTL_R = 0x00000000;
	TIMER0_CFG_R = 0x00000000; // 32-bit mode
	TIMER0_TAMR_R = 0x00000002; // periodic mode, default down-count
	TIMER0_TAILR_R = 7255; // 11.025 kHz
	TIMER0_TAPR_R = 0; // bus clock resolution
	TIMER0_ICR_R = 0x00000001;
	TIMER0_IMR_R = 0x00000001;
	NVIC_PRI4_R &= (NVIC_PRI4_R&0x0FFFFFFF)|(0 << 29); // priority 3
	NVIC_EN0_R |= 1<<19;
	TIMER0_CTL_R = 0x00000000; // disable Timer0
}

void Timer0A_Handler(void){
// Play sound in CurrSound 
	TIMER0_ICR_R = 0x00000001;
  if(SoundCount > 0){ 
    GPIO_PORTB_DATA_R = CurrSound[SoundIndex]>>4; // DAC out
    SoundIndex += 1;
    SoundCount -= 1;
  }
	else // If sound ends, disable timer0 & stop sound
		TIMER0_CTL_R = 0x00000000;
}

void PlaySound(const unsigned char *sound, unsigned long length){
// Enables timer0 & set timer0 to output sound
  CurrSound = sound;
  SoundIndex = 0;
  SoundCount = length;
	TIMER0_CTL_R = 0x00000001; // enable Timer0
}

void Sound_UFO(void){
// UFO sound
  PlaySound(UFO_Sound,1802);
}

void Sound_Shoot(void){
// Shooting sound
  PlaySound(shoot,4080);
}

void Sound_Hit(void){
// Enemy explosion
  PlaySound(hit,3377);
}

void Sound_GameOver(void){
// Player explosion
  PlaySound(gameOver,2000);
}

void Sound_Tempo(unsigned char var){
// Plays 4-beat tempo sound
	if(var == 0)
		PlaySound(tempo0,982);
	else if(var == 1)
		PlaySound(tempo1,1042);
	else if(var == 2)
		PlaySound(tempo2,1054);
	else
		PlaySound(tempo3,1098);
}
