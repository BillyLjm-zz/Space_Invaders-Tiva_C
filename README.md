#Space Invaders (Survival Mode)

**
Done as lab 15 of  
UT.6.01x Embedded Systems  
Runs on LM4F120/TM4C123
**

## Requirements
1. Movement slide potentiometer - PE2/AIN1  
2. Main fire button -  PE0  
3. DAC - PB0,PB1,PB2,PB3  
5. Nokia 5510: (optional)  
   SSI0Clk - PA2  
   SSI0Fss - PA3  
   SSI0Tx - PA5  
   Data/Command - PA6  
   Reset - PA7   


## Links
- [YouTube Demo](http://youtu.be/88ZHcAEK1Pw)


## What It Does
Space Invaders in **ENDLESS SURVIVAL MODE**!  
Fend off 4x2 enemy hordes for as long as you can.  
**Enemy** hordes move **faster** & **fire more** frequently every level.  
**Bunkers** also eventually get **destroyed**, so no more cover for you!  


## Code Features
Easily change many features:  
- **Change difficulty curve** (via `enemyShootFn` & `EnemySpeedFn`)  
- **GamePlay** (via `ENEMYROWS`, `ENEMYCOLS`, `UFOSPAWNRATE` & `BUNKERNUM`)  
- **Screen layout** (via `UFOH`, `PLAYERH` & `BUNKERH`)  
- **Graphic adjustments** (via `SpriteImage.c`, `ENEMYGAPX` & `ENEMYGAPY`)  
- **Easily configure for larger screen** (via `MAXSCREENY`, `MINSCREENX`, etc)  

## Files
`SpaceInvaders.h` - main game function  
`Sprites.h` - Sprite functions  
`SpriteImages.c` - Sprite image data files  
`SoundDriver.h` - Sound ouput driver functions  
`SoundFiles.c` - Sound data files  
`PlayerInput.h` - Player input drivers (fire button & potentiometer)  
`images` - Sprite images in .bmp format  
`random.h` - Random number generator function  
`Nokia5110.h` - Nokia5110 LCD driver functions  
`VirtualNokia5110.h` - Virtual screen driver functions (requires `TExaSdisplay.exe`)  
`tm4c123gh6pm.h` - Launchpad's functions  
`BmpConvert.exe` - Converts .bmp to array format. Array can then be inserte into `SpriteImages.c`  
