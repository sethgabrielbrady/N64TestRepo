#include <libdragon.h>
#include "shared.h"
#include "fighter.h"



int frame_w = 110;
int frame = 0;
bool showbackground = false;
float scale_x = 1.0;
float scale_y = 1.0;
float posX = 160.0f;
float posY = 240.0f;
sprite_t* fulgoresheetv1;
sprite_t* fulgorejump;
sprite_t* current_spritesheet;
sprite_t* background;




void load_sprites(void) {
  fulgoresheetv1 = sprite_load("rom:/fulgoresheetv1.sprite");
  fulgorejump = sprite_load("rom:/fulgorejumpv2.sprite");
  background = sprite_load("rom:/facility.sprite");
  current_spritesheet = fulgoresheetv1;
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
  rdpq_sprite_blit(background, 160, 120, &(rdpq_blitparms_t){
      .s0 = 0, //Extract correct sprite from sheet
      .t0 = 0,
      //Set sprite center to bottom-center
      .cx = 160,
      .cy = 120,
      .width = 320, //Extract correct width from sheet
      .height = 240,
  });
}