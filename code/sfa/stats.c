
#include <libdragon.h>
#include <stdint.h>
#include <string.h>
#include <stdio.h>

#include "stats.h"
#include "fighter.h"
#include "controller.h"
#include "shared.h"

rdpq_font_t *fontBillboard;


void load_font(void)
{
  fontBillboard = rdpq_font_load("rom:/squarewave.font64");
  rdpq_text_register_font(FONT_BILLBOARD, fontBillboard);
  rdpq_font_style(fontBillboard, 0, &(rdpq_fontstyle_t){.color = color_from_packed32(TEXT_COLOR) });
}

void stats_draw_billboard(fighter_data data)
{
  int x = 10;
  int y = 10;
  int width = 100;
  int height = 100;
  int alpha = 64;
  float fps = display_get_fps();


  int ticks = get_ticks_ms();
  int secs = ticks/1000;
  int mem = get_memory_size();



  rdpq_set_mode_standard();

  rdpq_mode_combiner(RDPQ_COMBINER_FLAT);
  rdpq_mode_blender(RDPQ_BLENDER_MULTIPLY);
  rdpq_set_prim_color(RGBA32(255, 0, 0, alpha));
  rdpq_fill_rectangle(x, y, width, height);
  rdpq_text_printf(&(rdpq_textparms_t){}, FONT_BILLBOARD, x+2, y+15, "FPS %f", fps);
  rdpq_text_printf(&(rdpq_textparms_t){}, FONT_BILLBOARD, x+2, y+25, "a frame %d", fighter.anim_frame);
  rdpq_text_printf(&(rdpq_textparms_t){}, FONT_BILLBOARD, x+2, y+45, "spr idx %d", fighter.spr_ndx);
  rdpq_text_printf(&(rdpq_textparms_t){}, FONT_BILLBOARD, x+2, y+55, "posX %f", posX);
  rdpq_text_printf(&(rdpq_textparms_t){}, FONT_BILLBOARD, x+2, y+65, "ticks %d", ticks);
  rdpq_text_printf(&(rdpq_textparms_t){}, FONT_BILLBOARD, x+2, y+75, "secs %d", secs);
  rdpq_text_printf(&(rdpq_textparms_t){}, FONT_BILLBOARD, x+2, y+85, "mem %d", mem);
  rdpq_text_printf(&(rdpq_textparms_t){}, FONT_BILLBOARD, x+2, y+95, "ft %d", fighter.time);
  rdpq_text_printf(&(rdpq_textparms_t){}, FONT_BILLBOARD, x+2, y+105, "posY %f", posY);



  if (fighter.reverse_frame) {
    rdpq_text_printf(&(rdpq_textparms_t){}, FONT_BILLBOARD, x+45, y+35, "%s", "reverse");
  } else {
    rdpq_text_printf(&(rdpq_textparms_t){}, FONT_BILLBOARD, x+45, y+35, "%s", "forward");
  }

  if (fighter.idle) {
    rdpq_text_printf(&(rdpq_textparms_t){}, FONT_BILLBOARD, x+2, y+35, "%s", "idle");
  } else if (fighter.walking) {
    rdpq_text_printf(&(rdpq_textparms_t){}, FONT_BILLBOARD, x+2, y+35, "%s", "walking");
  } else if (fighter.jumping) {
    rdpq_text_printf(&(rdpq_textparms_t){}, FONT_BILLBOARD, x+2, y+35, "%s", "jumping");
  }

  rdpq_sync_pipe(); // Hardware crashes otherwise
}
