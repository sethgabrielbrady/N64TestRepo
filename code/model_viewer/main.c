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

#include "controller.h"
#include "stats.h"


T3DViewport viewport;
T3DMat4FP* mapMatFP;
T3DModel *modelMap;
T3DModel *modelMap2;


// T3DMat4 modelMap; // matrix for our model, this is a "normal" float matrix
T3DVec3 camPos;
T3DVec3 camTarget;
T3DVec3 lightDirVec;

surface_t *depthBuffer;
rspq_block_t *dplMap;
rspq_syncpoint_t syncPoint;
display_context_t disp;
surface_t *fs;

bool last_model = false;



void game_init(void)
{
  // initialize various components
  debug_init_isviewer();
  debug_init_usblog();

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


  load_font();

  modelMap = t3d_model_load("rom:/samus3.t3dm");
  modelMap2 = t3d_model_load("rom:/samus5.t3dm");

  rspq_block_begin();
  t3d_matrix_push(mapMatFP);
  t3d_model_draw(modelMap);
  t3d_matrix_pop(1);
  dplMap = rspq_block_end();

}

void update() {

}

void update_model(void)
{

  free_uncached(mapMatFP);

  if (modelMap != modelMap2) {
    modelMap = modelMap2;
  } else {
    modelMap = t3d_model_load("rom:/samus3.t3dm");

  }

  rspq_block_begin();
  t3d_matrix_push(mapMatFP);
  t3d_model_draw(modelMap);
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
    if (last_model != change_model) {
      update_model();
      last_model = change_model;
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
  t3d_model_free(modelMap);
  free_uncached(mapMatFP);
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
