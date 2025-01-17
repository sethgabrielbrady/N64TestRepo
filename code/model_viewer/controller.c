#include "controller.h"

#include <malloc.h>
#include <libdragon.h>



float x_dist = 0.0f;
float y_dist = 0.0f;
float rotBGAngleY = 0.0f;
float rotBGAngleX = 0.0f;

joypad_inputs_t joypad;

void check_controller_state(void) {
    joypad_buttons_t btnPressed = joypad_get_buttons_pressed(JOYPAD_PORT_1);
    joypad_buttons_t btnHeld = joypad_get_buttons_held(JOYPAD_PORT_1);
    joypad_buttons_t btnReleased = joypad_get_buttons_released(JOYPAD_PORT_1);


    if (btnHeld.d_up){
      y_dist += 0.5;
    }
    if (btnHeld.d_down){
      y_dist -= 0.5;
    }
    if (btnHeld.d_right){
      x_dist += 0.5;

    }
    if (btnHeld.d_left){
      x_dist -= 0.5;
    }
    if (btnPressed.c_up) {
      rotBGAngleY += 0.5;
    }
    if (btnPressed.c_down) {
      rotBGAngleY -= 0.5;
    }
    if (btnPressed.c_right) {
      rotBGAngleX += 0.5;
    }
    if (btnPressed.c_left) {
      rotBGAngleX -= 0.5;
    }


  // for debug
    // if (btnHeld.b) {

    // } else if (btnReleased.b) {

    // }

    // if (btnHeld.a) {

    // }

    // if (btnPressed.z) {

    // }




}
