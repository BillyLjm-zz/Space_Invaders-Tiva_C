/*****************************************
 * Space Invaders
 * A Space Invaders game
 * Standard enemy horde dimensions for every level
 * Enemies move faster & fire more frequently every level
 *
 * Runs on LM4F120/TM4C123
 * coded by Billy.Ljm
 * MIT License
 * 10 May 2014
 *****************************************

   Required Hardware Connections
   -----------------------------
1. Movement slide potentiometer - PE2/AIN1 
2. Main fire button -  PE0
3. DAC - PB0,PB1,PB2,PB3 
4. Nokia 5510:
   SSI0Clk - PA2
   SSI0Fss - PA3
   SSI0Tx - PA5
   Data/Command - PA6
   Reset - PA7                         */

// SysTick_Init causes virtual display to stop working
// Debug still works

#include "tm4c123gh6pm.h"
#include "Nokia5110.h"
#include "Random.h"
#include "TExaS.h"

#include "SoundDriver.h"
#include "PlayerInput.h"
#include "Sprites.h"

/*** DEFINITIONS ***/
#define UFOH 7 // height of UFO's row (will be at top)
#define PLAYERH 8 // height of player ship's row (will be at bottom)
#define BUNKERH 5 // height of bunker's row (will be right above player's ship row)

#define MAXFPS 15 // maximum Frames per second 
#define UFOSPAWNRATE 1 // Spawn rate of UFO (in seconds)

#define MINSCREENY 2 // Minimum screen y-coord (top)
#define MAXSCREENY 47 // Maximum screen y-coord (bottom)
#define EFFSCREENH (MAXSCREENY - MINSCREENY) // effective screen height
#define MINSCREENX 0 // Minimum screen x-coord (left)
#define MAXSCREENX 82 // Maximum screen x-coord (right)
#define EFFSCREENW (MAXSCREENX - MINSCREENX) // effective screen width

#define ENEMYGAPX 0 // horizontal gap (in px) between adjacent enemy sprites
#define ENEMYGAPY 1 // vertical gap (in px) between adjacent enemy sprites
#define ENEMYROWS 2 // number of rows in enemy horde
#define ENEMYCOLS 4 // number of columns in enemy horde
#define ENEMYNUM (ENEMYROWS * ENEMYCOLS) // number of enemies in horde

#define BUNKERNUM 3 // max number of bunkers

/*** Global variables ***/
unsigned long score;
unsigned int level;

unsigned long enemySpeed; // Delay (in 100ms) between enemy movements
unsigned long enemyShoot; // Each enemy's chance of shotting = enemyShoot/ENEMYSHOOTDIV
#define ENEMYSHOOTDIV 1000

sprite playerSpr, UFOSpr, bunkerSprs[BUNKERNUM], enemySprs[ENEMYNUM]; // sprites
sprite playerBullet, enemyBullets[ENEMYCOLS]; // bullet sprites

unsigned char screenRefreshFlag;
unsigned char enemyMove; // 0 => enemies move right, 1 => move left
unsigned char timer; // to alternate enemy images per step
#define TIMERMAX 12 // max of timer (will be lowest common multipler of enemies' number of images)

unsigned char lvlCleared; // bool to indicate if level cleared (systick marks for main)
sprite* toExplode; // marks dead sprites to explode (systick marks for main)

/*** Helper Function Definitions ***/
void DisableInterrupts(void); // Disable interrupts
void EnableInterrupts(void);  // Enable interrupts

void TitleScreen(void); // Title screen
void GameOver(void); // Game over screen
void SysTick_Init(unsigned long period); // screen refresh & player input
void Timer1_Init(unsigned char seconds); // UFO creation
void Timer2_Init(unsigned long freq); // Laser Movement
void PrintSpr(sprite* spr, unsigned char var); // Print specified variant of sprite's image
void ExplodeSpr(sprite* spr); // Print sprite's explosion
void PrintBunker(sprite* bunker); // Print appropriate variant of bunker
void GameInit(void); // Init player & bunker
void EnemyHordeInit(void); // Init a enemy horde
void SprCrash(sprite* bullet, sprite* spr, unsigned char explodeFlag); // Checks collision & update
void Delay100ms(unsigned long count); // Delay for count*100ms (excluding interrupts)

/*** Level Increment Functions ***/
// Edit to change learning curve
void EnemyShootFn(void){
// How `enemyShoot` changes after every level
// Higher value = enemies shoot more
	if(level == 1)
		enemyShoot = 20;
	else if (level < 9)
		enemyShoot += 20;
	else
		enemyShoot += 100 / level;
}

void EnemySpeedFn(){
// How `enemySpeed` changes after every level
// Higher value = enemies move faster
	if(level == 1)
		enemySpeed = 5000;
	else
		enemySpeed -= 500 / level;
}

/*** Main Functions ***/
int main(void){
	unsigned char i;
	
	TExaS_Init(NoLCD_NoScope);  // set system clock to 80 MHz
	Random_Init(1);
	Nokia5110_Init();	
	Controls_Init();
	DAC_Init();
	EnableInterrupts(); 
	
	// Update screen right after systick triggers
	while(1){ 
		Timer0_Init();
		Timer1_Init(MAXFPS);
		Timer2_Init(UFOSPAWNRATE);
		
		GameInit();
		screenRefreshFlag = 1;
		lvlCleared = 1;
		
		TitleScreen();
		
		while(playerSpr.hp > 0){
			while(screenRefreshFlag == 0){}
			
			if(lvlCleared){
				level += 1;
				EnemyHordeInit();
				EnemyShootFn();
				EnemySpeedFn();
				SysTick_Init(enemySpeed);
				lvlCleared = 0;
			}
				
			// UPDATE SCREEN
			Nokia5110_ClearBuffer();
			
			if(playerSpr.hp > 0) // Player ship
				PrintSpr(&playerSpr,0);
			if(playerBullet.hp > 0) // Player bullets
				PrintSpr(&playerBullet,0);
			for(i = 0; i < BUNKERNUM; i++){ // Bunkers
				PrintBunker(&bunkerSprs[i]);
			} for(i = 0; i < ENEMYNUM; i++){ // Enemies
				if(enemySprs[i].hp > 0)
					PrintSpr(&enemySprs[i], timer % 2);
			} for(i = 0; i < ENEMYCOLS; i++){ // Enemy bullets
				if(enemyBullets[i].hp > 0)
					PrintSpr(&enemyBullets[i], 0);
			} if(UFOSpr.hp > 0) // UFO
				PrintSpr(&UFOSpr, timer % 3);
			if(toExplode != 0){ // Explosions
				ExplodeSpr(toExplode);
				toExplode = 0;
			}
			
			Nokia5110_DisplayBuffer();
			screenRefreshFlag = 0;
		}

		GameOver();
	}
}
void SysTick_Handler(void){
// Update game (move enemies, enemies shoot, collision check, etc.)
  unsigned char i, j;

	// ENEMIES
	for(i = 0; i < ENEMYNUM; i++){ //-- Move
		if(enemySprs[i].hp > 0){ 
			enemySprs[i].x += enemyMove == 0 ? 1 : -1;
			PrintSpr(&enemySprs[i], timer%2);
			Sound_Tempo(timer%4);
		}
	} 
	for(i = 0; i < ENEMYCOLS; i++){ // Shoot 
		if((enemyBullets[i].hp == 0) && // 1 bullet per row
			(((Random()) % ENEMYSHOOTDIV) < enemyShoot)){ // random probability
			for(j = 0; j < ENEMYROWS; j++){ // find front sprite & fire laser
				if(enemySprs[j * ENEMYROWS + i].hp != 0){
					enemyBullets[i] = ShootLaser(&enemySprs[j * ENEMYROWS + i], 0);
					Sound_Shoot();
					break;
				}
			}
		}
	}
	
	// UFO
	if(UFOSpr.x > SCREENW - UFOSpr.width){ // Out of screen
		UFOSpr.hp = 0;
	} if(UFOSpr.hp > 0){ // Move
		UFOSpr.x += 2;
		PrintSpr(&UFOSpr, timer%3);
	}
	
	// COLLISION CHECK
	for(j = 0; j < BUNKERNUM; j++) // Player hit bunker
		SprCrash(&playerBullet, &bunkerSprs[j], 0);
	for(i = 0; i < ENEMYNUM; i++){
		SprCrash(&playerBullet, &enemySprs[i], 1); // Player hit enemy
		SprCrash(&enemyBullets[i], &playerSpr, 1); // Enemy hit player
		for(j = 0; j < BUNKERNUM; j++)  // Enemy hit bunker
			SprCrash(&enemyBullets[i], &bunkerSprs[j], 0);
	}
	SprCrash(&playerBullet, &UFOSpr, 1);
	
	// ENEMY DIRECTION
	for(i = 0; i < ENEMYNUM; i++){		
		if(enemySprs[i].hp > 0){
			if(enemySprs[i].x >= SCREENW - enemySprs[i].width) // Enemy collides on right
				enemyMove = 1;
			else if(enemySprs[i].x <= 0) // Enemy colldies on left
				enemyMove = 0;
		}
	}

	// LEVEL CLEARED?
	lvlCleared = 1;
	for(i = 0; i < ENEMYNUM; i++){
		if(enemySprs[i].hp > 0)
			lvlCleared = 0;
	}
	
	timer = (timer + 1) % TIMERMAX;
}

void Timer1A_Handler(void){
// Get player input & mark for refresh screen
	unsigned char i;
	TIMER1_ICR_R = 0x00000001;
	
	// PLAYER
	playerSpr.x = MINSCREENX + SlidePot() * (EFFSCREENW - playerSpr.width - 1) / 4094 + 1; // Move
	if((FIRE && playerBullet.hp == 0) && playerSpr.hp > 0){ // Shoot
		playerBullet = ShootLaser(&playerSpr, 1);
		Sound_Shoot();
	}
	
	// Player bullet
	if(playerBullet.hp != 0){
		if(playerBullet.y <= MINSCREENY + playerBullet.height)
			playerBullet.hp = 0;
		else 
			playerBullet.y -= 1; 
	}
	
	// Enemy bullet
	for(i = 0; i < ENEMYNUM; i++){ 
		if(enemyBullets[i].hp > 0){
			if(enemyBullets[i].y >= MAXSCREENY)
				enemyBullets[i].hp = 0;
			else 
				enemyBullets[i].y += 1;
		}
	}
	
	screenRefreshFlag = 1;
}

void Timer2A_Handler(void){
// Create UFO on top-left
	TIMER2_ICR_R = 0x00000001;
	if(UFOSpr.hp == 0){
		UFOSpr = UFO_Init(0,UFOH);
		Sound_UFO();
	}
}

/*** Helper Function Implementation ***/
void TitleScreen(void){
// Title Screen
	Nokia5110_Clear();
	Nokia5110_SetCursor(3,1);
	Nokia5110_OutString("SPACE"); 
	Nokia5110_SetCursor(2,2);
	Nokia5110_OutString("INVADERS");
	Nokia5110_SetCursor(1,4);
	Nokia5110_OutString("Press Fire");
	Delay100ms(5);	
	while(!FIRE){};
	Delay100ms(5);
}

void GameOver(void){
// Game Over Screen
	Nokia5110_Clear();
	Nokia5110_SetCursor(1,1);
	Nokia5110_OutString("Nice Try,");
	Nokia5110_SetCursor(1,2);
	Nokia5110_OutString("Earthling");
	Nokia5110_SetCursor(0,4);
	Nokia5110_OutString("score:");
	Nokia5110_OutUDec(score);
	Nokia5110_SetCursor(0,5);
	Nokia5110_OutString("level:");
	Nokia5110_OutUDec(level);
	Delay100ms(5);	
	while(!FIRE){};
	Delay100ms(5);
}

void SysTick_Init(unsigned long milliseconds){
// Initalise systick at `freq`Hz (assuming 80Mhz PLL)
	NVIC_ST_CTRL_R = 0; 
	NVIC_ST_RELOAD_R = milliseconds * 80000 - 1;
  NVIC_ST_CURRENT_R = 0;
  NVIC_SYS_PRI3_R |= 0x01 << 29; // priority 1
	NVIC_ST_CTRL_R = 0x07;
}

void Timer1_Init(unsigned char freq){ 
// Initalise Timer1 to trigger every specified seconds(assuming 880Mhz PLL)
  unsigned long volatile delay;
  SYSCTL_RCGCTIMER_R |= 0x02;
  delay = SYSCTL_RCGCTIMER_R;
  TIMER1_CTL_R = 0x00000000;
  TIMER1_CFG_R = 0x00000000; // 32-bit mode
  TIMER1_TAMR_R = 0x00000002; // periodic mode, default down-count
  TIMER1_TAILR_R = 80000000 / freq -1;
  TIMER1_TAPR_R = 0; // bus clock resolution
  TIMER1_ICR_R = 0x00000001;
  TIMER1_IMR_R = 0x00000001;
  NVIC_PRI5_R = (NVIC_PRI5_R&0xFFFF0FFF)|(0x02 <<13); // priority 2
	NVIC_EN0_R |= 1<<21;
	TIMER1_CTL_R = 0x00000001; 
}

void Timer2_Init(unsigned long seconds){ 
// Initalise Timer2 to trigger every few seconds(assuming 880Mhz PLL)
  unsigned long volatile delay;
  SYSCTL_RCGCTIMER_R |= 0x04;
  delay = SYSCTL_RCGCTIMER_R;
  TIMER2_CTL_R = 0x00000000;
  TIMER2_CFG_R = 0x00000000; // 32-bit mode
  TIMER2_TAMR_R = 0x00000002; // periodic mode, default down-count
  TIMER2_TAILR_R = seconds * 80000000 - 1; 
  TIMER2_TAPR_R = 0; // bus clock resolution
  TIMER2_ICR_R = 0x00000001;
  TIMER2_IMR_R = 0x00000001;
  NVIC_PRI5_R = (NVIC_PRI5_R&0x0FFFFFFF)|(0x03 << 29); // priority 3
	NVIC_EN0_R |= 1<<23;
	TIMER2_CTL_R = 0x00000001; 
}


void PrintSpr(sprite* spr, unsigned char var){
// Print specified variant of sprite's image
	Nokia5110_PrintBMP(spr->x, spr->y, spr->image[var],0);
}

void ExplodeSpr(sprite* spr){
// Print sprite's explosion
	Nokia5110_PrintBMP(spr->x, spr->y, spr->explode,0);
}

void PrintBunker(sprite* bunker){
// Print appropraite varaint of bunker
	if(bunker->hp > (20))
		PrintSpr(bunker, 0);
	else if(bunker->hp > (10))
		PrintSpr(bunker, 1);
	else if(bunker->hp > 0)
		PrintSpr(bunker,2 );
}

void GameInit(void){
// Intialise player's ship & bunkers & print them
// PlayerSpr will be at bottom-centre
// BunkerSprs[] will be evenly spaced across screen 
	unsigned char x_coord, y_coord; // coordinates 		
	unsigned char i; // "for" loops
	unsigned char gap; // gap b/w bunkers
	sprite tmp; // to get .width of any sprite
	
	// Reset global variables
	level = 0;
	score = 0;
	enemyMove = 0;
	timer = 0;
	
	// Destroy any other sprites
	playerBullet.hp = 0;
	UFOSpr.hp = 0 ;
	for(i = 0; i < ENEMYNUM; i++){
		enemySprs[i].hp = 0;
		enemyBullets[i].hp = 0;
	}
	
	// Player initialisation
	tmp = Player_Init(0,0);
	playerSpr = Player_Init(MINSCREENX + (EFFSCREENW - tmp.width)/2,MAXSCREENY);
	
	// Bunkers initialisation
	tmp = Bunker_Init(0,0);
	y_coord = MAXSCREENY - PLAYERH;
	gap = (EFFSCREENW - BUNKERNUM * tmp.width) / (BUNKERNUM - 1);
	for(i = 0; i < BUNKERNUM; i++){ // For each bunker
		x_coord = MINSCREENX + i * (tmp.width + gap);
		bunkerSprs[i] = Bunker_Init(x_coord, y_coord);
	}
}

void EnemyHordeInit(void){
// Fill enemySprs[] with enemy horde of ENEMYCOLS & ENEMYROWS
// Enemy type randomly varies between rows
// Nokia5110 displays up to 2 rows & 5 columns
// Indexing increases left to right from front row, then following rows
	unsigned char y_coord, x_coord; // individual enemy coordinates
	unsigned char i, j; // "for" loops
	unsigned char	var; // enemy variant
	sprite enemyT = Enemy_Init(0,0,0); // to get enemy.width
	enemyMove = 0; // enemies spawn on left, move right
	
	// Clear enemySprs[] & screen
	for(i = 0; i < ENEMYNUM; i++){ // for all enemies
		enemySprs[i].hp = 0; 
	}
	
	// Filling enemySprs[] & print
	for(i = 0; i < ENEMYROWS; i++){ // for each row
		y_coord = MINSCREENY + UFOH + (enemyT.height + 1) * (ENEMYROWS - i);	
		var = (Random() >> 24) % 4;	
		for(j = 0; j < ENEMYCOLS; j++){ // for each enemy in row
			x_coord = MINSCREENX + j * (enemyT.width + ENEMYGAPY);											
			enemySprs[i * ENEMYCOLS + j] = Enemy_Init(x_coord, y_coord, var);
		}
	}
}

void SprCrash(sprite* bullet, sprite* spr, unsigned char explodeFlag){
// Checks of bullet hits player/enemy sprites & update
// explodeFlag = 1 -> Destoryed sprites get exploded
	if((bullet->hp > 0 && spr->hp > 0) && CrashCheck(bullet, spr)){
		Sound_Hit();
		bullet->hp -= 1;
		spr->hp -= 1;
		score += spr->pt * level;
		if(explodeFlag == 1 && spr->hp == 0)
			toExplode = spr;
	}
}

void Delay100ms(unsigned long count){
// Delay by count*100ms (excluding interrupts)
	unsigned long volatile time;
  while(count>0){
    time = 727240;  // 0.1sec at 80 MHz
    while(time){
	  	time--;
    }
    count--;
  }
}
