#include <libdragon.h>
#include <t3d/t3d.h>
#include <t3d/t3dmath.h>
#include <t3d/t3dmodel.h>
#include <t3d/t3ddebug.h>
#include <stdlib.h>
#include <malloc.h>
#include <math.h>
#include <stdint.h>
#include <string.h>
#include <stdio.h>

#include "fighter.h"


surface_t *depthBuffer;
T3DViewport viewport;
T3DMat4FP* mapMatFP;
T3DMat4FP* lightMatFP;
T3DModel *modelLight;
T3DModel *modelMap;
T3DModel *modelCube;
rspq_block_t *dplMap;
rspq_block_t *dplLight;

T3DVec3 camPos;
T3DVec3 camTarget;
T3DVec3 lightDirVec;
rspq_syncpoint_t syncPoint;
sprite_t* fulgoresheetv1;
sprite_t* fulgorejump;
sprite_t* current_spritesheet;
sprite_t* fulgstand1;
// sprite_t* fulgidle;
sprite_t* background;
rdpq_font_t *fontBillboard;
display_context_t disp;
joypad_inputs_t joypad;


float fps;
float angle_speed = 0.00095f;
float x_dist = 0.0f;
float xd_copy = 0.0f;
float rotBGAngleY = 0.0f;
float rotBGAngleYCopy = 0.0f;
float posX = 160.0f;
float posY = 240.0f;
bool canJump = true;
int frame_w = 110;
int vel_x = 0;
int vel_y = 0;
int y_speed = 10.5f;
int current_x = 0;
int max_frame = 0;
// int frame_delay = 6;
bool showbackground = false;


float pos_speed = 1.25f;
int frame;
bool jump_peak = false;
float scale_x = 1.0;
float scale_y = 1.0;

#define BILLBOARD_YOFFSET   15.0f
#define FONT_BILLBOARD      2
#define TEXT_COLOR    0x6CBB3CFF


void game_init(void)
{

  // update this inside fighter.c
  fighter.reverse_frame = false;
  fighter.spr_ndx = 0;
  fighter.anim_frame = 0;

  // debug_init_isviewer();
  // debug_init_usblog();
  joypad_init();
  joypad = joypad_get_inputs(0);

  asset_init_compression(2);
  dfs_init(DFS_DEFAULT_LOCATION);
  rdpq_init();
  display_init(RESOLUTION_320x240, DEPTH_16_BPP, 3, GAMMA_NONE, FILTERS_RESAMPLE);
  depthBuffer = display_get_zbuf();
  //rdpq_set_mode_copy(true);
 //rdpq_set_mode_standard();


  t3d_init((T3DInitParams){});

  viewport = t3d_viewport_create();

  mapMatFP = malloc_uncached(sizeof(T3DMat4FP));
  // lightMatFP = malloc_uncached(sizeof(T3DMat4FP));


  t3d_mat4fp_from_srt_euler(mapMatFP,
    (float[3]){0.3f, 0.3f, 0.3f},
    (float[3]){0, 0, 0},
    (float[3]){0, 0, -10}
  );
  // t3d_mat4fp_from_srt_euler(lightMatFP,
  //   (float[3]){0.3f, 0.3f, 0.3f},
  //   (float[3]){0, 0, 0},
  //   (float[3]){0, 0, -20}
  // );




  //camPos = (T3DVec3){{0, 30.0f, 115.0f}}; //facility
  camPos = (T3DVec3){{0, 20.0f, 115.0f}};
  camTarget = (T3DVec3){{0, 0, 45}};


  lightDirVec = (T3DVec3){{1.0f, 1.0f, 0.0f}};
  //lightDirVec = (T3DVec3){{1.0f, 1.0f, 1.0f}};

  t3d_vec3_norm(&lightDirVec);

  fulgoresheetv1 = sprite_load("rom:/fulgoresheetv1.sprite");   // fulgorejump = sprite_load("rom:/fulgorejumpv2.sprite");
  fulgorejump = sprite_load("rom:/fulgorejumpv2.sprite");
  fulgstand1 = sprite_load("rom:/fulgstand1.sprite");
  // fulgidle = sprite_load("rom:/fulgidle.sprite");
  background = sprite_load("rom:/facility.sprite");
  fontBillboard = rdpq_font_load("rom:/squarewave.font64");
  rdpq_text_register_font(FONT_BILLBOARD, fontBillboard);
  rdpq_font_style(fontBillboard, 0, &(rdpq_fontstyle_t){.color = color_from_packed32(TEXT_COLOR) });




  current_spritesheet = fulgoresheetv1;




  // rdp_load_texture( 0, 0, MIRROR_DISABLED, background );

  modelMap = t3d_model_load("rom:/facility.t3dm");
  //modelLight = t3d_model_load("rom:/cube.t3dm");


  // modelCube = t3d_model_load("rom:/skyline.t3dm");



  rspq_block_begin();
  t3d_matrix_push(mapMatFP);
  //rdpq_set_prim_color(RGBA32(255, 255, 255, 122));
  // t3d_model_draw(modelCube);
  t3d_model_draw(modelMap);
  //t3d_model_draw(modelCube);

  t3d_matrix_pop(1);
  dplMap = rspq_block_end();

  // rspq_block_begin();
  // //t3d_matrix_push(lightMatFP);

  // //t3d_model_draw(modelLight);
  // //t3d_matrix_pop(1);

  // dplLight = rspq_block_end();
}

void updateFrame(void) {
// need to set up so that the fighter.anim_frame updates at an interval of fighter.time
// this way, if needed fighter.time can be sped up or slowed down without affect the frame. Rather
// the frame would just update quicker
// anim_frame should not be directly effected by fighter time?
//


  if(fighter.reverse_frame) {
    // redo with max frame or count number of times reversed
    fighter.time --;
  } else {
    fighter.time ++;
  }

  // I want to change this because in order to have the ability to update frame timing.
  fighter.anim_frame = fighter.time/ANIM_FRAME_DELAY; //Calculate fighter frame
}
void update(void)
{
    updateFrame();
    // update this in fighter.c
    if(fighter.walking || fighter.jumping)  {

        fighter.idle = false;
        if (fighter.walking) {
          fighter.jumping = false;

          // backing up animation
          if (fighter.backing_up && fighter.time <= 0) {

            if (fighter.spr_ndx > 2 && fighter.anim_frame == 0){
              fighter.spr_ndx -= 1;
              fighter.time = ANIM_FRAME_DELAY*ANIM_FULG_WALK_COL_MAX;
            } else if (fighter.spr_ndx == 2) {
              fighter.spr_ndx = 4;
              fighter.time = ANIM_FRAME_DELAY*ANIM_FULG_WALK_COL_MAX;
            }

          } else if (fighter.time >= ANIM_FRAME_DELAY*ANIM_FULG_WALK_COL_MAX) {
              fighter.time = 0;
              fighter.spr_ndx += 1;

            if(fighter.time >= ANIM_FRAME_DELAY*ANIM_FULG_WALK_FRAME_MAX) {
                fighter.time = 0;
                fighter.spr_ndx = walk_start_index;
            }
          }
        }
        else if (fighter.jumping)
        {
          fighter.walking = false;
          if (fighter.time >= ANIM_FRAME_DELAY*ANIM_FULG_JUMP_COL_MAX) {
              fighter.time = 0;

              fighter.spr_ndx += 1;

            if(fighter.time >= ANIM_FRAME_DELAY*ANIM_FULG_JUMP_FRAME_MAX) {
                fighter.time = 0;

                fighter.spr_ndx = jump_start_index;
            }
          }
        }
    } else {
        fighter.idle = true;
        if (fighter.time > max_frame) {
          max_frame = fighter.time;
        }

        if (fighter.time <= 0) {
          if(fighter.spr_ndx == 0 && fighter.anim_frame == 0) {
            fighter.reverse_frame = false;
            fighter.time = 0;
          } else {
            fighter.spr_ndx -= 1;
            fighter.time = ANIM_FRAME_DELAY*ANIM_FULG_STAND_COL_MAX;
          }
        }
        else if (fighter.spr_ndx == 0 && fighter.time <= 0)
        {
            fighter.reverse_frame = false;
            fighter.spr_ndx  = 0;
            fighter.time = 0;
        }
        else if (fighter.time >= ANIM_FRAME_DELAY*ANIM_FULG_STAND_COL_MAX )
        {
            // go to the next row of the standing animation
          if (fighter.spr_ndx == 1 && fighter.time >= 1) {
            fighter.reverse_frame = true;
          }
          else
          {
            fighter.reverse_frame = false;
            fighter.spr_ndx += 1;
            fighter.time = 0;
          }
        }
    }


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

void stats_draw_billboard(void)
{
  int x = 10;
  int y = 10;
  int width = 100;
  int height = 100;
  int alpha = 64;

  rdpq_set_mode_standard();

  rdpq_mode_combiner(RDPQ_COMBINER_FLAT);
  rdpq_mode_blender(RDPQ_BLENDER_MULTIPLY);
  rdpq_set_prim_color(RGBA32(255, 0, 0, alpha));
  rdpq_fill_rectangle(x, y, width, height);
  rdpq_text_printf(&(rdpq_textparms_t){}, FONT_BILLBOARD, x+2, y+15, "FPS %f", fps);
  rdpq_text_printf(&(rdpq_textparms_t){}, FONT_BILLBOARD, x+2, y+25, "a frame %d", fighter.anim_frame);
  rdpq_text_printf(&(rdpq_textparms_t){}, FONT_BILLBOARD, x+2, y+45, "spr idx %d", fighter.spr_ndx);

  if (fighter.reverse_frame) {
    rdpq_text_printf(&(rdpq_textparms_t){}, FONT_BILLBOARD, x+45, y+35, "%s", "reverse");
  } else {
    rdpq_text_printf(&(rdpq_textparms_t){}, FONT_BILLBOARD, x+45, y+35, "%s", "forward");
  }

  if (fighter.idle) {
    rdpq_text_printf(&(rdpq_textparms_t){}, FONT_BILLBOARD, x+2, y+35, "%s", "idle");
  } else if (fighter.walking) {
    rdpq_text_printf(&(rdpq_textparms_t){}, FONT_BILLBOARD, x+2, y+35, "%s", "walking");
  } else if (fighter.jumping) {
    rdpq_text_printf(&(rdpq_textparms_t){}, FONT_BILLBOARD, x+2, y+35, "%s", "jumping");
  }

  rdpq_sync_pipe(); // Hardware crashes otherwise
}



void game_loop(float deltaTime)
{

  //uint8_t colorAmbient[4] = {0x00, 0x00, 0x00, 0xAA};
  //uint8_t colorAmbient[4] = {0x00, 0x00, 0x00, 0xff};
  uint8_t colorAmbient[4] = {0xaa, 0xaa, 0xaa, 0xaa};
  uint8_t colorDir[4]     = {0xdd, 0xdd, 0xdd, 0xdd};
  //t3d_viewport_set_projection(&viewport, T3D_DEG_TO_RAD(90.0f), 20.0f, 160.0f);

  t3d_viewport_set_projection(&viewport, T3D_DEG_TO_RAD(90.0f), 10.0f, 160.0f);
  t3d_viewport_look_at(&viewport, &camPos, &camTarget, &(T3DVec3){{0,1,0}});


  // // ======== Draw (3D) ======== //


  // //rdpq_attach(display_get(), NULL);
  rdpq_attach(display_get(), depthBuffer);
  t3d_frame_start();
  t3d_viewport_attach(&viewport);

  // also bg color?
  t3d_screen_clear_color(RGBA32(00, 00, 00, 0xFF));

  rspq_block_run(dplMap);


  t3d_screen_clear_depth();
  t3d_light_set_ambient(colorAmbient);
  t3d_light_set_directional(0, colorDir, &lightDirVec);
  t3d_light_set_count(1);
  //t3d_light_set_count(lightCount);



  // t3d_matrix_push_pos(1);
  //   for(int i = 0; i < lightCount; i++) {
  //     t3d_light_set_directional(i, &dirLights[i].color.r, &dirLights[i].dir);

  //     t3d_matrix_set(&lightMatFP[i], true);

  //     rdpq_set_prim_color(dirLights[i].color);
  //     rspq_block_run(dplLight);
  //   }
  // t3d_matrix_pop(1);

  // rspq_block_run(dplLight);

  syncPoint = rspq_syncpoint_new();

  // add sprite
  rdpq_set_mode_standard();
  rdpq_mode_combiner(RDPQ_COMBINER_TEX);
  rdpq_mode_blender(RDPQ_BLENDER_MULTIPLY);


  if ( x_dist != xd_copy) {
  //if (rotBGAngleY != rotBGAngleYCopy) {
    t3d_mat4fp_from_srt_euler(mapMatFP,
      (float[3]){0.3f, 0.3f, 0.3f},
      // (float[3]){0, rotBGAngleY, 0},
      (float[3]){0, 0, 0},

      (float[3]){x_dist, 0, -10}
    );

    // t3d_mat4fp_from_srt_euler(lightMatFP,
    //   (float[3]){0.3f, 0.3f, 0.3f},
    //   (float[3]){0, (rotBGAngleY + .0001), 0},
    //   (float[3]){0, 0, -10}
    // );

    xd_copy = x_dist;
    rotBGAngleYCopy = rotBGAngleY;
  }


  get_fighter_state();
  updateFrame();



  if (showbackground) {
    add_background();
  }
  rdpq_sync_tile();
  //stats_draw_billboard();

  updateFighterBlit();
  stats_draw_billboard();

  rspq_wait();
  rdpq_sync_tile();
  rdpq_sync_pipe(); // Hardware crashes otherwise
  rdpq_detach_show();
  joypad_poll();
  update();
}

void game_cleanup(void)
{
  rspq_block_free(dplMap);
  t3d_model_free(modelMap);
  //t3d_model_free(modelCube);
  free_uncached(mapMatFP);
  sprite_free(current_spritesheet);
  sprite_free(background);
  t3d_destroy();
  display_close();
}

void check_controller_state(void) {
    joypad_buttons_t btnPressed = joypad_get_buttons_pressed(JOYPAD_PORT_1);
    joypad_buttons_t btnHeld = joypad_get_buttons_held(JOYPAD_PORT_1);
    joypad_buttons_t btnReleased = joypad_get_buttons_released(JOYPAD_PORT_1);

    if (btnPressed.d_right || btnPressed.d_left)
    {
      // fighter.time = 0;
      fighter.walking = true;
      fighter.idle = false;
      // fighter.backing_up = false;
      if (btnPressed.d_right){
        fighter.reverse_frame = false;
        fighter.backing_up = true;
        fighter.time = 0;
        fighter.spr_ndx = walk_start_index;

      } else if (btnPressed.d_left){
        //beginning of the backwards walking animation
        fighter.backing_up = true;
        fighter.reverse_frame = true;
        fighter.spr_ndx = 4;
        fighter.time = ANIM_FRAME_DELAY*ANIM_FULG_WALK_COL_MAX;
      }

    }

    if (btnHeld.d_right)
    {
      if (angle_speed < 0) {
        angle_speed = angle_speed * -1;
      }
      rotBGAngleY += angle_speed;
      x_dist -= 0.15;
      if (posX < 320.0f) {
        vel_x = pos_speed;
        posX += vel_x;
        //bg_x -= 0.5;
        // x_dist -= 0.05;
      }
    }
    if (btnHeld.d_left)
    {
      if (angle_speed > 0) {
        angle_speed = angle_speed * -1;
      }
      rotBGAngleY += angle_speed;
      x_dist += 0.15;

      if (posX > 0.0f) {
        vel_x = pos_speed;
        posX -= vel_x;
        // rotBGAngleY += -0.00095f;
        //bg_x += 0.5;
        // x_dist += 0.05;
      }

    }

    if ((btnReleased.d_right || btnReleased.d_left) && !fighter.jumping)
    {
      fighter.idle = true;
      fighter.walking = false;
      //fighter.time = 0;
      vel_x = 0;
      fighter.spr_ndx = standing_start_index;
    }



    // jumping animation
    if (fighter.jumping && !jump_peak){

      if (vel_x == 0) {
        if (posY > 130.0f) {
          posY -= y_speed;
        } else {
            jump_peak = true;
        }
      }
      else if (vel_x != 0) {
        if (posY > 145.0f) {
          posY -= y_speed;
        } else {
          jump_peak = true;
        }
      }


    } else if (fighter.jumping && jump_peak) {
      if (posY < 240.0f) {
        posY += y_speed;

      } else if (posY >= 240.0f) {
        jump_peak = false;
        fighter.jumping = false;
        frame_w = ANIM_FRAME_W;
        current_spritesheet = fulgoresheetv1;
        fighter.time = 0;

        if (vel_x == 0) {
          fighter.idle = true;
          fighter.spr_ndx = standing_start_index;
        } else {
          fighter.walking = true;
          fighter.spr_ndx = walk_start_index;
        }
      }
    }

    int current_x = posX;
    if (fighter.jumping) {
      if (btnHeld.d_right || btnPressed.d_right) {
        if (posX < current_x + 160) { // need to factor in the screen width
          posX += vel_x*3;

        }
      }
      else if (btnHeld.d_left || btnPressed.d_left) {
        if (posX > current_x - 160) {
          posX -= vel_x*3;
        }
      }
    }



    if (btnPressed.d_up && !fighter.jumping){
      fighter.reverse_frame = false;
      current_spritesheet = fulgorejump;
      fighter.time = 0;
      fighter.jumping = true;
      fighter.idle = false;
      fighter.walking = false;
      frame_w = ANIM_JUMP_W;
      fighter.spr_ndx = jump_start_index;

    }
    else if (posY < 240.0f && fighter.jumping) {
      if (posY >= 240.0f && frame_w == ANIM_JUMP_W) {
        frame_w = ANIM_FRAME_W;
        current_spritesheet = fulgoresheetv1;
        fighter.time = 0;
        fighter.jumping = false;
        jump_peak = false;

        if (vel_x == 0) {
          fighter.idle = true;
          fighter.spr_ndx = standing_start_index;
        } else {
          fighter.walking = true;
          fighter.spr_ndx = walk_start_index;
        }
      }
    }

  // for debug
    if (btnHeld.b) {
      frame_delay = 30;
      frame = fighter.time/6;
    } else if (btnReleased.b) {
      frame_delay = 6;
    }

    if (btnHeld.a) {
      angle_speed = 0.01195f;
    } else {
      angle_speed = 0.00095f;
    }

    if (btnPressed.z) {
      showbackground = !showbackground;
    }

    if (btnPressed.c_up) {
      scale_x += .05;
      scale_y += .05;
    }
    if (btnPressed.c_down) {
      scale_x -= .05;
      scale_y -= .05;
    }
}





int main()
{
  game_init();

  while(1)
  {
    game_loop(0.0f);
    check_controller_state();
    fps = display_get_fps();
  };


  game_cleanup();
  return 0;
}
