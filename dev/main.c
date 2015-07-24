#include <gb/gb.h>

extern const unsigned char tileset[];
extern const unsigned char bkgset[];
extern const unsigned char map[];

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

struct player {
	UBYTE x; // x axis position of the sprite
	UBYTE y; // y axis position of the sprite
	UBYTE orientation; // left (0), right (1), up (2), down (3)
	UBYTE state; // idle (0), walking (1)
	UBYTE frame; // frame number in the tileset
	UBYTE animframe; // frame number in the animation
	UBYTE frametimer; // numbers of iterations before reaching the next frame in the animation
};

static struct player hero;

signed UBYTE scrlx;
signed UBYTE scrly;

/* Animations */
/* odd: frame, even: iterations of the frame */
/* 255: end of the animation */

unsigned UBYTE horizontalidleanim[] = {
	 20,   5, 255, NULL
};

unsigned UBYTE upidleanim[] = {
	 36,   5, 255, NULL
};

unsigned UBYTE downidleanim[] = {
	  4,   5, 255, NULL
};

unsigned UBYTE horizontalwalkanim[] = {
	 20,   5,  24,   5,  28,   5,  32,   5, 255, NULL
};

unsigned UBYTE upwalkanim[] = {
	 36,   5,  40,   5,  44,   5,  48,   5, 255, NULL
};

unsigned UBYTE downwalkanim[] = {
	  4,   5,   8,   5,  12,   5,  16,   5, 255, NULL
};

void animhero() {
	
	unsigned UBYTE correctframe = 0;
	
	hero.frametimer = hero.frametimer - 1;
	
	if (hero.frametimer) return;
	
	while (!correctframe) {
		if (hero.state == 0) {
			if (hero.orientation < 2) {
				hero.frame = horizontalidleanim[hero.animframe];
				hero.frametimer = horizontalidleanim[hero.animframe + 1];
			} else if (hero.orientation == 2) {
				hero.frame = upidleanim[hero.animframe];
				hero.frametimer = upidleanim[hero.animframe + 1];
			} else if (hero.orientation == 3) {
				hero.frame = downidleanim[hero.animframe];
				hero.frametimer = downidleanim[hero.animframe + 1];
			}
		} else if (hero.state == 1) {
			if (hero.orientation < 2) {
				hero.frame = horizontalwalkanim[hero.animframe];
				hero.frametimer = horizontalwalkanim[hero.animframe + 1];
			} else if (hero.orientation == 2) {
				hero.frame = upwalkanim[hero.animframe];
				hero.frametimer = upwalkanim[hero.animframe + 1];
			} else if (hero.orientation == 3) {
				hero.frame = downwalkanim[hero.animframe];
				hero.frametimer = downwalkanim[hero.animframe + 1];
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

void init() {
	
	wait_vbl_done();
	disable_interrupts();
	
	SPRITES_8x8;
	
	DISPLAY_OFF;
	HIDE_BKG;
	HIDE_WIN;
	HIDE_SPRITES;
	
	set_bkg_data(0, 1, bkgset);
	set_bkg_tiles(0, 0, 32, 32, map);
	
	set_sprite_data(0, 52, tileset);
	
	hero.x = 80;
	hero.y = 80;
	hero.orientation = 3;
	hero.state = 0;
	hero.frame = 0;
	hero.frametimer = 1;
	
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
	unsigned UBYTE newstate = 0;
	
	scrlx = 0;
	scrly = 0;
	
	if (input[0]) { // left
		scrlx = -1;
		neworientation = 0;
		newstate = 1;
	}
	
	if (input[1]) { // right
		scrlx = 1;
		neworientation = 1;
		newstate = 1;
	}
	
	if (input[2]) { // up
		scrly =-1;
		neworientation = 2;
		newstate = 1;
	}
	
	if (input[3]) { // down
		scrly = 1;
		neworientation = 3;
		newstate = 1;
	}
	
	if (neworientation != hero.orientation || newstate != hero.state) {
		hero.animframe = 0;
		hero.frametimer = 1;
		hero.orientation = neworientation;
		hero.state = newstate;
	}
}

void logic() {
	inputlogic();
	animhero();
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

void paintbkg() {
	scroll_bkg(scrlx, scrly);
}

void paint() {
	
	painthero();
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

