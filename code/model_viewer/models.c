#include <libdragon.h>
#include <t3d/t3d.h>
#include <t3d/t3dmath.h>
#include <t3d/t3dmodel.h>
#include <t3d/t3dskeleton.h>
#include <t3d/t3danim.h>
#include <t3d/t3ddebug.h>

#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include "models.h"

T3DModel* modelMaps[MODEL_MAPS_LENGTH];
T3DModel* model;

model_data modell;

void load_model(void) {

  model = t3d_model_load("rom:/snakeOG.t3dm");
  modell.animBlend = 0.0f;
}

void load_models(void) {
    modelMaps[0] = t3d_model_load("rom:/samus_vlp3.t3dm");
    modelMaps[1] = t3d_model_load("rom:/samus_vlp2.t3dm");
    modelMaps[2] = t3d_model_load("rom:/snake2.t3dm");
    modelMaps[3] = t3d_model_load("rom:/snakeOG.t3dm");
    modelMaps[4] = t3d_model_load("rom:/knight.t3dm");
    modelMaps[5] = t3d_model_load("rom:/samus_anim2.t3dm");
    modelMaps[6] = model;
}






