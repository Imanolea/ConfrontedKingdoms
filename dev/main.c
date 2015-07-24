#include <gb/gb.h>

extern const unsigned char tileset[];

/* key pressed (1) or not (0)
0: left
1: right
2: up
3: down
4: a
5: b
6: start
7: select
 */
unsigned ubyte input[] = {
	0, 0, 0, 0, 0, 0, 0, NULL
};

struct player {
	ubyte x; // x axis position of the sprite
	ubyte y; // y axis position of the sprite
};

static struct player hero;

void init() {
	
	wait_vbl_done();
	disable_interrupts();
	
	SPRITES_8x8;
	
	DISPLAY_OFF;
	HIDE_BKG;
	HIDE_WIN;
	HIDE_SPRITES;
	
	hero.x = 80;
	hero.y = 72;
	
	set_sprite_data(0, 4, tileset);
	set_sprite_tile(0, 1);
	move_sprite(0, hero.x, hero.y);
	
	SHOW_BKG;
	SHOW_WIN;
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
	if (input[0]) { // left
		hero.x = hero.x - 1;
	}
	
	if (input[1]) { // right
		hero.x = hero.x + 1;		
	}
	
	if (input[2]) { // up
		hero.y = hero.y - 1;		
	}
	
	if (input[3]) { // down
		hero.y = hero.y + 1;
	}
}

void logic() {
	inputlogic();
}

void paint() {
	move_sprite(0, hero.x, hero.y);
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

