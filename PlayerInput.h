/***************************
 * PlayerInput.h
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
 
#ifndef	__PLAYERINPUT_INCLUDED__
#define __PLAYERINPUT_INCLUDED__
 
// main fire button (PE0)
#define FIRE (*((volatile unsigned long *)0x40024004))

// Initalise port E for config specified above
void Controls_Init(void);

// Read slide pot (Busy-Wait)
// Returns: 12-bit ADC input (from 0 - 4094) 
unsigned long SlidePot(void); 

#endif
