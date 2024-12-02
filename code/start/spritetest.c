#include <libdragon.h>
#include <stdlib.h>
#include <string.h>

int main(void) {
	display_context_t disp;
	display_init(RESOLUTION_320x240, DEPTH_16_BPP, 3, GAMMA_NONE, FILTERS_RESAMPLE);
	dfs_init(DFS_DEFAULT_LOCATION);

	// Load the spritesheet into memory
  sprite_t* fulgore = {
    sprite_load("rom:/fulgore.sprite"),
  };

	// Frame counter
	int i = 0;

	// Main loop
	while (1) {
		// Load and clear the frame buffer
		while(!(disp = display_lock()));
		// Clear the screen
		graphics_fill_screen(disp, graphics_make_color (0, 0, 0, 0));

		// Draw the sprite from the spritesheet
		graphics_draw_sprite_trans_stride(
			disp,					// Load into frame buffer
			display_get_width()/2,	// Move it towards the right
			100,					// Don't move up or down
			fulgore,				// Load this spritesheet
			(i>>2)%36				// Select the next sprite in the spritesheet every 4 frames
			);
		  i++;

		// Show the display
		display_show(disp);
	}
}