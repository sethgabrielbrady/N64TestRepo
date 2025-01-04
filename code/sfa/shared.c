#include <libdragon.h>
#include "shared.h"
#include "fighter.h"



int frame_w = 110;
int frame = 0;
bool showbackground = false;
float scale_x = 1.0;
float scale_y = 1.0;
float posX = 72.0f;
float posY = 240.0f;
sprite_t* fulgoresheetv1;
sprite_t* fulgorejump;
sprite_t* current_spritesheet;
sprite_t* background;
sprite_t* background2;
sprite_t* current_background;


float cx_val = 160.00;

void update_cx(void){
  current_background = background2;
}

void load_sprites(void) {
  fulgoresheetv1 = sprite_load("rom:/kenidle.sprite");
  fulgorejump = sprite_load("rom:/fulgorejumpv2.sprite");
  // background = sprite_load("rom:/fac3.sprite");
  // background2 = sprite_load("rom:/fac4.sprite");
  current_spritesheet = fulgoresheetv1;
  current_background = background;
}

void updateFighterBlit(void) {
  rdpq_sprite_blit(current_spritesheet, posX, posY, &(rdpq_blitparms_t){
      .s0 = fighter.anim_frame*frame_w, //Extract correct sprite from sheet
      .t0 = fighter.spr_ndx*ANIM_FRAME_H,
      //Set sprite center to bottom-center
      .cx = ANIM_FRAME_W/2,
      .cy = ANIM_FRAME_H,
      .width = ANIM_FRAME_W, //Extract correct width from sheet
      .height = ANIM_FRAME_H,
      .flip_x = fighter.flip,
      .scale_x = scale_x,
      .scale_y = scale_y,
  });
    rdpq_sync_tile();
}

void add_background(void) {
  rdpq_sprite_blit(current_background, 160, 120, &(rdpq_blitparms_t){
      // .s0 = 0, //Extract correct sprite from sheet
      // .t0 = 0,
      //Set sprite center to bottom-center
      .cx = cx_val,
      .cy = 120,
      // .width = 320, //Extract correct width from sheet
      // .height = 240,
      // .scale_x = 1.2,


  });
  rdpq_sync_tile();
}
