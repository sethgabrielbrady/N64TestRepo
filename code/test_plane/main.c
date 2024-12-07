
// we may want to use a plane like the font_billboard in snake3d
// use rdpq_sprite_blit to draw the sprite -- see tiny3d/examples/99_testscene/main.c

#include <libdragon.h>
#include <t3d/t3d.h>
#include <t3d/t3dmath.h>
#include <t3d/t3dmodel.h>
#include <t3d/t3ddebug.h>
#include <stdlib.h>


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
float posX = 0.0f;
float posY = 105.0f;
float rotAngleY = 0.0f;
float rotAngleYCopy = 0.0f;
joypad_inputs_t joypad;


typedef struct rdpq_blitparms_s {
	rdpq_tile_t tile;
	int s0;
	int t0;
	int width;
	int height;
	bool flip_x;
	bool flip_y;

	int cx;
	int cy;
	float scale_x;
	float scale_y;
	float theta;

	bool filtering;
	int nx;
	int ny;
} rdpq_blitparms_s;

rdpq_blitparms_s params = {
  // .tile = RDPQ_TILE_0,
  .s0 = 0,
  .t0 = 0,
  .width = 0,
  .height = 0,
  .flip_x = false,
  .flip_y = false,
  .cx = 0,
  .cy = 0,
  .scale_x = 1.0f,
  .scale_y = 1.0f,
  .theta = 0.0f,
  .filtering = false,
  .nx = 0,
  .ny = 0,
};

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

  fulgore = sprite_load("rom:/fulgstand2.sprite");

  modelMap = t3d_model_load("rom:/map.t3dm");

  rspq_block_begin();
    t3d_matrix_push(mapMatFP);
    rdpq_set_prim_color(RGBA32(255, 255, 255, 255));
    t3d_model_draw(modelMap);
    t3d_matrix_pop(1);
  dplMap = rspq_block_end();

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
  // joypad_inputs_t joypad = joypad_get_inputs(0);
  joypad_buttons_t btn = joypad_get_buttons_pressed(0);
  joypad_buttons_t btnHeld = joypad_get_buttons_held(0);


  if (btn.d_left || btnHeld.d_left) {
    posX += -1.0f;
    rotAngleY += -0.00095f;
  }
  if (btn.d_right || btnHeld.d_right) {
    posX += 1.0f;
    rotAngleY += 0.00095f;
  }

if (rotAngleY != rotAngleYCopy) {
  t3d_mat4fp_from_srt_euler(mapMatFP,
    (float[3]){0.3f, 0.3f, 0.3f},
    (float[3]){0, rotAngleY, 0},
    (float[3]){0, 0, -10}
  );
  rotAngleYCopy = rotAngleY;
}


  rdpq_sprite_blit(fulgore, posX, posY, &params);




  //rspq_wait();
  rdpq_sync_tile();
  rdpq_sync_pipe(); // Hardware crashes otherwise
  rdpq_detach_show();
  joypad_poll();
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

  }

  game_cleanup();
  return 0;
}

