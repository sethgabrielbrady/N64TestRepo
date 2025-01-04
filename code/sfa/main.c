#include <libdragon.h>
#include <t3d/t3d.h>
#include <t3d/t3dmath.h>
#include <t3d/t3dmodel.h>
#include <t3d/t3ddebug.h>
#include <stdlib.h>
#include <malloc.h>
#include <math.h>
#include <stdint.h>
#include <stdio.h>

#include "fighter.h"
#include "controller.h"
#include "shared.h"
#include "stats.h"


T3DViewport viewport;
T3DMat4FP* mapMatFP;
T3DModel *modelMap;
T3DVec3 camPos;
T3DVec3 camTarget;
T3DVec3 lightDirVec;

surface_t *depthBuffer;
rspq_block_t *dplMap;
rspq_syncpoint_t syncPoint;
display_context_t disp;




// float fps;
float xd_copy = 0.0f;

void game_init(void)
{
  // initialize various components
  debug_init_isviewer();
  debug_init_usblog();

  fighter_init(fighter);

  joypad_init();
  joypad = joypad_get_inputs(0);

  asset_init_compression(2);
  dfs_init(DFS_DEFAULT_LOCATION);
  rdpq_init();


  display_init(RESOLUTION_320x240, DEPTH_16_BPP, 3, GAMMA_NONE, FILTERS_RESAMPLE);

  depthBuffer = display_get_zbuf();
  t3d_init((T3DInitParams){});
  viewport = t3d_viewport_create();
  camPos = (T3DVec3){{0, 20.0f, 115.0f}};
  camTarget = (T3DVec3){{0, 0, 45}};
  lightDirVec = (T3DVec3){{1.0f, 1.0f, 0.0f}};
  t3d_vec3_norm(&lightDirVec);

  //This is where I should seperate out the T3d?
  mapMatFP = malloc_uncached(sizeof(T3DMat4FP));
  t3d_mat4fp_from_srt_euler(mapMatFP,
    (float[3]){0.3f, 0.3f, 0.3f},
    (float[3]){0, 0, 0},
    (float[3]){0, 0, -10}
  );

  load_sprites();
  load_font();

  modelMap = t3d_model_load("rom:/facility.t3dm");
  rspq_block_begin();
  t3d_matrix_push(mapMatFP);
  t3d_model_draw(modelMap);
  t3d_matrix_pop(1);
  dplMap = rspq_block_end();

}


void update(void)
{
  // update to take in fighter data
  update_frame_direction(fighter);
  fighter_state_check(fighter);
}

void game_loop(float deltaTime)
{
  uint8_t colorAmbient[4] = {0xaa, 0xaa, 0xaa, 0xaa};
  uint8_t colorDir[4]     = {0xdd, 0xdd, 0xdd, 0xdd};

  t3d_viewport_set_projection(&viewport, T3D_DEG_TO_RAD(90.0f), 10.0f, 160.0f);
  t3d_viewport_look_at(&viewport, &camPos, &camTarget, &(T3DVec3){{0,1,0}});


  // // ======== Draw (3D) ======== //
  rdpq_attach(display_get(), depthBuffer);
  //rdpq_attach(display_get(), NULL);
  t3d_frame_start();
  t3d_viewport_attach(&viewport);
  t3d_screen_clear_color(RGBA32(00, 00, 00, 0xFF));

  rspq_block_run(dplMap);

  t3d_screen_clear_depth();
  t3d_light_set_ambient(colorAmbient);
  t3d_light_set_directional(0, colorDir, &lightDirVec);
  t3d_light_set_count(1);


  syncPoint = rspq_syncpoint_new();

  // add sprite
  rdpq_set_mode_standard();
  rdpq_mode_combiner(RDPQ_COMBINER_TEX);
  rdpq_mode_blender(RDPQ_BLENDER_MULTIPLY);

  if ( x_dist != xd_copy) {
    t3d_mat4fp_from_srt_euler(mapMatFP,
      (float[3]){0.3f, 0.3f, 0.3f},
      (float[3]){0, 0, 0},
      (float[3]){x_dist, 0, -10}
      );

    xd_copy = x_dist;
  }


  // get_fighter_animation(fighter);
  update_frame_direction(fighter);
  //add_background();

  // if (showbackground) {
  //   add_background();
  // }

  rdpq_sync_tile();
  updateFighterBlit(); // sprite update
  stats_draw_billboard(fighter);

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
  free_uncached(mapMatFP);
  sprite_free(current_spritesheet);
  sprite_free(background);
  t3d_destroy();
  display_close();
}

int main()
{
  game_init();
  while(1)
  {
    game_loop(0.0f);
    check_controller_state();
  };

  game_cleanup();
  return 0;
}
