#ifndef SHARED_H
#define SHARED_H

#include <libdragon.h>

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

void updateFighterBlit();
void add_background();
void load_sprites();


#endif