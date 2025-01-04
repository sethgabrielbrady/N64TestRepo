#ifndef SHARED_H
#define SHARED_H

#include <libdragon.h>

extern float cx_val;
extern int frame_w;
extern int frame ;
extern bool showbackground;
extern float scale_x;
extern float scale_y;
extern float posX;
extern float posY;
extern sprite_t* fulgoresheetv1;
extern sprite_t* fulgorejump;
extern sprite_t* current_spritesheet;
extern sprite_t* background;
extern sprite_t* background2;
extern sprite_t* current_background;


//Animation frame size defines
#define ANIM_FRAME_W 66
#define ANIM_FRAME_H 98

// #define ANIM_FRAME_W 110
// #define ANIM_FRAME_H 140
#define ANIM_IDLE_W  66
#define ANIM_JUMP_W 116


//#define ANIM_FRAME_DELAY 60
#define ANIM_FRAME_DELAY frame_delay

//standing animation frame size defines
#define ANIM_FULG_STAND_FRAME_MAX 0
#define ANIM_FULG_STAND_ROW_MAX 0
#define ANIM_FULG_STAND_COL_MAX 6
//walking animation frame size defines
#define ANIM_FULG_WALK_FRAME_MAX 15
#define ANIM_FULG_WALK_ROW_MAX 5
#define ANIM_FULG_WALK_COL_MAX 5
//jumping animation frame size defines
#define ANIM_FULG_JUMP_FRAME_MAX 9
#define ANIM_FULG_JUMP_ROW_MAX 4
#define ANIM_FULG_JUMP_COL_MAX 5

void updateFighterBlit();
void add_background();
void load_sprites();
void update_cx();


#endif