#include "libdragon.h"
#include <malloc.h>
#include <math.h>
// #include <t3d/t3d.h>
// #include <t3d/t3dmath.h>
// #include <t3d/t3dmodel.h>
// #include <stdlib.h>


//combine the animations into 1 sprite sheet
// look at putting the sprite on a triangle or a plane

//Animation frame size defines
#define ANIM_FRAME_W 110
#define ANIM_FRAME_H 140

#define ANIM_FULG_STAND_FRAME_MAX 10
#define ANIM_FULG_STAND_ROW_MAX 5




//Animation frame timing defines
#define ANIM_FRAME_DELAY 4.5
#define ANIM_FULG_WALK_FRAME_MAX 15
#define ANIM_FULG_WALK_COL_MAX 5
#define ANIM_FULG_WALK_ROW_MAX 5


int tile_sheet_row_index = 0;
int walk_start_index = 2;
int frame;
int sheet_index_start = 0;
char str[32];

//Structure for fighter sprite
typedef struct {
    float x;
    float y;
    bool walk_forward;
    bool flip;
    int time;

} fighter_data;

static fighter_data fighter;
static sprite_t * fighter_1;
display_context_t disp;

void render(void)
{
    // Attach and clear the screen
    surface_t *disp = display_get();
    rdpq_attach_clear(disp, NULL);
    //Set render mode to the standard render mode
    rdpq_set_mode_standard();
    rdpq_mode_alphacompare(1); // colorkey (draw pixel with alpha >= 1)


    if(fighter.walk_forward )
    {
        if (tile_sheet_row_index == ANIM_FULG_WALK_ROW_MAX) {
            tile_sheet_row_index = walk_start_index;
            sheet_index_start = ANIM_FRAME_H*tile_sheet_row_index;
        }
    }
    else
    {
        sheet_index_start = 0;
        tile_sheet_row_index = 0;
    }

    frame = 0;
    frame = fighter.time/ANIM_FRAME_DELAY; //Calculate fighter frame
    //Draw fighter sprite
    rdpq_sprite_blit(fighter_1, fighter.x, fighter.y, &(rdpq_blitparms_t){
        .s0 = frame*ANIM_FRAME_W, //Extract correct sprite from sheet
        .t0 = sheet_index_start,
        // .t0 = 0,
        //Set sprite center to bottom-center
        .cx = ANIM_FRAME_W/2,
        .cy = ANIM_FRAME_H,
        .width = ANIM_FRAME_W, //Extract correct width from sheet
        .height = ANIM_FRAME_H
    });

    //Detach the screen
    rdpq_detach_show();
}

void update(void)
{
    if(fighter.walk_forward ) {

        fighter.time++; //Increment time
        //Stop walk at end of animation

        if (fighter.time >= ANIM_FRAME_DELAY*ANIM_FULG_WALK_COL_MAX) {
            tile_sheet_row_index += 1;
            fighter.time = 0;

            if(fighter.time >= ANIM_FRAME_DELAY*ANIM_FULG_WALK_FRAME_MAX) {
                fighter.time = 0;
                tile_sheet_row_index = 2;
                fighter.walk_forward = false;
            }
        }
    } else {
        tile_sheet_row_index = 0;
        fighter.time++;

         if (fighter.time >= ANIM_FRAME_DELAY*ANIM_FULG_STAND_ROW_MAX) {
            tile_sheet_row_index += 1;
        } else if (fighter.time >= ANIM_FRAME_DELAY*ANIM_FULG_STAND_FRAME_MAX)
        {
            tile_sheet_row_index = 0;
            fighter.time = 0;
        }
    }
}


int main()
{
    //Init logging
    debug_init_isviewer();
    debug_init_usblog();

    //Init display
    display_init(RESOLUTION_320x240, DEPTH_16_BPP, 3, GAMMA_NONE, FILTERS_RESAMPLE);
    //Init DragonFS
    dfs_init(DFS_DEFAULT_LOCATION);
    //Init RDPQ
    rdpq_init();
    //Init joypad
    joypad_init();
    //Load Sprite Sheet
    fighter_1 = sprite_load("rom:/fulgoresheetv1.sprite");
    //Initialize left fulgore
    fighter.x = (display_get_width()/2)-50;
    fighter.y = display_get_height()-15;

    while (1)
    {
        render();
        update();

        // display_context_t disp = 0;
        // while (!(disp = display_lock()));
        // graphics_fill_screen(disp, graphics_make_color(0, 0, 0, 0));
        // sprintf( str, "%d", tile_sheet_row_index);
        // graphics_draw_text(disp, 10, 10, str );
        // display_show(disp);

        //Read joypad
        joypad_poll();
        joypad_buttons_t ckeys = joypad_get_buttons_pressed(JOYPAD_PORT_1);
        joypad_buttons_t btnHeld = joypad_get_buttons_held(JOYPAD_PORT_1);
        //Set walk for left fighter
        // Draw text on the screen
        if(btnHeld.d_right) {
            fighter.walk_forward = true;
        }else {
            fighter.walk_forward = false;
            tile_sheet_row_index = 0;
        }

        // //Set attack for right fighter
        // if(ckeys.b) {
        //     fighter.attack = true;
        // }
    }
}


