#include "fighter.h"
#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>
#include <math.h>
#include <stdint.h>

int frame_delay = 6;
int walk_start_index = 2;
int standing_start_index = 0;
int jump_start_index = 2;

void get_fighter_state(void)
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
