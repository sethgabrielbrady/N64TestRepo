#include "libdragon.h"
#include <malloc.h>
#include <math.h>
// #include <t3d/t3d.h>
// #include <t3d/t3dmath.h>
// #include <t3d/t3dmodel.h>
// #include <stdlib.h>

//Animation frame size defines
#define ANIM_FRAME_W 100
#define ANIM_FRAME_H 130

//Animation frame timing defines
#define ANIM_FRAME_DELAY 5
#define ANIM_FRAME_MAX 10

#define KNIGHT_MAX 2

//Structure for knight sprite
typedef struct {
    float x;
    float y;
    bool attack;
    bool flip;
    int time;
} knight_data;

static knight_data knights[2];
static sprite_t *sheet_knight;
static sprite_t *sheet_knight2;
static sprite_t *current_knight;

void render(void)
{
    // Attach and clear the screen
    surface_t *disp = display_get();
    rdpq_attach_clear(disp, NULL);
    //Set render mode to the standard render mode
    rdpq_set_mode_standard();
    rdpq_mode_alphacompare(1); // colorkey (draw pixel with alpha >= 1)
    for(int i=0; i<KNIGHT_MAX; i++) {

        if (i>0) {
            current_knight = sheet_knight2;
        } else {
            current_knight = sheet_knight;
        }
        int frame = knights[i].time/ANIM_FRAME_DELAY; //Calculate knight frame
        //Draw knight sprite
        rdpq_sprite_blit(current_knight, knights[i].x, knights[i].y, &(rdpq_blitparms_t){
            .s0 = frame*ANIM_FRAME_W, //Extract correct sprite from sheet
            //Set sprite center to bottom-center
            .cx = ANIM_FRAME_W/2,
            .cy = ANIM_FRAME_H,
            .width = ANIM_FRAME_W, //Extract correct width from sheet
            .flip_x = knights[i].flip
        });

    }
    //Detach the screen
    rdpq_detach_show();
}

void update(void)
{
    for(int i=0; i<KNIGHT_MAX; i++) {
        if(knights[i].attack) {
            knights[i].time++; //Increment time
            //Stop attack at end of animation
            if(knights[i].time >= ANIM_FRAME_DELAY*ANIM_FRAME_MAX) {
                knights[i].time = 0;
                // knights[i].attack = false;
            }
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
    sheet_knight = sprite_load("rom:/fulgorestandhq.sprite");
    sheet_knight2 = sprite_load("rom:/fulgorestandlq.sprite");
    //Initialize left fulgore
    knights[0].x = (display_get_width()/2)-50;
    knights[0].y = display_get_height()-15;
    // //Initialize right knight
    knights[1].x = (display_get_width()/2)+50;
    knights[1].y = knights[0].y;
    knights[1].flip = true;
    while (1)
    {
        render();
        update();

        //Read joypad
        joypad_poll();
        joypad_buttons_t ckeys = joypad_get_buttons_pressed(JOYPAD_PORT_1);
        //Set attack for left knight
        if(ckeys.a) {
            knights[0].attack = !knights[0].attack;
        }
        //Set attack for right knight
        if(ckeys.b) {
            knights[1].attack = true;
        }
    }
}
