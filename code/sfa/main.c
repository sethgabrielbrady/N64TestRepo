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


// T3DViewport viewport;
// T3DMat4FP* mapMatFP;
// T3DModel *modelMap;
// T3DVec3 camPos;
// T3DVec3 camTarget;
// T3DVec3 lightDirVec;

// surface_t *depthBuffer;
// rspq_block_t *dplMap;
rspq_syncpoint_t syncPoint;
display_context_t disp;
surface_t *fs;

resolution_t custom_res = {
    .width = 388,
    .height = 240,
    .interlaced = false
};


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

  // custom resolution


  display_init(custom_res, DEPTH_16_BPP, 3, GAMMA_NONE, FILTERS_RESAMPLE);
  // display_init(RESOLUTION_320x240, DEPTH_16_BPP, 3, GAMMA_NONE, FILTERS_RESAMPLE);

  load_sprites();
  load_font();
}


void update(void)
{
  // update to take in fighter data
  update_frame_direction(fighter);
  fighter_state_machine(fighter);
}

void game_loop(float deltaTime)
{

  // // ======== Draw (3D) ======== //
  //rdpq_attach(display_get(), depthBuffer);
  rdpq_attach(display_get(), NULL);

  syncPoint = rspq_syncpoint_new();

  // add sprite
  rdpq_set_mode_standard();
  rdpq_mode_combiner(RDPQ_COMBINER_TEX);
  rdpq_mode_blender(RDPQ_BLENDER_MULTIPLY);


  update_frame_direction(fighter);
  add_background(scroll_x);

  rdpq_sync_tile();
  updateFighterBlit(); // sprite update
  if (show_stats) {
    stats_draw_billboard(fighter);
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
  // rspq_block_free(dplMap);
  // t3d_model_free(modelMap);
  // free_uncached(mapMatFP);
  sprite_free(current_spritesheet);
  sprite_free(background);
  // t3d_destroy();
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
