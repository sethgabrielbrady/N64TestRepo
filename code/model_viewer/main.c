#include <libdragon.h>
#include <t3d/t3d.h>
#include <t3d/t3dmath.h>
#include <t3d/t3dmodel.h>
#include <t3d/t3dskeleton.h>
#include <t3d/t3danim.h>
#include <t3d/t3ddebug.h>
#include <stdlib.h>
#include <malloc.h>
#include <math.h>
#include <stdint.h>
#include <stdio.h>
#include "controller.h"
#include "menu.h"
#include "models.h"

T3DViewport viewport;
T3DMat4FP* mapMatFP;
// T3DModel *modelMap;
// T3DModel *modelMap2;



// T3DMat4 modelMap; // matrix for our model, this is a "normal" float matrix
T3DVec3 camPos;
T3DVec3 camTarget;
T3DVec3 lightDirVec;

surface_t *depthBuffer;
rspq_block_t *dplMap;
rspq_syncpoint_t syncPoint;
display_context_t disp;
surface_t *fs;

int current_model = 0;
int selection = 0;
float lastTime = 0.0;

bool last_model = false;



void game_init(void)
{
  // initialize various components
  debug_init_isviewer();
  debug_init_usblog();
  selection = selection_counter;

  joypad_init();
  joypad = joypad_get_inputs(0);

  asset_init_compression(2);
  dfs_init(DFS_DEFAULT_LOCATION);
  rdpq_init();
  //rsp_init();


  display_init(RESOLUTION_320x240, DEPTH_16_BPP, 3, GAMMA_NONE, FILTERS_RESAMPLE);

  depthBuffer = display_get_zbuf();
  t3d_init((T3DInitParams){});
  viewport = t3d_viewport_create();
  // camPos = (T3DVec3){{-4.0f, 10.0f, 30.0f}};
  camPos = (T3DVec3){{0,10.0f,40.0f}};
  camTarget = (T3DVec3){{0, 0, 0}};

  lightDirVec = (T3DVec3){{1.0f, 1.0f, 0.0f}};
  t3d_vec3_norm(&lightDirVec);

  mapMatFP = malloc_uncached(sizeof(T3DMat4FP));

  load_model();
  modell.skel = t3d_skeleton_create(model);
  modell.skelBlend = t3d_skeleton_clone(&modell.skel, false); // optimized for blending, has no matrices
  modell.animWalk = t3d_anim_create(model, "walk");
  t3d_anim_attach(&modell.animWalk, &modell.skelBlend);
  // set up the anim
  t3d_anim_set_looping(&modell.animWalk, true); // loop this animation
  t3d_anim_set_playing(&modell.animWalk, true); // start in a paused state
  t3d_anim_attach(&modell.animWalk, &modell.skelBlend);
  t3d_anim_set_time(&modell.animWalk, 1.0f);


  load_font();
  load_models();


  rspq_block_begin();
  t3d_matrix_push(mapMatFP);
  t3d_model_draw(modelMaps[current_model]);
  t3d_matrix_pop(1);
  dplMap = rspq_block_end();

}

void update() {
  float newTime = get_time_s();
  deltaTime = newTime - lastTime;
  lastTime = newTime;
  // float currSpeed = 0.84f;
  float animBlend = 0.84 / 0.51f;

  if(animBlend > 1.0f)animBlend = 1.0f;

  if (current_model >= 5) {
    t3d_anim_update(&modell.animWalk, deltaTime);
    t3d_anim_set_speed(&modell.animWalk, animBlend + 0.15f);
    t3d_skeleton_blend(&modell.skel, &modell.skel, &modell.skelBlend, animBlend);
    t3d_skeleton_update(&modell.skel);
  }
}

void update_model(int current_model)
{
  free_uncached(mapMatFP);

  rspq_block_begin();
  t3d_matrix_push(mapMatFP);
  if (current_model >= 6) {
    t3d_model_draw_skinned(modelMaps[current_model],&modell.skel);
  }else {
    t3d_model_draw(modelMaps[current_model]);
  }
  t3d_matrix_pop(1);
  dplMap = rspq_block_end();

  rdpq_sync_tile();
}

void game_loop(float deltaTime)
{

  uint8_t colorAmbient[4] = {0xaa, 0xaa, 0xaa, 0xaa};
  uint8_t colorDir[4]     = {0xdd, 0xdd, 0xdd, 0xdd};

  // t3d_viewport_set_projection(&viewport, T3D_DEG_TO_RAD(85.0f), 10.0f, 90.0f);

  t3d_viewport_set_projection(&viewport, T3D_DEG_TO_RAD(90.0f), 2.0f, 90.0f);
  t3d_viewport_look_at(&viewport, &camPos, &camTarget, &(T3DVec3){{0,1,0}});



  // // ======== Draw (3D) ======== //
  rdpq_attach(display_get(), depthBuffer);
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


  t3d_mat4fp_from_srt_euler(mapMatFP,
      (float[3]){0.4f, 0.4f, 0.4f},
      (float[3]){rotBGAngleY, rotBGAngleX, 0},
      (float[3]){x_dist, y_dist, z_dist }
      // (float[3]){x_dist, y_dist, -10}
    );

  // t3d_mat4_to_fixed(mapMatFP, modelMap);




  rdpq_sync_tile();

  if (show_menu) {
    stats_draw_billboard();
    if (current_model != selection_counter - 1) {
      selection = selection_counter - 1;
      current_model = selection;
      if (current_model == 6){
        t3d_anim_set_playing(&modell.animWalk, true);
      }
      update_model(current_model);

    }
  }

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
  //I should to free all models
  t3d_model_free(modelMaps[current_model]);
  free_uncached(mapMatFP);
  t3d_destroy();
  display_close();
}

int main()
{
  lastTime = get_time_s() - (1.0f / 60.0f);

  game_init();
  while(1)
  {
    game_loop(deltaTime);
    check_controller_state();
  };

  game_cleanup();
  return 0;
}
