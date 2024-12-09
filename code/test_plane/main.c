
// we may want to use a plane like the font_billboard in snake3d
// use rdpq_sprite_blit to draw the sprite -- see tiny3d/examples/99_testscene/main.c

#include <libdragon.h>
#include <t3d/t3d.h>
#include <t3d/t3dmath.h>
#include <t3d/t3dmodel.h>
#include <t3d/t3ddebug.h>
#include <stdlib.h>
#include <malloc.h>
#include <math.h>


surface_t *depthBuffer;
T3DViewport viewport;
T3DMat4FP* mapMatFP;
T3DModel *modelMap;
rspq_block_t *dplMap;
T3DVec3 camPos;
T3DVec3 camTarget;
T3DVec3 lightDirVec;
rspq_syncpoint_t syncPoint;
sprite_t* fulgore;
float posX = 60.0f;
float posY = 230.0f;
float rotBGAngleY = 0.0f;
float rotBGAngleYCopy = 0.0f;
joypad_inputs_t joypad;
bool canJump = true;


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
float walk_speed = 1;
int frame;
typedef struct {
    float x;
    float y;
    bool walk_forward;
    bool idle;
    bool flip;
    int time;

} fighter_data;

static fighter_data fighter;
// static sprite_t * fighter_1;

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

void game_init(void)
{
  debug_init_isviewer();
  debug_init_usblog();
  joypad_init();
  joypad = joypad_get_inputs(0);

  asset_init_compression(2);
  dfs_init(DFS_DEFAULT_LOCATION);
  rdpq_init();
  display_init(RESOLUTION_320x240, DEPTH_16_BPP, 3, GAMMA_NONE, FILTERS_RESAMPLE_ANTIALIAS);
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

  fulgore = sprite_load("rom:/fulgoresheetv1.sprite");

  modelMap = t3d_model_load("rom:/map.t3dm");

  rspq_block_begin();
    t3d_matrix_push(mapMatFP);
    rdpq_set_prim_color(RGBA32(255, 255, 255, 255));
    t3d_model_draw(modelMap);
    t3d_matrix_pop(1);
  dplMap = rspq_block_end();

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


void game_loop(float deltaTime)
{
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
  frame = fighter.time/ANIM_FRAME_DELAY; //Calculate fighter frame
  rdpq_sprite_blit(fulgore, posX, posY, &(rdpq_blitparms_t){
        .s0 = frame*ANIM_FRAME_W, //Extract correct sprite from sheet
        .t0 = current_sheet_row_index*ANIM_FRAME_H,
        //Set sprite center to bottom-center
        .cx = ANIM_FRAME_W/2,
        .cy = ANIM_FRAME_H,
        .width = ANIM_FRAME_W, //Extract correct width from sheet
        .height = ANIM_FRAME_H,
        .flip_x = fighter.flip
    });

  //rspq_wait();
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
        fighter.time = 0;
        fighter.walk_forward = true;
        fighter.idle = false;
        current_sheet_row_index = walk_start_index;
    }

    if (btnHeld.d_right && posX < 320.0f)
    {
      posX += walk_speed;
      rotBGAngleY += -0.00095f;
    }
    if (btnHeld.d_left && posX > 0.0f)
    {
      posX -= walk_speed;
      rotBGAngleY += 0.00095f;
    }

    if (btnReleased.d_right || btnReleased.d_left)
    {
        fighter.idle = true;
        fighter.walk_forward = false;
        fighter.time = 0;
        current_sheet_row_index = standing_start_index;
    }


    // jumping animation
    if (btnPressed.d_up && canJump){
      while (posY >= 165.0f){
        posY -= walk_speed;
        canJump = false;
      }
    } else if (posY < 230.0f && !canJump) {
      posY += walk_speed * 2.75f;
      if (posY >= 230.0f ) {
        canJump = true;
      }
    }

}

int main()
{

  game_init();

  while(1)
  {
    game_loop(0.0f);
    check_controller_state();
  }

  game_cleanup();
  return 0;
}

