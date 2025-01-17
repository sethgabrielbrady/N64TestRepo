#ifndef CONTROLLER_H
#define CONTROLLER_H

#include <libdragon.h>

extern joypad_inputs_t joypad;
extern float x_dist;
extern float y_dist;
extern float rotBGAngleY;
extern float rotBGAngleX;

void check_controller_state();


#endif //controller.h