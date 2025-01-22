#include "controller.h"

#include <malloc.h>
#include <libdragon.h>
#include "menu.h"
#include "models.h"


float x_dist = 0.0f;
float y_dist = 0.0f;
float z_dist = 0.0f;
float rotBGAngleY = 0.0f;
float rotBGAngleX = 0.0f;
bool change_model = false;

joypad_inputs_t joypad;

void check_controller_state(void) {

    joypad_buttons_t btnPressed = joypad_get_buttons_pressed(JOYPAD_PORT_1);
    joypad_buttons_t btnHeld = joypad_get_buttons_held(JOYPAD_PORT_1);
    // joypad_buttons_t btnReleased = joypad_get_buttons_released(JOYPAD_PORT_1);

  if (btnPressed.start) {
    show_menu = !show_menu;
  }

    if (show_menu) {
      if (btnPressed.z) {
        change_model = !change_model;
      }

      if (btnPressed.d_down && not_at_menu_end) {
        selector_start_pos += 10;
      }

       if (btnPressed.d_up && (selector_start_pos != menu_center_y)) {
        selector_start_pos -= 10;
      }
    }

  if (!show_menu) {
    if (btnHeld.d_up && btnHeld.r){
      y_dist += 0.5;
    }
     if (btnHeld.d_down && btnHeld.r){
      y_dist -= 0.5;
    }
    if (btnHeld.d_up && !btnHeld.r){
       z_dist -= 0.5;
    }
    if (btnHeld.d_down && !btnHeld.r){
      z_dist += 0.5;
    }
    if (btnHeld.d_right && !btnHeld.r){
      x_dist += 0.5;
    }
    if (btnHeld.d_left && !btnHeld.r){
      x_dist -= 0.5;
    }

    if (btnHeld.c_up) {
      rotBGAngleY += 0.05;
    }

    if (btnHeld.c_down) {
      rotBGAngleY -= 0.05;
    }

    if (btnHeld.c_right) {
      rotBGAngleX += 0.05;
    }

    if (btnHeld.c_left) {
      rotBGAngleX -= 0.05;
    }
  }




  // for debug
    // if (btnHeld.b) {

    // } else if (btnReleased.b) {

    // }

    // if (btnHeld.a) {

    // }

    // if (btnHeld.z) {

    // }




}
