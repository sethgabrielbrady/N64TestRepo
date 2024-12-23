
// we may want to use a plane like the font_billboard in snake3d
// use rdpq_sprite_blit to draw the sprite -- see tiny3d/examples/99_testscene/main.c

// need to update with state rather than if statements


//https://libdragon.dev/ref/rdpq__sprite_8h.html





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
#include "core.h"





surface_t *depthBuffer;
T3DViewport viewport;
T3DMat4FP* mapMatFP;
T3DModel *modelMap;
rspq_block_t *dplMap;
T3DVec3 camPos;
T3DVec3 camTarget;
T3DVec3 lightDirVec;
rspq_syncpoint_t syncPoint;
// sprite_t* fulgore;
sprite_t* fulgoresheetv1;
sprite_t* fulgorejump;
sprite_t* current_spritesheet;


float frametime = 0.0f;
float posX = 160.0f;
float posY = 240.0f;
float rotBGAngleY = 0.0f;
float rotBGAngleYCopy = 0.0f;
joypad_inputs_t joypad;
bool canJump = true;
int frame_w = 110;
int vel_x = 0;
int vel_y = 0;
int y_speed = 10.5f;
char str1[32];
char str2[32];
char str3[32];
char str4[32];
float jump_frame_delay = 3;
float standing_frame_delay = 3;
int new_frame = 0;
int current_x = 0;
int max_frame = 0;
bool show_debug = false;
//int frame_delay = 6;
int frame_delay = 6;

int pressCounter = 0;
int counter = 0;
int seconds = 0; //animation frames update every half second


//Animation frame size defines
#define ANIM_FRAME_W 110
#define ANIM_FRAME_H 140
#define ANIM_IDLE_W 110
#define ANIM_JUMP_W 116


//#define ANIM_FRAME_DELAY 6
#define ANIM_FRAME_DELAY frame_delay

//standing animation frame size defines
#define ANIM_FULG_STAND_FRAME_MAX 5
#define ANIM_FULG_STAND_ROW_MAX 2
#define ANIM_FULG_STAND_COL_MAX 5
//walking animation frame size defines
#define ANIM_FULG_WALK_FRAME_MAX 15
#define ANIM_FULG_WALK_ROW_MAX 5
#define ANIM_FULG_WALK_COL_MAX 5
//jumping animation frame size defines
#define ANIM_FULG_JUMP_FRAME_MAX 9
#define ANIM_FULG_JUMP_ROW_MAX 4
#define ANIM_FULG_JUMP_COL_MAX 5

// sprite_t * current_spritesheet;
int walk_start_index = 2;
int standing_start_index = 0;
int jump_start_index = 2;
float pos_speed = 1.25f;
int frame;
bool jump_peak = false;
typedef struct {
    float x;
    float y;
    bool walking;
    bool idle;
    bool jumping;
    bool flip;
    int time;
    bool reverse_frame;
    bool backing_up;
    int sheet_index;
    int row_max;
    int anim_frame;

} fighter_data;

static fighter_data fighter;
// static sprite_t * fighter_1;


void get_fighter_state (void)
{
    if (fighter.walking)
        {
            fighter.row_max = ANIM_FULG_WALK_ROW_MAX;
            fighter.idle = false;
            if (fighter.sheet_index == ANIM_FULG_WALK_ROW_MAX)
            {
                fighter.sheet_index = walk_start_index;
            }
        }
    if (fighter.idle)
    {
         fighter.walking = false;
         fighter.jumping = false;
         fighter.row_max = ANIM_FULG_STAND_ROW_MAX;
         if (fighter.sheet_index == ANIM_FULG_STAND_ROW_MAX)
         {
            fighter.sheet_index = standing_start_index;
         }
    }
    if (fighter.jumping)
    {
        fighter.idle = false;
        fighter.walking = false;
        fighter.row_max = ANIM_FULG_JUMP_ROW_MAX;
        if (fighter.sheet_index == ANIM_FULG_JUMP_ROW_MAX)
        {
          fighter.sheet_index = jump_start_index;
        }
    }
}

void game_init(void)
{
  timer_init();

  fighter.reverse_frame = false;
  fighter.sheet_index = 0;
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



  t3d_init((T3DInitParams){});

  viewport = t3d_viewport_create();

  mapMatFP = malloc_uncached(sizeof(T3DMat4FP));
  t3d_mat4fp_from_srt_euler(mapMatFP,
    (float[3]){0.3f, 0.3f, 0.3f},
    (float[3]){0, 0, 0},
    (float[3]){0, 0, -10}
  );

  camPos = (T3DVec3){{0, 30.0f, 115.0f}};
  camTarget = (T3DVec3){{0, 0, 45}};

  lightDirVec = (T3DVec3){{1.0f, 1.0f, 1.0f}};
  t3d_vec3_norm(&lightDirVec);

  fulgoresheetv1 = sprite_load("rom:/fulgoresheetv1.sprite");
  fulgorejump = sprite_load("rom:/fulgorejumpv2.sprite");
  current_spritesheet = fulgoresheetv1;

  modelMap = t3d_model_load("rom:/map.t3dm");

  rspq_block_begin();
    t3d_matrix_push(mapMatFP);
    rdpq_set_prim_color(RGBA32(255, 255, 255, 255));
    t3d_model_draw(modelMap);
    t3d_matrix_pop(1);
  dplMap = rspq_block_end();

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
    if(fighter.walking || fighter.jumping)  {

        fighter.idle = false;
        if (fighter.walking) {
          fighter.jumping = false;

          // backing up animation
          if (fighter.backing_up && fighter.time <= 0) {

            if (fighter.sheet_index > 2 && fighter.anim_frame == 0){
              fighter.sheet_index -= 1;
              fighter.time = ANIM_FRAME_DELAY*ANIM_FULG_WALK_COL_MAX;
            } else if (fighter.sheet_index == 2) {
              fighter.sheet_index = 4;
              fighter.time = ANIM_FRAME_DELAY*ANIM_FULG_WALK_COL_MAX;
            }

          } else if (fighter.time >= ANIM_FRAME_DELAY*ANIM_FULG_WALK_COL_MAX) {
              fighter.time = 0;
              fighter.sheet_index += 1;

            if(fighter.time >= ANIM_FRAME_DELAY*ANIM_FULG_WALK_FRAME_MAX) {
                fighter.time = 0;
                fighter.sheet_index = walk_start_index;
            }
          }
        }
        else if (fighter.jumping)
        {
          fighter.walking = false;
          if (fighter.time >= ANIM_FRAME_DELAY*ANIM_FULG_JUMP_COL_MAX) {
              fighter.time = 0;

              fighter.sheet_index += 1;

            if(fighter.time >= ANIM_FRAME_DELAY*ANIM_FULG_JUMP_FRAME_MAX) {
                fighter.time = 0;

                fighter.sheet_index = jump_start_index;
            }
          }
        }
    } else {
        fighter.idle = true;
        if (fighter.time > max_frame) {
          max_frame = fighter.time;
        }

        if (fighter.time <= 0) {
          if(fighter.sheet_index == 0 && fighter.anim_frame == 0) {
            fighter.reverse_frame = false;
            fighter.time = 0;
          } else {
            fighter.sheet_index -= 1;
            fighter.time = ANIM_FRAME_DELAY*ANIM_FULG_STAND_COL_MAX;
          }
        }
        else if (fighter.sheet_index == 0 && fighter.time <= 0)
        {
            fighter.reverse_frame = false;
            fighter.sheet_index  = 0;
            fighter.time = 0;
        }
        else if (fighter.time >= ANIM_FRAME_DELAY*ANIM_FULG_STAND_COL_MAX )
        {
            // go to the next row of the standing animation
          if (fighter.sheet_index == 1 && fighter.time >= 1) {
            fighter.reverse_frame = true;
          }
          else
          {
            fighter.reverse_frame = false;
            fighter.sheet_index += 1;
            fighter.time = 0;
          }
        }
    }


}


void updateFighterBlit(void) {
  //fighter.anim_frame = fighter.time/ANIM_FRAME_DELAY; //Calculate fighter frame
  rdpq_sprite_blit(current_spritesheet, posX, posY, &(rdpq_blitparms_t){
      //.s0 = fighter.frame*frame_w, //Extract correct sprite from sheet
      .s0 = fighter.anim_frame*frame_w, //Extract correct sprite from sheet
      .t0 = fighter.sheet_index*ANIM_FRAME_H,
      //Set sprite center to bottom-center
      .cx = ANIM_FRAME_W/2,
      .cy = ANIM_FRAME_H,
      .width = ANIM_FRAME_W, //Extract correct width from sheet
      .height = ANIM_FRAME_H,
      .flip_x = fighter.flip,
  });
  rdpq_sync_tile();
}

void game_loop(float deltaTime)
{

  // surface_t *disp = display_get();
  //   while (!(disp = display_get()));

  //rdpq_attach_clear(disp, depthBuffer);
    // surface_t* disp = display_get();
    // // display_context_t disp = 0;
  // while (!(disp = display_get()));

  uint8_t colorAmbient[4] = {0x00, 0x00, 0x00, 0xAA};
  uint8_t colorDir[4]     = {0xAA, 0xAA, 0xAA, 0xFF};

  t3d_viewport_set_projection(&viewport, T3D_DEG_TO_RAD(90.0f), 20.0f, 160.0f);
  t3d_viewport_look_at(&viewport, &camPos, &camTarget, &(T3DVec3){{0,1,0}});


  // ======== Draw (3D) ======== //
  rdpq_attach(display_get(), depthBuffer);
  t3d_frame_start();
  t3d_viewport_attach(&viewport);

  // also bg color?
  t3d_screen_clear_color(RGBA32(00, 00, 102, 0xFF));
  // t3d_screen_clear_color(RGBA32(224, 180, 96, 0xFF));
  t3d_screen_clear_depth();
  t3d_light_set_ambient(colorAmbient);
  t3d_light_set_directional(0, colorDir, &lightDirVec);
  t3d_light_set_count(1);
  rspq_block_run(dplMap);
  syncPoint = rspq_syncpoint_new();


  // add sprite
  // rdpq_set_mode_copy(true);
  rdpq_set_mode_standard();
  rdpq_mode_combiner(RDPQ_COMBINER_TEX);
  rdpq_mode_blender(RDPQ_BLENDER_MULTIPLY);

if (rotBGAngleY != rotBGAngleYCopy) {
  t3d_mat4fp_from_srt_euler(mapMatFP,
    (float[3]){0.3f, 0.3f, 0.3f},
    (float[3]){0, rotBGAngleY, 0},
    (float[3]){0, 0, -10}
  );

  rotBGAngleYCopy = rotBGAngleY;
}
  get_fighter_state();
  updateFrame();
  updateFighterBlit();
  //rspq_wait();
  // rdpq_sync_tile();
  rdpq_sync_pipe(); // Hardware crashes otherwise
  rdpq_detach_show();
  joypad_poll();
  update();

}

void game_cleanup(void)
{
  rspq_block_free(dplMap);
  t3d_model_free(modelMap);
  free_uncached(mapMatFP);
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
        fighter.sheet_index = walk_start_index;

      } else if (btnPressed.d_left){
        //beginning of the backwards walking animation
        fighter.backing_up = true;
        fighter.reverse_frame = true;
        fighter.sheet_index = 4;
        fighter.time = ANIM_FRAME_DELAY*ANIM_FULG_WALK_COL_MAX;
      }

    }

    if (btnHeld.d_right && posX < 320.0f)
    {
      vel_x = pos_speed;
      posX += vel_x;
      rotBGAngleY += -0.00095f;
    }
    if (btnHeld.d_left && posX > 0.0f)
    {
      vel_x = pos_speed;
      posX -= vel_x;
      rotBGAngleY += 0.00095f;
    }

    if ((btnReleased.d_right || btnReleased.d_left) && !fighter.jumping)
    {
      fighter.idle = true;
      fighter.walking = false;
      //fighter.time = 0;
      vel_x = 0;
      fighter.sheet_index = standing_start_index;
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

      // else {
      //   fighter.walking = true;
      //   fighter.sheet_index = walk_start_index;
      // }

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
          fighter.sheet_index = standing_start_index;
        } else {
          fighter.walking = true;
          fighter.sheet_index = walk_start_index;
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
      fighter.sheet_index = jump_start_index;

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
          fighter.sheet_index = standing_start_index;
        } else {
          fighter.walking = true;
          fighter.sheet_index = walk_start_index;
        }
      }
    }

  // for debug
    if (btnHeld.b) {
      show_debug = true;
      frame_delay = 30;
      frame = fighter.time/6;
    } else if (btnReleased.b) {
      show_debug = false;
      frame_delay = 6;
    }

}

void debug_stuff() {
    display_context_t disp = 0;
    while (!(disp = display_get()));
    // graphics_fill_screen(disp, graphics_make_color(0, 0, 0, 255));
    sprintf( str3, "%d", frame);
    graphics_draw_text(disp, 80, 120, "frame" );
    graphics_draw_text(disp, 10, 120, str3);
    sprintf( str1, "%d", fighter.sheet_index);
    graphics_draw_text(disp, 80, 100, str1 );
    graphics_draw_text(disp, 10, 100, "sprindx");
    if (fighter.reverse_frame) {
      graphics_draw_text(disp, 80, 110, "reverse");
    } else {
      graphics_draw_text(disp, 80, 110, "forward");
    }
    if (fighter.idle) {
      graphics_draw_text(disp, 10, 110, "idle");
    } else if (fighter.walking) {
      graphics_draw_text(disp, 10, 110, "walking");
    } else if (fighter.jumping) {
      graphics_draw_text(disp, 10, 110, "jumping");
    }

    sprintf( str2, "%d", fighter.anim_frame);
    graphics_draw_text(disp, 10, 90, str2);
    graphics_draw_text(disp, 80, 90, "anim_frame");

    sprintf( str4, "%d", fighter.time);

    graphics_draw_text(disp, 10, 80, str4);
    graphics_draw_text(disp, 80, 80, "FT");
    // sprintf( str2, "%d", pressCounter);
    // graphics_draw_text(disp, 80, 20, str2 );
    // graphics_draw_text(disp, 10, 20, "press#" );
    display_show(disp);

}

int main()
{
  game_init();

  while(1)
  {
    game_loop(0.0f);
    check_controller_state();

    if (show_debug) {
      debug_stuff();
    }
  }


  game_cleanup();
  return 0;
}
