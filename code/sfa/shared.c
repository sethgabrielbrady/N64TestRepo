#include <libdragon.h>
#include "shared.h"
#include "fighter.h"



int frame_w = 66;
int frame = 0;
bool showbackground = false;
float scale_x = 1.0;
float scale_y = 1.0;
float posX = 72.0f;
float posY = 218.0f;
sprite_t* fulgoresheetv1;
sprite_t* fulgorejump;
sprite_t* kenwalk;
sprite_t* current_spritesheet;
sprite_t* background;
sprite_t* background2;
sprite_t* current_background;
sprite_t* fulgore;



float cx_val = 379.00;

void update_cx(void){
  current_background = background2;
}

void load_sprites(void) {
  fulgoresheetv1 = sprite_load("rom:/kenidle.sprite");
  kenwalk = sprite_load("rom:/kenwalk.sprite");
  fulgorejump = sprite_load("rom:/fulgorejumpv2.sprite");
  background = sprite_load("rom:/guysheet.sprite");
  background2 = sprite_load("rom:/facility.sprite");
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

void add_background(float scroll_x) {
  // Needs rewrite
  cx_val = scroll_x;


  rdpq_sprite_blit(background, 388, 120, &(rdpq_blitparms_t){
      .s0 = 1, //Extract correct sprite from sheet
      .t0 = 0,
      //Set sprite center to bottom-center
      .cx = 379 + (cx_val * 1.07),
      .cy = 120,
      .width = 723, //Extract correct width from sheet
      .height = 238,
  });
    rdpq_sync_tile();

   rdpq_sprite_blit(background, 388, 133, &(rdpq_blitparms_t){
    .s0 = 1, //Extract correct sprite from sheet
    .t0 = 240,
    //Set sprite center to bottom-center
    // .cx = 379 + (cx_val*5),
    .cx = 379 + (cx_val * 1.15),
    .cy = 120,
    .width = 719, //Extract correct width from sheet
    .height = 226,
  });
    rdpq_sync_tile();


  rdpq_sprite_blit(background, 388, 120, &(rdpq_blitparms_t){
      .s0 = 1, //Extract correct sprite from sheet
      .t0 = 474,
      //Set sprite center to bottom-center
      .cx = 379 + (cx_val*1.23),
      .cy = 120,
      .width = 769, //Extract correct width from sheet
      .height = 242,
  });

  rdpq_sync_tile();
}

