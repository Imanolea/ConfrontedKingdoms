#include <gb/gb.h>
#include <rand.h>

extern const unsigned char tileset[];
extern const unsigned char map[];
extern const unsigned char title[];

#define ENEMY_NO 7
#define TILES_SIZE 16

/* key pressed (true) or not (false)
0: left
1: right
2: up
3: down
4: a
5: b
6: start
7: select
 */
UBYTE input[] = {
	0, 0, 0, 0, 0, 0, 0, NULL
};

UBYTE preinput[] = {
	0, 0, 0, 0, 0, 0, 0, NULL
};

UBYTE scrlx;
UBYTE scrly;

BYTE scrlxinc;
BYTE scrlyinc;

UBYTE debug;

UBYTE swordx;
UBYTE swordy;
UBYTE swordframe;
UBYTE swordtimer;

UBYTE odd;

UBYTE heroinvulnerability;

UBYTE spawntimer;
UWORD spawnrate;

UBYTE health;

typedef struct soldier {
	UBYTE x; // x axis position of the sprite
	UBYTE y; // y axis position of the sprite
	UBYTE orientation; // left (0), right (1), up (2), down (3)
	UBYTE state; // null (0), idle (1), walk (2)
	UBYTE frame; // frame number in the tileset
	UBYTE animframe; // frame number in the animation
	UBYTE frametimer; // numbers of iterations before reaching the next frame in the animation
	UBYTE dirtimer;
}soldier_generic;

soldier_generic hero;
soldier_generic enemy[ENEMY_NO];

/* Animations */
/* odd: frame, even: iterations of the frame */
/* 255: end of the animation */

/* Hero */

UBYTE herohorizontalidleanim[] = {
	 12,   5, 255, NULL
};

UBYTE heroupidleanim[] = {
	 28,   5, 255, NULL
};

UBYTE herodownidleanim[] = {
	 4,   5, 255, NULL
};

UBYTE herohorizontalwalkanim[] = {
	 12,   5,  16,   5,  12,   5,  16,   5, 255, NULL
};

UBYTE heroupwalkanim[] = {
	 28,   5,  32,   5,  28,   5,  32,   5, 255, NULL
};

UBYTE herodownwalkanim[] = {
	  4,   5,   8,   5,  4,   5,  8,   5, 255, NULL
};

/* Enemies */

UBYTE enemyhorizontalidleanim[] = {
	 60,   5, 255, NULL
};

UBYTE enemyupidleanim[] = {
	 76,   5, 255, NULL
};

UBYTE enemydownidleanim[] = {
	 52,   5, 255, NULL
};

UBYTE enemyhorizontalwalkanim[] = {
	 60,   5,  64,   5,  60,   5,  64,   5, 255, NULL
};

UBYTE enemyupwalkanim[] = {
	 76,   5,  80,   5,  76,   5,  80,   5, 255, NULL
};

UBYTE enemydownwalkanim[] = {
	 52,   5,  56,   5,  52,   5,  56,   5, 255, NULL
};

void spawnenemy() {
	UBYTE i;
	
	for (i = 0; i != ENEMY_NO; i++) {
		if (enemy[i].state == 0) {
			enemy[i].x = rand() % (256 + scrlx) + 160 + scrlx;
			enemy[i].y = rand() % (256 + scrly) + 144 + scrly;
			enemy[i].state = 2;
			enemy[i].animframe = 0;
			enemy[i].frametimer = 1;
			break;
		}
	}
}

void init() {
	
	UBYTE i = 0;
	
	initarand(20);
	
	wait_vbl_done();
	disable_interrupts();
	
	SPRITES_8x8;
	
	DISPLAY_OFF;
	HIDE_BKG;
	HIDE_WIN;
	HIDE_SPRITES;
	
	set_bkg_data(0, 190, tileset);
	set_win_data(0, 190, tileset);
	set_bkg_tiles(0, 0, 32, 32, map);
	
	set_sprite_data(0, 190, tileset);
	
	odd = 1;
	
	hero.x = 80;
	hero.y = 80;
	hero.orientation = 3;
	hero.state = 1;
	hero.frame = 0;
	hero.frametimer = 1;
	
	health = 3;
	heroinvulnerability = 0;
	
	for (i = 0; i != ENEMY_NO; i++) {
		enemy[i].x = 0;
		enemy[i].y = 0;
		enemy[i].orientation = 3;
		enemy[i].state = 0;
		enemy[i].frame = 0;
		enemy[i].frametimer = 1;
		enemy[i].dirtimer = 20;
	}
	
	move_sprite(37, 8, 20);
	move_sprite(38, 16, 20);
	move_sprite(39, 24, 20);
	
	move_win(0, 144);
	
	debug = 0;
	
	spawnrate = 1024;
	spawntimer = spawnrate;
	
	SHOW_BKG;
	SHOW_WIN;
	SHOW_SPRITES;
	DISPLAY_ON;
	
	enable_interrupts();
	
	spawnenemy();
}

void setinput(UBYTE key) {
	
	preinput[0] = input[0];
	preinput[1] = input[1];
	preinput[2] = input[2];
	preinput[3] = input[3];
	preinput[4] = input[4];
	preinput[5] = input[5];
	preinput[6] = input[6];
	preinput[7] = input[7];
	
	input[0] = key & J_LEFT;
	input[1] = key & J_RIGHT;
	input[2] = key & J_UP;
	input[3] = key & J_DOWN;
	input[4] = key & J_A;
	input[5] = key & J_B;
	input[6] = key & J_START;
	input[7] = key & J_SELECT;
}

void swingsword() {
	if (hero.orientation == 0) {
		swordx = hero.x - 13;
		swordy = hero.y;
		swordframe = 44;
	} else if (hero.orientation == 1) {
		swordx = hero.x + 13;
		swordy = hero.y;
		swordframe = 36;		
	} else if (hero.orientation == 2) {
		swordx = hero.x;
		swordy = hero.y - 13;
		swordframe = 48;		
	} else if (hero.orientation == 3) {
		swordx = hero.x;
		swordy = hero.y + 13;
		swordframe = 40;			
	}
	
	swordtimer = 12;
}

void hurthero() {
	if (!heroinvulnerability) {
		health = health - 1;
		heroinvulnerability = 32;
	}
}

void killenemy(UBYTE enemyno) {
	enemy[enemyno].frame = 84;
	enemy[enemyno].state = 3;
	enemy[enemyno].frametimer = 8;
	debug = debug + 1;
	
	spawnenemy();
}

void inputlogic() {
	UBYTE neworientation = hero.orientation;
	UBYTE newstate = 1;
	
	scrlxinc = 0;
	scrlyinc = 0;
	
	if (input[0]) { // left
		scrlxinc = -1;
		neworientation = 0;
		newstate = 2;
	}
	
	if (input[1]) { // right
		scrlxinc = 1;
		neworientation = 1;
		newstate = 2;
	}
	
	if (input[2]) { // up
		scrlyinc =-1;
		neworientation = 2;
		newstate = 2;
	}
	
	if (input[3]) { // down
		scrlyinc = 1;
		neworientation = 3;
		newstate = 2;
	}
	
	if (input[5]) { // B
		if (!preinput[5]) {
			swingsword();
		}
	}
	
	if (input[6]) { // Star
		if (!preinput[7]) {
			hurthero();
			killenemy(0);
		}
	}
	
	scrlx = scrlx + scrlxinc;
	scrly = scrly + scrlyinc;
	
	hero.orientation = neworientation;
	
	if (newstate != hero.state) {
		hero.animframe = 0;
		hero.frametimer = 1;
		hero.state = newstate;
	}
}

void animhero() {
	
	UBYTE correctframe = 0;
	
	hero.frametimer = hero.frametimer - 1;
	
	if (heroinvulnerability != 0) {	
		heroinvulnerability = heroinvulnerability - 1;
		if (odd == 1) hero.frame = 0;
	}
	
	if (hero.frametimer) return;
	
	while (!correctframe) {
		if (hero.state == 1) {
			if (hero.orientation < 2) {
				hero.frame = herohorizontalidleanim[hero.animframe];
				hero.frametimer = herohorizontalidleanim[hero.animframe + 1];
			} else if (hero.orientation == 2) {
				hero.frame = heroupidleanim[hero.animframe];
				hero.frametimer = heroupidleanim[hero.animframe + 1];
			} else if (hero.orientation == 3) {
				hero.frame = herodownidleanim[hero.animframe];
				hero.frametimer = herodownidleanim[hero.animframe + 1];
			}
		} else if (hero.state == 2) {
			if (hero.orientation < 2) {
				hero.frame = herohorizontalwalkanim[hero.animframe];
				hero.frametimer = herohorizontalwalkanim[hero.animframe + 1];
			} else if (hero.orientation == 2) {
				hero.frame = heroupwalkanim[hero.animframe];
				hero.frametimer = heroupwalkanim[hero.animframe + 1];
			} else if (hero.orientation == 3) {
				hero.frame = herodownwalkanim[hero.animframe];
				hero.frametimer = herodownwalkanim[hero.animframe + 1];
			}
		}
		
		if (hero.frame == 255) {
			hero.animframe = 0;
		} else {
			correctframe = 1;
			hero.animframe = hero.animframe + 2;
		}
	}
}

UBYTE collision(UBYTE x1, UBYTE y1, UBYTE x2, UBYTE y2){
	
	UBYTE disx;
	UBYTE disy;
	
	disx = ((x1 - x2) > (x2 - x1))? x2 - x1: x1 - x2;
	disy = ((y1 - y2) > (y2 - y1))? y2 - y1: y1 - y2;
			
	if(disx < TILES_SIZE && disy < TILES_SIZE) {
		return 1;
	}
	
	return 0;

}

void enemylogic() {
	UBYTE i;
	UBYTE disx;
	UBYTE disy;
	UBYTE neworientation;
	
	for (i = 0; i != ENEMY_NO; i++) {
		
		if (enemy[i].state != 3 && enemy[i].state != 0) {
			
			enemy[i].dirtimer = enemy[i].dirtimer - 1;
			
			if(collision(hero.x, hero.y, enemy[i].x - scrlx, enemy[i].y - scrly)) {
				hurthero();
			}
		
			if (!enemy[i].dirtimer) {
				
				disx = ((hero.x - enemy[i].x + scrlx) > (enemy[i].x - hero.x - scrlx))? enemy[i].x - hero.x - scrlx: hero.x - enemy[i].x + scrlx;
				disy = ((hero.y - enemy[i].y + scrly) > (enemy[i].y - hero.y - scrly))? enemy[i].y - hero.y - scrly: hero.y - enemy[i].y + scrly;
			
				if (disx > disy) {
					neworientation = ((hero.x - enemy[i].x + scrlx) > (enemy[i].x - hero.x - scrlx))? 0: 1;
				} else {
					neworientation = ((hero.y - enemy[i].y + scrly) > (enemy[i].y - hero.y - scrly))? 2: 3;
				}
				
				enemy[i].orientation = neworientation;
				
				enemy[i].dirtimer = 20;
			
			}
			
			if (enemy[i].orientation == 0) {
				enemy[i].x = enemy[i].x - odd;
			} else if (enemy[i].orientation == 1) {
				enemy[i].x = enemy[i].x + odd;			
			} else if (enemy[i].orientation == 2) {
				enemy[i].y = enemy[i].y - odd;			
			} else if (enemy[i].orientation == 3) {
				enemy[i].y = enemy[i].y + odd;			
			}
			
			if (swordframe && collision(swordx, swordy, enemy[i].x - scrlx, enemy[i].y - scrly)) {
				killenemy(i);
			}
		}
		
	}
		
}

void animenemies() {
	UBYTE correctframe;
	UBYTE i;
	
	for (i = 0; i != ENEMY_NO; i++) {
	
		correctframe = 0;
		
		enemy[i].frametimer = enemy[i].frametimer - 1;
		
		if (!enemy[i].frametimer) {
		
			while (!correctframe) {
				if (enemy[i].state == 1) {
					if (enemy[i].orientation < 2) {
						enemy[i].frame = enemyhorizontalidleanim[enemy[i].animframe];
						enemy[i].frametimer = enemyhorizontalidleanim[enemy[i].animframe + 1];
					} else if (enemy[i].orientation == 2) {
						enemy[i].frame = enemyupidleanim[enemy[i].animframe];
						enemy[i].frametimer = enemyupidleanim[enemy[i].animframe + 1];
					} else if (enemy[i].orientation == 3) {
						enemy[i].frame = enemydownidleanim[enemy[i].animframe];
						enemy[i].frametimer = enemydownidleanim[enemy[i].animframe + 1];
					}
				} else if (enemy[i].state == 2) {
					if (enemy[i].orientation < 2) {
						enemy[i].frame = enemyhorizontalwalkanim[enemy[i].animframe];
						enemy[i].frametimer = enemyhorizontalwalkanim[enemy[i].animframe + 1];
					} else if (enemy[i].orientation == 2) {
						enemy[i].frame = enemyupwalkanim[enemy[i].animframe];
						enemy[i].frametimer = enemyupwalkanim[enemy[i].animframe + 1];
					} else if (enemy[i].orientation == 3) {
						enemy[i].frame = enemydownwalkanim[enemy[i].animframe];
						enemy[i].frametimer = enemydownwalkanim[enemy[i].animframe + 1];
					}
				}
				
				if (enemy[i].frame == 255) {
					enemy[i].animframe = 0;
				} else {
					correctframe = 1;
					enemy[i].animframe = enemy[i].animframe + 2;
				}
			}
			
			if (enemy[i].state == 3) {
				enemy[i].state = 0;
			}
		
		}
		
		if (enemy[i].state == 0) enemy[i].frame = 0;
	
	}
}

void gameplay() {
	spawntimer = spawntimer - 1;
	
	if (!spawntimer) {
		spawnenemy();
		spawnrate = (spawnrate < 100)? spawnrate - 100: 5;
		spawntimer = spawnrate;
	}
}

void logic() {
	inputlogic();
	enemylogic();
	animhero();
	animenemies();
	gameplay();
	
	odd = (odd == 1)? 2: 1;
}

void painthero() {
	UBYTE orientationvalue = 0;
	
	set_sprite_tile(0, hero.frame);
	set_sprite_tile(1, hero.frame + 1);
	set_sprite_tile(2, hero.frame + 2);
	set_sprite_tile(3, hero.frame + 3);
	
	if (!hero.orientation) {
		move_sprite(2, hero.x, hero.y);
		move_sprite(3, hero.x, hero.y + 8);
		move_sprite(0, hero.x + 8, hero.y);
		move_sprite(1, hero.x + 8, hero.y + 8);
		orientationvalue = S_FLIPX;
	} else {
		move_sprite(0, hero.x, hero.y);
		move_sprite(1, hero.x, hero.y + 8);
		move_sprite(2, hero.x + 8, hero.y);
		move_sprite(3, hero.x + 8, hero.y + 8);
	}
	
	set_sprite_prop(0, orientationvalue);
	set_sprite_prop(1, orientationvalue);
	set_sprite_prop(2, orientationvalue);
	set_sprite_prop(3, orientationvalue);
}

void paintenemies() {
	UBYTE orientationvalue;
	UBYTE enemysprite;
	UBYTE i;
	
	for (i = 0; i != ENEMY_NO; i++) {
	
		orientationvalue = 0;
		enemysprite = 8 + (i * 4);
		
		set_sprite_tile(enemysprite + 0, enemy[i].frame);
		set_sprite_tile(enemysprite + 1, enemy[i].frame + 1);
		set_sprite_tile(enemysprite + 2, enemy[i].frame + 2);
		set_sprite_tile(enemysprite + 3, enemy[i].frame + 3);
		
		if (!enemy[i].orientation) {
			move_sprite(enemysprite + 2, enemy[i].x - scrlx, enemy[i].y - scrly);
			move_sprite(enemysprite + 3, enemy[i].x - scrlx, enemy[i].y + 8 - scrly);
			move_sprite(enemysprite + 0, enemy[i].x + 8 - scrlx, enemy[i].y - scrly);
			move_sprite(enemysprite + 1, enemy[i].x + 8 - scrlx, enemy[i].y + 8 - scrly);
			orientationvalue = S_FLIPX;
		} else {
			move_sprite(enemysprite + 0, enemy[i].x - scrlx, enemy[i].y - scrly);
			move_sprite(enemysprite + 1, enemy[i].x - scrlx, enemy[i].y + 8 - scrly);
			move_sprite(enemysprite + 2, enemy[i].x + 8 - scrlx, enemy[i].y - scrly);
			move_sprite(enemysprite + 3, enemy[i].x + 8 - scrlx, enemy[i].y + 8 - scrly);
		}
		
		set_sprite_prop(enemysprite + 0, orientationvalue);
		set_sprite_prop(enemysprite + 1, orientationvalue);
		set_sprite_prop(enemysprite + 2, orientationvalue);
		set_sprite_prop(enemysprite + 3, orientationvalue);

	}
}

void paintbkg() {
	scroll_bkg(scrlxinc, scrlyinc);
}

void writenum (UBYTE num) {
	UBYTE fdigit;
	UBYTE sdigit;
	UBYTE tdigit;
	
	UBYTE i = 0;
	
	tdigit = num % 10;
	sdigit = (num % 100 - tdigit) / 10;
	fdigit = num / 100;
	
	set_sprite_tile(37, 180 + fdigit);
	set_sprite_tile(38, 180 + sdigit);
	set_sprite_tile(39, 180 + tdigit);

}

void paintsword() {
	if (!swordtimer) return;
	
	swordtimer = swordtimer - 1;
	if (swordtimer == 0) {
		swordframe = 0;
	}
	
	move_sprite(4, swordx, swordy);
	move_sprite(5, swordx, swordy + 8);
	move_sprite(6, swordx + 8, swordy);
	move_sprite(7, swordx + 8, swordy + 8);
	
	set_sprite_tile(4, swordframe);
	set_sprite_tile(5, swordframe + 1);
	set_sprite_tile(6, swordframe + 2);
	set_sprite_tile(7, swordframe + 3);
}

void paint() {
	
	writenum(debug);
	painthero();
	paintenemies();
	paintbkg();
	paintsword();
}

void game() {
	
	while (health!=0) {
		
		setinput(joypad());
		
		logic();
		
		wait_vbl_done();
		
		paint();
	}
}

void main() {
	UBYTE i;
	
	while(1){
		
		wait_vbl_done();
		
		for (i = 0; i < 37; i++) {
			set_sprite_tile(i, 0);
		}
		
		disable_interrupts();
		
		SPRITES_8x8;
		
		DISPLAY_OFF;
		HIDE_BKG;
		HIDE_WIN;
		HIDE_SPRITES;
		
		
		set_bkg_data(0, 190, tileset);
		set_bkg_tiles(0, 0, 20, 18, title);
		
		move_bkg(0, 0);
		
		SHOW_BKG;
		SHOW_SPRITES;
		DISPLAY_ON;
	
		enable_interrupts();
		
		do	{
			setinput(joypad());
			wait_vbl_done();
		} while (!input[6]);
		
		init();
	
		game();
	}
}

