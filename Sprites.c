/**************************************
 * Sprites.c
 * Functions to create & manage sprites
 *
 * Runs on LM4F120/TM4C123
 * coded by Billy.Ljm
 * MIT License
 * 10 May 2014
 *************************************/

#include "Sprites.h"
#include "SpriteImages.c"

#define BUNKERMAXHP 30 // Bunker's max hp

sprite Player_Init(unsigned char x, unsigned char y){
// Intialise player's ship sprite center-bottom of screen
// Returns: sprite of player's ship
	sprite player;
	player.x = x;
	player.y = y;
	player.hp = 1;
	player.pt = 0;
	player.image[0] = Player;
	player.explode = Player_Explode;
	player.width = Player[18];
	player.height = Player[22];
	return player;
}

sprite Bunker_Init(unsigned char x, unsigned char y){
// Initialise bunker at (x,y)
// Returns: sprite of bunker
	sprite bunker;
	bunker.x = x;
	bunker.y = y;
	bunker.hp = BUNKERMAXHP;
	bunker.pt = 0;
	bunker.image[0] = Bunker_NoDmg;
	bunker.image[1] = Bunker_LightDmg;
	bunker.image[2] = Bunker_HeavyDmg;
	bunker.width = Bunker_NoDmg[18];
	bunker.height = Bunker_NoDmg[22];
	return bunker;
}

sprite Enemy_Init(unsigned char x, unsigned char y, unsigned char variant){
// Intialise 1 enemy sprite of specified variant at (x,y)
// Returns: sprite of enemy
	sprite enemy;
	enemy.x = x;
	enemy.y = y;
	enemy.hp = 1;
	enemy.pt = 1;
	if(variant == 0){ // pick appropraite variant
		enemy.image[0] = Enemy1_pointA;
		enemy.image[1] = Enemy1_pointB;
	} else if(variant == 1){
		enemy.image[0] = Enemy2_pointA;
		enemy.image[1] = Enemy2_pointB;
	} else if(variant == 2){
		enemy.image[0] = Enemy3_pointA;
		enemy.image[1] = Enemy3_pointB;
	} else{
		enemy.image[0] = Enemy4_pointA;
		enemy.image[1] = Enemy4_pointB;
	}
	enemy.explode = Enemy_Explode;
	enemy.width = Enemy_Explode[18];
	enemy.height = Enemy_Explode[22];
	return enemy;
}

sprite UFO_Init(unsigned char x, unsigned char y){
// Intialise a UFO at top-left corner of screen
// Returns: sprite of UFO
	sprite UFO;
	UFO.x = x;
	UFO.y = y;
	UFO.hp = 1;
	UFO.pt = 10;
	UFO.image[0] = UFO_pointA;
	UFO.image[1] = UFO_pointB;
	UFO.image[2] = UFO_pointC;
	UFO.explode = UFO_Explode;
	UFO.width = UFO_pointA[18];
	UFO.height = UFO_pointA[22];
	return UFO;
}

sprite ShootLaser(sprite* spr, unsigned char updir){
// Initialise laser sprite at center of sprite 
// direction = 0 -> downward laser, direction = 1 -> upwards laser
// Returns: sprite of laser
	sprite laser;
	laser.width = Laser[18];
	laser.height = Laser[22];
	laser.x = spr->x + spr->width / 2 - 1;
	laser.y = updir == 0 ? spr->y + laser.height : spr->y - spr->height;
	if(updir == 0)
		laser.y = spr->y + laser.height - 1;
	else
		laser.y = spr->y - spr->height;
	laser.hp = 1;
	laser.pt = 0;
	laser.image[0] = Laser;
	return laser;
}

unsigned char CrashCheck(sprite* spr1, sprite* spr2){
// Checks if 2 sprites have collided
// returns: 1 if sprites crash, 0 otherwise
	if((((spr1->x > spr2->x) && (spr1->x - spr2->x < spr2->width)) || // spr1 collides w/ spr2 on left
		 ((spr1->x < spr2->x) && (spr2->x - spr1->x < spr1->width))) && // spr1 collies w/ spr2 on right
	   (((spr1->y > spr2->y) && (spr1->y - spr2->y < spr2->height)) || // spr1 collides w/ spr2 on top
		 ((spr1->y < spr2->y) && (spr2->y - spr1->y < spr1->height)))) // spr1 collides w/ spr2 on bottom
		return 1;
	else
		return 0;
}

