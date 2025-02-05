#include <libdragon.h>
#include <t3d/t3d.h>
#include <t3d/t3dmath.h>
#include <t3d/t3dmodel.h>
#include <t3d/t3dskeleton.h>
#include <t3d/t3danim.h>
#include <t3d/t3ddebug.h>


/**
 * Example project showcasing the usage of the animation system.
 * This includes instancing animations, blending animations, and controlling playback.
 */

bool isJump = false;
bool isRun = false;
bool shotFired = false;
bool aPress = false;
bool playerMove = true;

float shotTimer = 0.0f;
int shotCount = 0;
bool closeUp = false;


#define SHOT_TIME_START   0.0f
#define SHOT_TIME_END     0.222f

float get_time_s() {
  return (float)((double)get_ticks_us() / 1000000.0);
}
 int signum(int num) {
  return (num > 0) - (num < 0);
 };

int main()
{
  debug_init_isviewer();
  debug_init_usblog();
  asset_init_compression(2);

  dfs_init(DFS_DEFAULT_LOCATION);

  display_init(RESOLUTION_320x240, DEPTH_16_BPP, 3, GAMMA_NONE, FILTERS_RESAMPLE_ANTIALIAS);

  rdpq_init();
  joypad_init();

  t3d_init((T3DInitParams){});
  rdpq_text_register_font(FONT_BUILTIN_DEBUG_MONO, rdpq_font_load_builtin(FONT_BUILTIN_DEBUG_MONO));
  T3DViewport viewport = t3d_viewport_create();

  T3DMat4FP* modelMatFP = malloc_uncached(sizeof(T3DMat4FP));

  T3DMat4FP* mapMatFP = malloc_uncached(sizeof(T3DMat4FP));

  T3DMat4FP* cubeMatFP = malloc_uncached(sizeof(T3DMat4FP));

  t3d_mat4fp_from_srt_euler(mapMatFP, (float[3]){0.3f, 0.3f, 0.3f}, (float[3]){0, 0, 0}, (float[3]){0, 0, -10});

  T3DVec3 camPos = {{0, 45.0f, 90.0f}};
  T3DVec3 camTarget = {{0, 0,-10}};

  T3DVec3 lightDirVec = {{1.0f, 1.0f, 1.0f}};
  t3d_vec3_norm(&lightDirVec);

  // uint8_t colorAmbient[4] = {0xCC, 0xCC, 0xCC, 0x00};
  // uint8_t colorDir[4]     = {0xff, 0xff, 0xff, 0xff};
  uint8_t colorAmbient[4] = {0xaa, 0xaa, 0xaa, 0xaa};
  uint8_t colorDir[4]     = {0xdd, 0xdd, 0xdd, 0xdd};

  // uint8_t colorDir[4]     = {0xFF, 0xAA, 0xAA, 0xFF};

  T3DModel *modelMap = t3d_model_load("rom:/grid2.t3dm");
  T3DModel *modelShadow = t3d_model_load("rom:/shadow2.t3dm");

  // Model Credits: Quaternius (CC0) https://quaternius.com/packs/easyenemy.html

  // looks good after adjust rotation
  // T3DModel *model = t3d_model_load("rom:/sammy3.t3dm");
  T3DModel *model = t3d_model_load("rom:/otest.t3dm");
  T3DModel *cube = t3d_model_load("rom:/testcube.t3dm");


  //correct rotation bad animation
  //T3DModel *model = t3d_model_load("rom:/samis2.t3dm");



  // First instantiate skeletons, they will be used to draw models in a specific pose
  // And serve as the target for animations to modify
  T3DSkeleton skel = t3d_skeleton_create(model);
  T3DSkeleton skelBlend = t3d_skeleton_clone(&skel, false); // optimized for blending, has no matrices

  // Now create animation instances (by name), the data in 'model' is fixed,
  // whereas 'anim' contains all the runtime data.
  // Note that tiny3d internally keeps no track of animations, it's up to the user to manage and play them.
  T3DAnim animIdle = t3d_anim_create(model, "idleShoot");
  t3d_anim_set_looping(&animIdle, true); // don't loop this animation
  t3d_anim_set_playing(&animIdle, true); // start in a paused state

  t3d_anim_attach(&animIdle, &skel); // tells the animation which skeleton to modify

  T3DAnim animWalk = t3d_anim_create(model, "walk");
  t3d_anim_attach(&animWalk, &skelBlend);

  T3DAnim animRun = t3d_anim_create(model, "run");
  t3d_anim_attach(&animRun, &skelBlend);

  T3DAnim animJump = t3d_anim_create(model, "jump2");
  t3d_anim_set_looping(&animJump, false);
  t3d_anim_attach(&animJump, &skelBlend);

  // T3DAnim animIdleJump = t3d_anim_create(model, "idle_jump");
  // t3d_anim_set_looping(&animIdleJump, false);
  // t3d_anim_attach(&animIdleJump, &skelBlend);

  T3DAnim animCurrent = animIdle;



  // multiple animations can attach to the same skeleton, this will NOT perform any blending
  // rather the last animation that updates "wins", this can be useful if multiple animations touch different bones
  // T3DAnim animAttack = t3d_anim_create(model, "Snake_Attack");
  // t3d_anim_set_looping(&animAttack, false); // don't loop this animation
  // t3d_anim_set_playing(&animAttack, false); // start in a paused state
  // t3d_anim_attach(&animAttack, &skel);

  rspq_block_begin();
    t3d_matrix_push(modelMatFP);
    //rdpq_set_prim_color(RGBA32(255, 255, 255, 255));
    t3d_model_draw_skinned(model, &skel); // as in the last example, draw skinned with the main skeleton
    rdpq_set_prim_color(RGBA32(00, 00, 00, 120));
    t3d_model_draw(modelShadow);

    t3d_matrix_pop(1);
  rspq_block_t *dplSnake = rspq_block_end();

  rspq_block_begin();
    t3d_matrix_push(mapMatFP);
    rdpq_set_prim_color(RGBA32(00, 00, 00, 120));
    t3d_model_draw(modelMap);
    t3d_matrix_pop(1);
  rspq_block_t *dplMap = rspq_block_end();

  rspq_block_begin();
    t3d_matrix_push(cubeMatFP);
    rdpq_set_prim_color(RGBA32(00, 00, 00, 120));
    t3d_model_draw(cube);
    t3d_matrix_pop(1);
  rspq_block_t *dplCube = rspq_block_end();


  float lastTime = get_time_s() - (1.0f / 60.0f);
  rspq_syncpoint_t syncPoint = 0;

  T3DVec3 moveDir = {{0,0,0}};
  T3DVec3 playerPos = {{0,0.15f,0}};
  T3DVec3 lastPlayerPos = playerPos;

  T3DVec3 cubePos = {{0,playerPos.v[1]+26,0}};


  float rotY = 0.0f;
  float rotX = -1.50f;
  float rotZ = 0.0f;
  float currSpeed = 0.0f;
  float animBlend = 0.0f;
  bool isAttack = false;
  float shotSpeed = 0.0f;

  for(;;)
  {
    // ======== Update ======== //
    joypad_poll();

    float newTime = get_time_s();
    float deltaTime = newTime - lastTime;
    lastTime = newTime;

    joypad_inputs_t joypad = joypad_get_inputs(JOYPAD_PORT_1);
    joypad_buttons_t btn = joypad_get_buttons_pressed(JOYPAD_PORT_1);
    joypad_buttons_t btnHeld = joypad_get_buttons_held(JOYPAD_PORT_1);


    T3DVec3 newDir = {{
      (float)joypad.stick_x * 0.05f, 0, // adjust for swapped y and z axis
      -(float)joypad.stick_y * 0.05f
    }};
    float speed = sqrtf(t3d_vec3_len2(&newDir));

    // Player Attack
    // if((btn.a || btn.b) && !animAttack.isPlaying) {
    //   t3d_anim_set_playing(&animAttack, true);
    //   t3d_anim_set_time(&animAttack, 0.0f);
    //   isAttack = true;
    // }

    if(btnHeld.z ) {
      closeUp = true;
      playerMove = false;
    } else {
      closeUp = false;
      playerMove = true;
    }




    // Player movement
    if(speed > 0.15f) {
      newDir.v[0] /= speed;
      newDir.v[2] /= speed;

      moveDir = newDir;


      // what does this return?
      float newAngle = atan2f(moveDir.v[0], moveDir.v[2]);
      //rotY = t3d_lerp_angle(0.0f, newAngle, 0.25f);
      rotY = t3d_lerp_angle(0.0f, 1.0f, newAngle); //adjusting for swapped y and z axis
      currSpeed = t3d_lerp(currSpeed, speed * 0.15f, 0.15f);
    } else {
      currSpeed *= 0.8f;
    }

    if (speed >= 0.6f) {
      t3d_anim_set_playing(&animRun, true);
      t3d_anim_set_looping(&animRun, true);
      isRun = true;
    } else {
      isRun = false;
    }

    if (btn.a && !isJump) {
      aPress = true;
      isJump = true;
      t3d_anim_set_playing(&animJump, true);
      t3d_anim_set_time(&animJump, 0.0f);
    } else {
      aPress = false;
    }

     if (btn.b && shotCount < 3 ) {
        shotCount++;
        shotFired = true;
        shotTimer = 0.0f;
     }


    if (isRun) {
       currSpeed = 1.5f;
      // currSpeed = t3d_lerp(currSpeed, speed * 0.35f, 0.35f);
    } else {
      currSpeed = t3d_lerp(currSpeed, speed * 0.15f, 0.15f);
    }

    if (isJump) {
      currSpeed = 1.5f;
      //playerPos.v[1] += 0.08f;
    } else {
      playerPos.v[1] = 0.15f;
    }

    //height test
    if (playerPos.v[0] < 0.0f && playerPos.v[0] > -130.0f) {
      if (playerPos.v[2] < -10.0f && playerPos.v[2] > -63.0f) {
        playerPos.v[1] = 18.0f;
      }
    }


    // use blend based on speed for smooth transitions
    animBlend = currSpeed / 0.51f;
    if(animBlend > 1.0f)animBlend = 1.0f;

    // move player...
    if( playerMove ) {
      playerPos.v[0] += moveDir.v[0] * currSpeed;
      playerPos.v[2] += moveDir.v[2] * currSpeed;
      lastPlayerPos = playerPos;
    } else {
      playerPos = lastPlayerPos;
    }


    shotSpeed = 10.0f;

    if (shotFired) {
      shotTimer += deltaTime;
      if (shotTimer > SHOT_TIME_START && shotTimer < SHOT_TIME_END) {
        cubePos.v[0] += moveDir.v[0] * shotSpeed;
        cubePos.v[1] = playerPos.v[1]+26;
        cubePos.v[2] += moveDir.v[2] * shotSpeed;
      } else {
        shotCount --;
        shotFired = false;
      }
    } else {
      cubePos.v[0] = playerPos.v[0] - (4*signum(moveDir.v[0])); // good case for signum
      cubePos.v[1] = playerPos.v[1]+26;
      cubePos.v[2] = playerPos.v[2];
    }


    // ...and limit position inside the box
    // const float BOX_SIZE = 140.0f;
    const float BOX_SIZE = 240.0f;
    if(playerPos.v[0] < -BOX_SIZE)playerPos.v[0] = -BOX_SIZE;
    if(playerPos.v[0] >  BOX_SIZE)playerPos.v[0] =  BOX_SIZE;
    if(playerPos.v[2] < -BOX_SIZE)playerPos.v[2] = -BOX_SIZE;
    if(playerPos.v[2] >  BOX_SIZE)playerPos.v[2] =  BOX_SIZE;

    // position the camera behind the player
    camTarget = playerPos;
    camTarget.v[1] = 0.150f;

    if (closeUp) {
      camTarget.v[2] -= 65;
      camTarget.v[1] = -10;
    } else {
      camTarget.v[2] -= 25;
    }

    camPos.v[0] = camTarget.v[0];
    camPos.v[1] = camTarget.v[1] + 45;
    camPos.v[2] = camTarget.v[2] + 65;

    // t3d_viewport_set_projection(&viewport, T3D_DEG_TO_RAD(90.0f), 10.0f, 150.0f);

    t3d_viewport_set_projection(&viewport, T3D_DEG_TO_RAD(90.0f), 10.0f, 150.0f);
    t3d_viewport_look_at(&viewport, &camPos, &camTarget, &(T3DVec3){{0,1,0}});


    // Update the animation and modify the skeleton, this will however NOT recalculate the matrices
    t3d_anim_update(&animIdle, deltaTime);

    t3d_anim_set_speed(&animWalk, animBlend + 0.15f);
    t3d_anim_update(&animWalk, deltaTime);

    if (speed >= 0.6f) {
      // t3d_anim_set_speed(&animRun, animBlend + 0.25f);
      t3d_anim_update(&animRun, deltaTime);
    }


    if (isJump ) {
      // t3d_anim_set_speed(&animJump, animBlend + 0.35f);
      t3d_anim_update(&animJump, deltaTime);
      if (!animJump.isPlaying)isJump = false;
    }


    // if(isAttack) {
    //   t3d_anim_update(&animAttack, deltaTime); // attack animation now overrides the idle one
    //   if(!animAttack.isPlaying)isAttack = false;
    // }

    // We now blend the walk animation with the idle/attack one
    t3d_skeleton_blend(&skel, &skel, &skelBlend, animBlend);

    if(syncPoint)rspq_syncpoint_wait(syncPoint); // wait for the RSP to process the previous frame

    // Now recalc. the matrices, this will cause any model referencing them to use the new pose
    t3d_skeleton_update(&skel);

    // Update player matrix

    // this is where player movement is set
    t3d_mat4fp_from_srt_euler(modelMatFP,
      (float[3]){0.25f, 0.25f, 0.25f},
      //(float[3]){0.006f, 0.006f, 0.006f},
      //(float[3]){0.0f, 0.0f, rotY},
      (float[3]){rotX, 0.0f, rotY}, // rotation - puts the model upright


      //(float[3]){0.1f, 0.1f, 0.1f},
      //(float[3]){0.0f,-rotY,0.0f},
      playerPos.v
    );

    t3d_mat4fp_from_srt_euler(cubeMatFP,
      (float[3]){0.0125f, 0.0125f, 0.0125f},
      //(float[3]){0.006f, 0.006f, 0.006f},
      //(float[3]){0.0f, 0.0f, 0.0f},
      (float[3]){rotX, 0.0f, rotY}, // rotation - puts the model upright
      cubePos.v
    );


    // ======== Draw (3D) ======== //
    rdpq_attach(display_get(), display_get_zbuf());
    t3d_frame_start();
    t3d_viewport_attach(&viewport);

    //t3d_screen_clear_color(RGBA32(255, 255, 255, 0xFF));
    t3d_screen_clear_color(RGBA32(00, 00, 00, 0xFF)); // black bg


    // t3d_screen_clear_color(RGBA32(224, 180, 96, 0xFF));
    t3d_screen_clear_depth();

    t3d_light_set_ambient(colorAmbient);
    t3d_light_set_directional(0, colorDir, &lightDirVec);
    t3d_light_set_count(1);

    rspq_block_run(dplMap);
    rspq_block_run(dplSnake);

    if (shotFired) {
      rspq_block_run(dplCube);
    }
    //rspq_block_run(dplCube);

    syncPoint = rspq_syncpoint_new();

    // ======== Draw (UI) ======== //
    float posX = 16;
    float posY = 24;

    rdpq_sync_pipe();
    // rdpq_text_printf(NULL, FONT_BUILTIN_DEBUG_MONO, posX, posY, "[A] Attack: %d", isAttack);

    posY = 150;
    rdpq_text_printf(NULL, FONT_BUILTIN_DEBUG_MONO, posX, posY, "x %f", playerPos.v[0]); posY += 10;
    rdpq_text_printf(NULL, FONT_BUILTIN_DEBUG_MONO, posX, posY, "y %f", playerPos.v[1]); posY += 10;
    rdpq_text_printf(NULL, FONT_BUILTIN_DEBUG_MONO, posX, posY, "z %f", playerPos.v[2]); posY += 10;
    rdpq_text_printf(NULL, FONT_BUILTIN_DEBUG_MONO, posX, posY, "Speeeeed: %.4f", currSpeed); posY += 10;
    rdpq_text_printf(NULL, FONT_BUILTIN_DEBUG_MONO, posX, posY, "Blend: %.4f", animBlend); posY += 10;

    rdpq_detach_show();
  }

  t3d_skeleton_destroy(&skel);
  t3d_skeleton_destroy(&skelBlend);

  t3d_anim_destroy(&animIdle);
  t3d_anim_destroy(&animWalk);
   t3d_anim_destroy(&animRun);
  t3d_anim_destroy(&animJump);
  // t3d_anim_destroy(&animAttack);

  t3d_model_free(model);
  t3d_model_free(modelMap);
  t3d_model_free(modelShadow);

  t3d_destroy();
  return 0;
}

