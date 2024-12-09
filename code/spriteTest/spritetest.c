#include "libdragon.h"
#include <malloc.h>
#include <math.h>
// #include <t3d/t3d.h>
// #include <t3d/t3dmath.h>
// #include <t3d/t3dmodel.h>
// #include <stdlib.h>


// look at putting the sprite on a triangle or a plane
// start incorporating state machines

//Animation frame size defines
#define ANIM_FRAME_W 110
#define ANIM_FRAME_H 140
#define ANIM_FRAME_DELAY 4
// #define ANIM_FRAME_DELAY 30


//standing animation frame size defines
#define ANIM_FULG_STAND_FRAME_MAX 10
#define ANIM_FULG_STAND_ROW_MAX 2
#define ANIM_FULG_STAND_COL_MAX 5
//walking animation frame size defines
#define ANIM_FULG_WALK_FRAME_MAX 15
#define ANIM_FULG_WALK_ROW_MAX 5
#define ANIM_FULG_WALK_COL_MAX 5

int current_sheet_row_index = 0;
int walk_start_index = 2;
int standing_start_index = 0;

int frame;
char str[32];

//Structure for fighter sprite
typedef struct {
    float x;
    float y;
    bool walk_forward;
    bool idle;
    bool flip;
    int time;

} fighter_data;

static fighter_data fighter;
static sprite_t * fighter_1;
display_context_t disp;


void get_fighter_state (void)
{
    if (fighter.walk_forward)
        {
            fighter.idle = false;
            if (current_sheet_row_index == ANIM_FULG_WALK_ROW_MAX)
            {
                current_sheet_row_index = walk_start_index;
            }
        }
    if (fighter.idle)
    {
         fighter.walk_forward = false;
         if (current_sheet_row_index == ANIM_FULG_STAND_ROW_MAX)
         {
            current_sheet_row_index = standing_start_index;
         }
    }
}

void render(void)
{
    // Attach and clear the screen
    surface_t *disp = display_get();
    rdpq_attach_clear(disp, NULL);
    //Set render mode to the standard render mode
    rdpq_set_mode_standard();
    rdpq_mode_alphacompare(1); // colorkey (draw pixel with alpha >= 1)


    get_fighter_state();

    frame = fighter.time/ANIM_FRAME_DELAY; //Calculate fighter frame
    //Draw fighter sprite
    rdpq_sprite_blit(fighter_1, fighter.x, fighter.y, &(rdpq_blitparms_t){
        .s0 = frame*ANIM_FRAME_W, //Extract correct sprite from sheet
        .t0 = current_sheet_row_index*ANIM_FRAME_H,
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
    fighter.time++; //Increment time
    if(fighter.walk_forward ) {
        if (fighter.time >= ANIM_FRAME_DELAY*ANIM_FULG_WALK_COL_MAX) {
            fighter.time = 0;
            current_sheet_row_index += 1;

            if(fighter.time >= ANIM_FRAME_DELAY*ANIM_FULG_WALK_FRAME_MAX) {
                fighter.time = 0;
                current_sheet_row_index = walk_start_index;
            }
        }
    } else {
        fighter.idle = true;
        if (fighter.time >= ANIM_FRAME_DELAY*ANIM_FULG_STAND_COL_MAX) {
            // go to the next row of the standing animation
            current_sheet_row_index += 1;
            fighter.time = 0;
        } else if (fighter.time >= ANIM_FRAME_DELAY*ANIM_FULG_STAND_FRAME_MAX)
        {
            // go back to the start of the standing animation
            current_sheet_row_index = standing_start_index;
            fighter.time = 0;
        }
    }
}

// Draw text on the screen
void show_debug_text(void) {
    display_context_t disp = 0;
    while (!(disp = display_get()));
    graphics_fill_screen(disp, graphics_make_color(0, 0, 0, 0));
    sprintf( str, "%d", current_sheet_row_index);
    graphics_draw_text(disp, 10, 10, str );
    display_show(disp);
}

void check_controller_state(void) {
    joypad_buttons_t btnPressed = joypad_get_buttons_pressed(JOYPAD_PORT_1);
    // joypad_buttons_t btnHeld = joypad_get_buttons_held(JOYPAD_PORT_1);
    joypad_buttons_t btnReleased = joypad_get_buttons_released(JOYPAD_PORT_1);

    if (btnPressed.d_right)
    {
        fighter.time = 0;
        fighter.walk_forward = true;
        fighter.idle = false;
        current_sheet_row_index = walk_start_index;
    }
    if (btnReleased.d_right)
    {
        fighter.idle = true;
        fighter.walk_forward = false;
        fighter.time = 0;
        current_sheet_row_index = standing_start_index;
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
        joypad_poll();
        check_controller_state();

        //show_debug_text();
    }
}


