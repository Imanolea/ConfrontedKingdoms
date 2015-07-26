#include <gb/gb.h>
#include <rand.h>

extern const unsigned char tileset[];
extern const unsigned char bkgset[];
extern const unsigned char map[];

#define ENEMY_NO 7

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
	 20,   5, 255, NULL
};

UBYTE heroupidleanim[] = {
	 36,   5, 255, NULL
};

UBYTE herodownidleanim[] = {
	  4,   5, 255, NULL
};

UBYTE herohorizontalwalkanim[] = {
	 20,   5,  24,   5,  28,   5,  32,   5, 255, NULL
};

UBYTE heroupwalkanim[] = {
	 36,   5,  40,   5,  44,   5,  48,   5, 255, NULL
};

UBYTE herodownwalkanim[] = {
	  4,   5,   8,   5,  12,   5,  16,   5, 255, NULL
};

/* Enemies */

UBYTE enemyhorizontalidleanim[] = {
	 68,   5, 255, NULL
};

UBYTE enemyupidleanim[] = {
	 84,   5, 255, NULL
};

UBYTE enemydownidleanim[] = {
	 52,   5, 255, NULL
};

UBYTE enemyhorizontalwalkanim[] = {
	 68,   5,  72,   5,  76,   5,  80,   5, 255, NULL
};

UBYTE enemyupwalkanim[] = {
	 84,   5,  88,   5,  92,   5,  96,   5, 255, NULL
};

UBYTE enemydownwalkanim[] = {
	 52,   5,  56,   5,  60,   5,  64,   5, 255, NULL
};

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
	
	set_bkg_data(0, 1, bkgset);
	set_win_data(0, 1, bkgset);
	set_bkg_tiles(0, 0, 32, 32, map);
	
	set_sprite_data(0, 126, tileset);
	
	hero.x = 80;
	hero.y = 80;
	hero.orientation = 3;
	hero.state = 1;
	hero.frame = 0;
	hero.frametimer = 1;
	
	for (i = 0; i != ENEMY_NO; i++) {
		enemy[i].x = i * 16;
		enemy[i].y = i * 16;
		enemy[i].orientation = 3;
		enemy[i].state = 0;
		enemy[i].frame = 0;
		enemy[i].frametimer = 1;
		enemy[i].dirtimer = 20;
	}
	
	move_sprite(37, 8, 20);
	move_sprite(38, 16, 20);
	move_sprite(39, 24, 20);
	
	scroll_win(0, 144);
	
	debug = 145;
	
	SHOW_BKG;
	SHOW_WIN;
	SHOW_SPRITES;
	DISPLAY_ON;
	
	enable_interrupts();
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
	
	if (input[7]) { // Select
		if (!preinput[7]) {
			spawnenemy();
		}
	}
	
	scrlx = scrlx + scrlxinc;
	scrly = scrly + scrlyinc;
	
	if (neworientation != hero.orientation || newstate != hero.state) {
		hero.animframe = 0;
		hero.frametimer = 1;
		hero.orientation = neworientation;
		hero.state = newstate;
	}
}

void animhero() {
	
	UBYTE correctframe = 0;
	
	hero.frametimer = hero.frametimer - 1;
	
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

void enemylogic() {
	UBYTE i;
	UBYTE disx;
	UBYTE disy;
	UBYTE neworientation;
	
	for (i = 0; i != ENEMY_NO; i++) {
		
		enemy[i].dirtimer = enemy[i].dirtimer - 1;
		
		if (!enemy[i].dirtimer) {
		
			disx = ((hero.x - enemy[i].x + scrlx) > (enemy[i].x - hero.x - scrlx))? enemy[i].x - hero.x - scrlx: hero.x - enemy[i].x + scrlx;
			disy = ((hero.y - enemy[i].y + scrly) > (enemy[i].y - hero.y - scrly))? enemy[i].y - hero.y - scrly: hero.y - enemy[i].y + scrly;
			
			if (disx > disy) {
				neworientation = ((hero.x - enemy[i].x + scrlx) > (enemy[i].x - hero.x - scrlx))? 0: 1;
			} else {
				neworientation = ((hero.y - enemy[i].y + scrly) > (enemy[i].y - hero.y - scrly))? 2: 3;
			}
			
			if (neworientation != enemy[i].orientation) {
				enemy[i].animframe = 0;
				enemy[i].frametimer = 1;
				enemy[i].orientation = neworientation;
			}
			
			enemy[i].dirtimer = 20;
		
		}
		
		if (enemy[i].orientation == 0) {
			enemy[i].x = enemy[i].x - 2;
		} else if (enemy[i].orientation == 1) {
			enemy[i].x = enemy[i].x + 2;			
		} else if (enemy[i].orientation == 2) {
			enemy[i].y = enemy[i].y - 2;			
		} else if (enemy[i].orientation == 3) {
			enemy[i].y = enemy[i].y + 2;			
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
		
		}
		
		if (enemy[i].state == 0) enemy[i].frame = 0;
	
	}
}

void logic() {
	inputlogic();
	enemylogic();
	animhero();
	animenemies();
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
	
	tdigit = num % 10;
	sdigit = (num % 100 - tdigit) / 10;
	fdigit = num / 100;
	
	set_sprite_tile(37, 116 + fdigit);
	set_sprite_tile(38, 116 + sdigit);
	set_sprite_tile(39, 116 + tdigit);
}

void paint() {
	
	writenum(debug);
	painthero();
	paintenemies();
	paintbkg();
}

void game() {
	
	while (1) {
		
		setinput(joypad());
		
		logic();
		
		wait_vbl_done();
		
		paint();
	}
}

void main() {
	init();
	
	game();
}

