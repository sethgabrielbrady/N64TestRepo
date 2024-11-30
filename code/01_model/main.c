#include <libdragon.h>
#include <t3d/t3d.h>
#include <t3d/t3dmath.h>
#include <t3d/t3dmodel.h>

color_t get_rainbow_color(float s) {
  float r = fm_sinf(s + 0.0f) * 127.0f + 128.0f;
  float g = fm_sinf(s + 2.0f) * 127.0f + 128.0f;
  float b = fm_sinf(s + 4.0f) * 127.0f + 128.0f;
  return RGBA32(r, g, b, 255);
}

/**
 * This is called once during the `t3d_model_draw_custom` call when a placeholder is encountered.
 * You are free to add any logic here, in this case we simply upload a surface directly.
 * See the example `06_offscreen` for a more complex example.
 */
void dynamic_tex_cb(void* userData, const T3DMaterial* material, rdpq_texparms_t *tileParams, rdpq_tile_t tile) {
  if(tile != TILE0)return; // we only want to set one texture
  // 'userData' is a value you can pass into 't3d_model_draw_custom', this can be any value or struct you want...
  surface_t* surface = (surface_t*)userData; // ...in this case it is a surface pointer

  rdpq_sync_tile();
  rdpq_mode_tlut(TLUT_NONE);
  rdpq_tex_upload(TILE0, surface, NULL);
}

/**
 * Simple example with a 3d-model file created in blender.
 * This uses the builtin model format for loading and drawing a model.
 */
int main()
{
	debug_init_isviewer();
	debug_init_usblog();
	asset_init_compression(2);

  dfs_init(DFS_DEFAULT_LOCATION);

  display_init(RESOLUTION_320x240, DEPTH_16_BPP, 3, GAMMA_NONE, FILTERS_RESAMPLE_ANTIALIAS);

  rdpq_init();

  // Create a placeholder texture, this is used during recording of the model.
  // This allows us to only record the model once, and then change the texture later on.
  surface_t placeholder = surface_make_placeholder_linear(1, FMT_RGBA16, 32, 32);

  // Now load the actual textures, those are later set to be used to fill placeholders
  sprite_t* spritesWarn[2] = {
    sprite_load("rom:/warn00.rgba16.sprite"),
    sprite_load("rom:/warn01.rgba16.sprite"),
  };

  t3d_init((T3DInitParams){});
  T3DViewport viewport = t3d_viewport_create();

  T3DMat4 modelMat; // matrix for our model, this is a "normal" float matrix
  t3d_mat4_identity(&modelMat);

  // Now allocate a fixed-point matrix, this is what t3d uses internally.
  // Note: this gets DMA'd to the RSP, so it needs to be uncached.
  // If you can't allocate uncached memory, remember to flush the cache after writing to it instead.
  T3DMat4FP* modelMatFP = malloc_uncached(sizeof(T3DMat4FP));

  const T3DVec3 camPos = {{0,10.0f,40.0f}};
  const T3DVec3 camTarget = {{0,0,0}};

  uint8_t colorAmbient[4] = {80, 80, 100, 0xFF};
  uint8_t colorDir[4]     = {0xEE, 0xAA, 0xAA, 0xFF};

  T3DVec3 lightDirVec = {{-1.0f, 1.0f, 1.0f}};
  t3d_vec3_norm(&lightDirVec);

  // Load a model-file, this contains the geometry and some metadata
  T3DModel *model = t3d_model_load("rom:/model.t3dm");


  float rotAngle = 0.0f;
  // rspq_block_t *dplDraw = NULL;

  for(;;)
  {
    // ======== Update ======== //
    rotAngle -= 0.02f;
    float modelScale = 0.1f;

    t3d_viewport_set_projection(&viewport, T3D_DEG_TO_RAD(85.0f), 10.0f, 150.0f);
    t3d_viewport_look_at(&viewport, &camPos, &camTarget, &(T3DVec3){{0,1,0}});

    // slowly rotate model, for more information on matrices and how to draw objects
    // see the example: "03_objects"
    t3d_mat4_from_srt_euler(&modelMat,
      (float[3]){modelScale, modelScale, modelScale},
      (float[3]){0.0f, rotAngle*0.2f, rotAngle},
      (float[3]){0,0,0}
    );
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
    rdpq_set_prim_color(get_rainbow_color(rotAngle * 0.42f));

    rspq_block_begin();

    t3d_matrix_push(modelMatFP);
    // Draw the model, material settings (e.g. textures, color-combiner) are handled internally
    // t3d_model_draw(model);
      t3d_model_draw_custom(model, (T3DModelDrawConf){
        .userData = &placeholder, // custom data you want to have in the callback (optional)
        .dynTextureCb = dynamic_tex_cb, // your callback (this pointer is only used for this call and is not saved)
        // .matrices = skeleton->boneMatricesFP // if this is a skinned mesh, set the bone matrices here too
    });
    t3d_matrix_pop(1);
    rspq_block_t *dplDraw = rspq_block_end();

    rdpq_set_lookup_address(1, spritesWarn[1]->data);
    // for the actual draw, you can use the generic rspq-api.
    rspq_block_run(dplDraw);
    rdpq_detach_show();
  }

  t3d_destroy();
  return 0;
}
