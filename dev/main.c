#include <gb/gb.h>

extern const unsigned char tileset[];
extern const unsigned char bkgset[];
extern const unsigned char map[];

#define ENEMY_NO 8

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
unsigned UBYTE input[] = {
	0, 0, 0, 0, 0, 0, 0, NULL
};

unsigned UBYTE scrlx;
unsigned UBYTE scrly;

signed UBYTE scrlxinc;
signed UBYTE scrlyinc;

typedef struct soldier {
	UBYTE x; // x axis position of the sprite
	UBYTE y; // y axis position of the sprite
	UBYTE orientation; // left (0), right (1), up (2), down (3)
	UBYTE state; // null (0), idle (1), walking (2)
	UBYTE frame; // frame number in the tileset
	UBYTE animframe; // frame number in the animation
	UBYTE frametimer; // numbers of iterations before reaching the next frame in the animation
}soldier_generic;

soldier_generic hero;
soldier_generic enemy[ENEMY_NO];

/* Animations */
/* odd: frame, even: iterations of the frame */
/* 255: end of the animation */

/* Hero */

unsigned UBYTE herohorizontalidleanim[] = {
	 20,   5, 255, NULL
};

unsigned UBYTE heroupidleanim[] = {
	 36,   5, 255, NULL
};

unsigned UBYTE herodownidleanim[] = {
	  4,   5, 255, NULL
};

unsigned UBYTE herohorizontalwalkanim[] = {
	 20,   5,  24,   5,  28,   5,  32,   5, 255, NULL
};

unsigned UBYTE heroupwalkanim[] = {
	 36,   5,  40,   5,  44,   5,  48,   5, 255, NULL
};

unsigned UBYTE herodownwalkanim[] = {
	  4,   5,   8,   5,  12,   5,  16,   5, 255, NULL
};

/* Enemies */

unsigned UBYTE enemyhorizontalidleanim[] = {
	 68,   5, 255, NULL
};

unsigned UBYTE enemyupidleanim[] = {
	 84,   5, 255, NULL
};

unsigned UBYTE enemydownidleanim[] = {
	 52,   5, 255, NULL
};

unsigned UBYTE enemyhorizontalwalkanim[] = {
	 68,   5,  72,   5,  76,   5,  80,   5, 255, NULL
};

unsigned UBYTE enemyupwalkanim[] = {
	 84,   5,  88,   5,  92,   5,  96,   5, 255, NULL
};

unsigned UBYTE enemydownwalkanim[] = {
	 52,   5,  56,   5,  60,   5,  64,   5, 255, NULL
};

void init() {
	
	UBYTE i = 0;
	
	wait_vbl_done();
	disable_interrupts();
	
	SPRITES_8x8;
	
	DISPLAY_OFF;
	HIDE_BKG;
	HIDE_WIN;
	HIDE_SPRITES;
	
	set_bkg_data(0, 1, bkgset);
	set_bkg_tiles(0, 0, 32, 32, map);
	
	set_sprite_data(0, 100, tileset);
	
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
		enemy[i].state = 2;
		enemy[i].frame = 0;
		enemy[i].frametimer = 1;
	}
	
	SHOW_BKG;
	SHOW_SPRITES;
	DISPLAY_ON;
	
	enable_interrupts();
}

void setinput(int key) {
	
	input[0] = key & J_LEFT;
	input[1] = key & J_RIGHT;
	input[2] = key & J_UP;
	input[3] = key & J_DOWN;
	input[4] = key & J_A;
	input[5] = key & J_B;
	input[6] = key & J_START;
	input[7] = key & J_SELECT;
}

inputlogic() {
	unsigned UBYTE neworientation = hero.orientation;
	unsigned UBYTE newstate = 1;
	
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
	
	unsigned UBYTE correctframe = 0;
	
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

void animenemies() {
	unsigned UBYTE correctframe;
	unsigned UBYTE i;
	
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
	animhero();
	animenemies();
}

void painthero() {
	unsigned UBYTE orientationvalue = 0;
	
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
	unsigned UBYTE orientationvalue;
	unsigned UBYTE enemysprite;
	unsigned UBYTE i;
	
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

void paint() {
	
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

