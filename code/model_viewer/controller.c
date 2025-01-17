#include "controller.h"
#include "fighter.h"
#include "shared.h"

#include <malloc.h>
#include <libdragon.h>


float angle_speed = 0.00095f;

float x_dist = 0.0f;
float rotBGAngleY = 0.0f;
float pos_speed = 1.25f;

int vel_x = 0;
int y_speed = 10.9f;
bool jump_peak = false;
joypad_inputs_t joypad;



void check_controller_state(void) {
    joypad_buttons_t btnPressed = joypad_get_buttons_pressed(JOYPAD_PORT_1);
    joypad_buttons_t btnHeld = joypad_get_buttons_held(JOYPAD_PORT_1);
    joypad_buttons_t btnReleased = joypad_get_buttons_released(JOYPAD_PORT_1);

    if (btnPressed.d_right || btnPressed.d_left)
    {
      fighter.walking = true;
      fighter.idle = false;
      if (btnPressed.d_right)
      {
        fighter.reverse_frame = false;
        fighter.backing_up = true;
        fighter.time = 0;
        fighter.spr_ndx = walk_start_index;

      } else if (btnPressed.d_left){
        //beginning of the backwards walking animation
        fighter.backing_up = true;
        fighter.reverse_frame = true;
        fighter.spr_ndx = 4;
        fighter.time = ANIM_FRAME_DELAY*ANIM_FULG_WALK_COL_MAX;
      }

    }

    if (btnHeld.d_right)
    {
      if (angle_speed < 0) {
        angle_speed = angle_speed * -1;
      }
      rotBGAngleY += angle_speed;
      x_dist -= 0.15;
      //if (posX < 320.0f) {
      if (posX < 320.0f) {
        vel_x = pos_speed;
        if (posX < 320.0f) {
          posX += vel_x;
        }

        if (posX > 160.0f && posX < 320.0f) {
          cx_val += 0.75;
          if (posX >= 205.0f) {
            current_background = background2;
            // cx_val = 160.0f;
            rdpq_sync_tile();
          }
        }


      }

    }
    if (btnHeld.d_left)
    {
      if (angle_speed > 0) {
        angle_speed = angle_speed * -1;
      }
      rotBGAngleY += angle_speed;
      x_dist += 0.15;

      if (posX > 0.0f) {
        vel_x = pos_speed;
        posX -= vel_x;
        if (205 > posX && posX > 160 ) {
          cx_val -= vel_x;
        }

        if (posX < 205) {
          current_background = background;
          rdpq_sync_tile();
        }
      }

    }

    if ((btnReleased.d_right || btnReleased.d_left) && !fighter.jumping)
    {
      fighter.idle = true;
      fighter.walking = false;
      vel_x = 0;
      fighter.spr_ndx = standing_start_index;
    }



    // jumping animation
    if (fighter.jumping && !jump_peak){

      if (vel_x == 0) {
        if (posY > 130.0f) {
          posY -= y_speed;
        } else {
            jump_peak = true;
        }
      }
      else if (vel_x != 0) {
        if (posY > 145.0f) {
          posY -= y_speed;
        } else {
          jump_peak = true;
        }
      }


    } else if (fighter.jumping && jump_peak) {
      if (posY < 240.0f) {
        posY += y_speed;

      } else if (posY >= 240.0f) {
        jump_peak = false;
        fighter.jumping = false;


        //fix
        frame_w = ANIM_FRAME_W;
        current_spritesheet = fulgoresheetv1;

        fighter.time = 0;

        if (vel_x == 0) {
          fighter.idle = true;
          fighter.spr_ndx = standing_start_index;
        } else {
          fighter.walking = true;
          fighter.spr_ndx = walk_start_index;
        }
      }
    }

    int current_x = posX;
    if (fighter.jumping) {
      if (btnHeld.d_right || btnPressed.d_right) {
        if (posX < current_x + 160) { // need to factor in the screen width
          posX += vel_x*3;
          // if(posX >= 320){
          //   current_background = background2;
          // }
        }
      }
      else if (btnHeld.d_left || btnPressed.d_left) {
        if (posX > current_x - 160) {
          posX -= vel_x*3;
        }
      }
    }



    if (btnPressed.d_up && !fighter.jumping){
      fighter.reverse_frame = false;

      //fix
      current_spritesheet = fulgorejump;


      fighter.time = 0;
      fighter.jumping = true;
      fighter.idle = false;
      fighter.walking = false;

      //fix
      frame_w = ANIM_JUMP_W;
      fighter.spr_ndx = jump_start_index;

    }
    else if (posY < 240.0f && fighter.jumping) {
      if (posY >= 240.0f && frame_w == ANIM_JUMP_W) {

        //fix
        frame_w = ANIM_FRAME_W;
        current_spritesheet = fulgoresheetv1;
        fighter.time = 0;
        fighter.jumping = false;
        jump_peak = false;

        if (vel_x == 0) {
          fighter.idle = true;
          fighter.spr_ndx = standing_start_index;
        } else {
          fighter.walking = true;
          fighter.spr_ndx = walk_start_index;
        }
      }
    }

  // for debug
    if (btnHeld.b) {
      //fix
      frame_delay = 30;
      frame = fighter.time/6;
    } else if (btnReleased.b) {
      frame_delay = 5;
    }

    if (btnHeld.a) {
      angle_speed = 0.01195f;
    } else {
      angle_speed = 0.00095f;
    }

    if (btnPressed.z) {
      //fix
      current_background = background2;

     // showbackground = !showbackground;
    }


    //fix
    if (btnPressed.c_up) {
      scale_x += .05;
      scale_y += .05;
    }
    if (btnPressed.c_down) {
      scale_x -= .05;
      scale_y -= .05;
    }
}
