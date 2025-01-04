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

void updateFighterBlit();
void add_background();
void load_sprites();
void update_cx();


#endif