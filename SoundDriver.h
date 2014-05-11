/****************************************************
 * SoundDriver.h
 * Functions to play sounds for Space Invader games
 * Requires DAC on PB3-PB0 & timer0
 *
 * Runs on LM4F120/TM4C123
 * coded by Billy.Ljm
 * MIT License
 * 10 May 2014
 ****************************************************/

#ifndef __SOUNDDRIVER_INCLUDED__
#define __SOUNDDRIVER_INCLUDED__

// Global variables used
extern const unsigned char* CurrSound;
extern unsigned long SoundIndex;
extern unsigned long SoundCount;

// Initialise DAC (PB3-PB0)
void DAC_Init(void);

// Initialise timer0 (11.025kHz)
void Timer0_Init(void);

// Play UFO sound
void Sound_UFO(void);

// Play Shooting sound
void Sound_Shoot(void);

// Hit sound
void Sound_Hit(void);

// Game over sound
void Sound_GameOver(void);

// Play 4-beat tempo sound (make var change from 0 to 3 to 0)
void Sound_Tempo(unsigned char var);

#endif
