#ifndef CONTROLLER_H
#define CONTROLLER_H

#include <libdragon.h>
#include "stats.h"

extern joypad_inputs_t joypad;
extern float x_dist;
extern float y_dist;
extern float z_dist;
extern float rotBGAngleY;
extern float rotBGAngleX;
extern bool change_model;

void check_controller_state();


#endif //controller.h