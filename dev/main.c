#include <gb/gb.h>

extern const unsigned char tileset[];

void init() {
	
	wait_vbl_done();
	disable_interrupts();
	
	SPRITES_8x8;
	
	DISPLAY_OFF;
	HIDE_BKG;
	HIDE_WIN;
	HIDE_SPRITES;
	
	set_sprite_data(0, 4, tileset);
	set_sprite_tile(0, 1);
	move_sprite(0, 80, 72);
	
	SHOW_BKG;
	SHOW_WIN;
	SHOW_SPRITES;
	DISPLAY_ON;
	enable_interrupts();
}

void game() {
	
	while (1) {
		
		// Lógica del juego
		
		wait_vbl_done();
		
		// Pintado
	}
}

void main() {
	init();
	
	game();
}

