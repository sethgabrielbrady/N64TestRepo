#include <libdragon.h>
#include <t3d/t3dmodel.h>
#include <stdint.h>
#include <string.h>
#include <stdio.h>


#include "models.h"

T3DModel* modelMaps[MODEL_MAPS_LENGTH];

void load_models(void) {
    modelMaps[0] = t3d_model_load("rom:/samus3.t3dm");
    modelMaps[1] = t3d_model_load("rom:/samus5.t3dm");
}



