#include <libdragon.h>
#include <t3d/t3d.h>
#include <t3d/t3dmath.h>
#include <t3d/t3dmodel.h>
#include <stdlib.h>


// Main function
int main()
{
  // setup debug
  debug_init_isviewer();
	debug_init_usblog();
  joypad_init();
  // asset_init_compression(1);
	asset_init_compression(2);
  dfs_init(DFS_DEFAULT_LOCATION);
  display_init(RESOLUTION_320x240, DEPTH_16_BPP, 3, GAMMA_NONE, FILTERS_RESAMPLE_ANTIALIAS);
  rdpq_init();
  t3d_init((T3DInitParams){});
  T3DViewport viewport = t3d_viewport_create();
  display_context_t disp;


  T3DMat4 modelMat; // matrix for our model, this is a "normal" float matrix
  t3d_mat4_identity(&modelMat);

  // Now allocate a fixed-point matrix, this is what t3d uses internally.
  // Note: this gets DMA'd to the RSP, so it needs to be uncached.
  // If you can't allocate uncached memory, remember to flush the cache after writing to it instead.
  T3DMat4FP* modelMatFP = malloc_uncached(sizeof(T3DMat4FP));

  // camera
  // const T3DVec3 camPos = {{0, 90.0f, 10.0f}};
  const T3DVec3 camPos = {{0,10.0f,40.0f}};
  const T3DVec3 camTarget = {{0,0,0}};

  // ambient color
  uint8_t colorAmbient[4] = {80, 80, 100, 0xFF};
  uint8_t colorDir[4]     = {0xEE, 0xAA, 0xAA, 0xFF};


  // lighting direction
  T3DVec3 lightDirVec = {{-1.0f, 1.0f, 1.0f}};
  t3d_vec3_norm(&lightDirVec);

  // Load a model-file, this contains the geometry and some metadata
  T3DModel *model = t3d_model_load("rom:/model.t3dm");

  // rotation angle
  float posX = 0.0f;
  float posY = 0.0f;
  float rotAngleY = 0.0f;


  rspq_block_t *dplDraw = NULL;




  sprite_t *fulgore = {
    sprite_load("rom:/fulgore.sprite"),
  };

  // infinite loop
  for(;;)
  {
    joypad_inputs_t joypad = joypad_get_inputs(0);
    joypad_buttons_t btn = joypad_get_buttons_pressed(0);
    joypad_buttons_t btnHeld = joypad_get_buttons_held(0);

    float modelScale = 0.1f;

    t3d_viewport_set_projection(&viewport, T3D_DEG_TO_RAD(85.0f), 10.0f, 90.0f);
    // t3d_viewport_look_at(&viewport, &camPos, &camTarget, &(T3DVec3){{0,1,0}});
    t3d_viewport_look_at(&viewport, &camPos, &camTarget, &(T3DVec3){{0,1,0}});


    // slowly rotate model, for more information on matrices and how to draw objects
    // see the example: "03_objects"

    //https://n64squid.com/homebrew/libdragon/controllers/
    if (btn.a || btnHeld.a) {
      rotAngleY = rotAngleY + 0.05f;
      t3d_mat4_from_srt_euler(&modelMat,
        (float[3]){modelScale, modelScale, modelScale},
        (float[3]){0.0f,rotAngleY,0.0f},
        (float[3]){posX,posY,0}
      );
    }

    if (joypad.stick_x || joypad.stick_y) {
     //move model if x
      posX += (float)joypad.stick_x * 0.05f;
      posY += (float)joypad.stick_y * -0.05f;
      t3d_mat4_from_srt_euler(&modelMat,
        (float[3]){modelScale, modelScale, modelScale},
        (float[3]){0.0f,rotAngleY,0.0f},
        (float[3]){posX,posY,0}
      );
    }

    t3d_mat4_to_fixed(modelMatFP, &modelMat);



    // ======== Draw ======== //
    rdpq_attach(display_get(), display_get_zbuf());
    t3d_frame_start();
    t3d_viewport_attach(&viewport);

    t3d_screen_clear_color(RGBA32(100, 80, 80, 0xFF));
    t3d_screen_clear_depth();

    t3d_light_set_ambient(colorAmbient);
    t3d_light_set_directional(0, colorDir, &lightDirVec);
    t3d_light_set_count(1);

    // you can use the regular rdpq_* functions with t3d.
    // In this example, the colored-band in the 3d-model is using the prim-color,
    // even though the model is recorded, you change it here dynamically.

    // rdpq_set_prim_color(get_rainbow_color(rotAngle * 0.42f));

    if(!dplDraw) {
      rspq_block_begin();

      t3d_matrix_push(modelMatFP);
      // Draw the model, material settings (e.g. textures, color-combiner) are handled internally
      t3d_model_draw(model);
      t3d_matrix_pop(1);
      dplDraw = rspq_block_end();
    }

    // for the actual draw, you can use the generic rspq-api.
    rspq_block_run(dplDraw);
    rdpq_detach_show();
    joypad_poll();
  }

  // Initialize the graphics context
  t3d_destroy();
  return 0;
}



