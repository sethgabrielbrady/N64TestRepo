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
int jump_start_index = 1;
int max_frame = 0;

fighter_data fighter;


void fighter_init(fighter_data data)
{
    fighter.reverse_frame = false;
    fighter.spr_ndx = 0;
    fighter.anim_frame = 0;

      // Initialize fighter_data if necessary
    fighter.state = STATE_IDLE;
    fighter.time = 0;
    //fighter.anim_frame = 0;
    //fighter.spr_ndx = 0;
    //fighter.reverse_frame = false;
    fighter.walking = false;
    fighter.jumping = false;
    fighter.backing_up = false;
    fighter.idle = true;
}


void update_frame_direction(fighter_data data) {
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


void fighter_state_machine(fighter_data data)
{
  switch (fighter.state) {
    case STATE_IDLE:
      // fighter.walking = false;
      // fighter.jumping = false;
      // fighter.row_max = ANIM_FULG_STAND_ROW_MAX;
      // if (fighter.spr_ndx == ANIM_FULG_STAND_ROW_MAX)
      // {
      //   fighter.spr_ndx = standing_start_index;
      // }


      if (fighter.time > max_frame)
      {
        max_frame = fighter.time;
      }

      if (fighter.time <= 0) {
        if(fighter.spr_ndx == 0 && fighter.anim_frame == 0)
        {
          fighter.reverse_frame = false;
          fighter.time = 0;
        }
        else
        {
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
        if (fighter.spr_ndx == 1 && fighter.time >= 1)
        {
          fighter.reverse_frame = true;
        }
        else
        {
          fighter.reverse_frame = false;
          fighter.spr_ndx += 1;
          fighter.time = 0;
        }
      }
    case STATE_WALKING:
        fighter.row_max = ANIM_FULG_WALK_ROW_MAX;
        fighter.idle = false;
        if (fighter.spr_ndx == ANIM_FULG_WALK_ROW_MAX)
        {
            fighter.spr_ndx = walk_start_index;
        }


        if (data.backing_up && data.time <= 0) {
            data.state = STATE_BACKING_UP;
        }
        else if (data.time >= ANIM_FRAME_DELAY * ANIM_FULG_WALK_COL_MAX)
        {
          data.time = 0;
        }
        break;

    case STATE_JUMPING:
        // Handle jumping logic
        fighter.idle = false;
        fighter.walking = false;
        fighter.row_max = ANIM_FULG_JUMP_ROW_MAX;
        // if (fighter.spr_ndx == ANIM_FULG_JUMP_ROW_MAX)
        // {
        //   fighter.spr_ndx = jump_start_index;
        // }

          fighter.walking = false;
          if (fighter.time >= ANIM_FRAME_DELAY*ANIM_FULG_JUMP_COL_MAX)
          {
            fighter.time = 0;
            fighter.spr_ndx += 1;

            if (fighter.time >= ANIM_FRAME_DELAY*ANIM_FULG_JUMP_FRAME_MAX)
            {
              fighter.time = 0;
              fighter.spr_ndx = jump_start_index;
            }
          }
        break;

    case STATE_BACKING_UP:
        if (data.spr_ndx > 2 && data.anim_frame == 0)
        {
          data.spr_ndx -= 1;
          data.time = ANIM_FRAME_DELAY * ANIM_FULG_WALK_COL_MAX;
        }
        else if (data.spr_ndx == 2)
        {
          data.spr_ndx = 4;
          data.time = ANIM_FRAME_DELAY * ANIM_FULG_WALK_COL_MAX;
        }
        if (!data.backing_up)
        {
          data.state = STATE_WALKING;
        }
        break;
        // Add other states as needed
    }
}

