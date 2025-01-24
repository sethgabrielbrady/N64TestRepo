#ifndef MODELS_H
#define MODELS_H

#include <t3d/t3d.h>
#include <t3d/t3dmath.h>
#include <t3d/t3dmodel.h>
#include <t3d/t3dskeleton.h>
#include <t3d/t3danim.h>
#include <t3d/t3ddebug.h>


#define MODEL_MAPS_LENGTH 7
extern T3DModel* modelMaps[MODEL_MAPS_LENGTH];
extern T3DModel *model;

typedef struct
{
  T3DSkeleton skelBlend;
  T3DSkeleton skel;
  T3DAnim animWalk;
  float animBlend;
} model_data;

extern model_data modell;

void load_models(void);
void load_model(void);


#endif




