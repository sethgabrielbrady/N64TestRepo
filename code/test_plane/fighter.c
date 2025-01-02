#include "fighter.h"
#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>
#include <math.h>
#include <stdint.h>

#include "controller.h"

int frame_delay = 6;
int walk_start_index = 2;
int standing_start_index = 0;
int jump_start_index = 2;
int max_frame = 0;

fighter_data fighter;


void fighter_init(fighter_data data)
{
    fighter.reverse_frame = false;
    fighter.spr_ndx = 0;
    fighter.anim_frame = 0;
}

void get_fighter_state(fighter_data data)
{
    if (fighter.walking)
        {
            fighter.row_max = ANIM_FULG_WALK_ROW_MAX;
            fighter.idle = false;
            if (fighter.spr_ndx == ANIM_FULG_WALK_ROW_MAX)
            {
                fighter.spr_ndx = walk_start_index;
            }
        }
    if (fighter.idle)
    {
         fighter.walking = false;
         fighter.jumping = false;
         fighter.row_max = ANIM_FULG_STAND_ROW_MAX;
         if (fighter.spr_ndx == ANIM_FULG_STAND_ROW_MAX)
         {
            fighter.spr_ndx = standing_start_index;
         }
    }
    if (fighter.jumping)
    {
        fighter.idle = false;
        fighter.walking = false;
        fighter.row_max = ANIM_FULG_JUMP_ROW_MAX;
        if (fighter.spr_ndx == ANIM_FULG_JUMP_ROW_MAX)
        {
          fighter.spr_ndx = jump_start_index;
        }
    }
}

void updateFrame(fighter_data data) {
// need to set up so that the fighter.anim_frame updates at an interval of fighter.time
// this way, if needed fighter.time can be sped up or slowed down without affect the frame. Rather
// the frame would just update quicker
// anim_frame should not be directly effected by fighter time?
//


  if(fighter.reverse_frame) {
    // redo with max frame or count number of times reversed
    fighter.time --;
  } else {
    fighter.time ++;
  }

  // I want to change this because in order to have the ability to update frame timing.
  fighter.anim_frame = fighter.time/ANIM_FRAME_DELAY; //Calculate fighter frame
}

void fighter_state_check(fighter_data data)
{
    if(fighter.walking || fighter.jumping)  {

        fighter.idle = false;
        if (fighter.walking) {
          fighter.jumping = false;

          // backing up animation
          if (fighter.backing_up && fighter.time <= 0) {

            if (fighter.spr_ndx > 2 && fighter.anim_frame == 0){
              fighter.spr_ndx -= 1;
              fighter.time = ANIM_FRAME_DELAY*ANIM_FULG_WALK_COL_MAX;
            } else if (fighter.spr_ndx == 2) {
              fighter.spr_ndx = 4;
              fighter.time = ANIM_FRAME_DELAY*ANIM_FULG_WALK_COL_MAX;
            }

          } else if (fighter.time >= ANIM_FRAME_DELAY*ANIM_FULG_WALK_COL_MAX) {
              fighter.time = 0;
              fighter.spr_ndx += 1;

            if(fighter.time >= ANIM_FRAME_DELAY*ANIM_FULG_WALK_FRAME_MAX) {
                fighter.time = 0;
                fighter.spr_ndx = walk_start_index;
            }
          }
        }
        else if (fighter.jumping)
        {
          fighter.walking = false;
          if (fighter.time >= ANIM_FRAME_DELAY*ANIM_FULG_JUMP_COL_MAX) {
              fighter.time = 0;

              fighter.spr_ndx += 1;

            if(fighter.time >= ANIM_FRAME_DELAY*ANIM_FULG_JUMP_FRAME_MAX) {
                fighter.time = 0;

                fighter.spr_ndx = jump_start_index;
            }
          }
        }
    } else {
        fighter.idle = true;
        if (fighter.time > max_frame) {
          max_frame = fighter.time;
        }

        if (fighter.time <= 0) {
          if(fighter.spr_ndx == 0 && fighter.anim_frame == 0) {
            fighter.reverse_frame = false;
            fighter.time = 0;
          } else {
            fighter.spr_ndx -= 1;
            fighter.time = ANIM_FRAME_DELAY*ANIM_FULG_STAND_COL_MAX;
          }
        }
        else if (fighter.spr_ndx == 0 && fighter.time <= 0)
        {
            fighter.reverse_frame = false;
            fighter.spr_ndx  = 0;
            fighter.time = 0;
        }
        else if (fighter.time >= ANIM_FRAME_DELAY*ANIM_FULG_STAND_COL_MAX )
        {
            // go to the next row of the standing animation
          if (fighter.spr_ndx == 1 && fighter.time >= 1) {
            fighter.reverse_frame = true;
          }
          else
          {
            fighter.reverse_frame = false;
            fighter.spr_ndx += 1;
            fighter.time = 0;
          }
        }
    }
}
