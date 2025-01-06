#ifndef CONTROLLER_H
#define CONTROLLER_H

#include <libdragon.h>

extern joypad_inputs_t joypad;

extern float angle_speed;
extern float posX;
extern float x_dist;
extern float rotBGAngleY;
extern float pos_speed;
extern float posY;
extern int vel_x;
extern int y_speed;
extern bool jump_peak;
extern bool show_stats;
extern float scroll_x;

void check_controller_state();


#endif //controller.h