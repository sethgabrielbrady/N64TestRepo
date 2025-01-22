
#include <libdragon.h>
#include <stdlib.h>
#include <malloc.h>
#include <math.h>
#include <stdint.h>
#include <stdio.h>
#include "controller.h"
#include "models.h"

rdpq_font_t *fontBillboard;
bool show_menu = false;

int x = 10;
int y = 10;
int width = 310;
int height = 230;
int alpha = 64;
int selector_start_pos = 110;
int menu_center_y = 110;
int selection_counter = 0;
bool not_at_menu_end = true;


void load_font(void)
{
  fontBillboard = rdpq_font_load("rom:/squarewave.font64");
  rdpq_text_register_font(FONT_BILLBOARD, fontBillboard);
  rdpq_font_style(fontBillboard, 0, &(rdpq_fontstyle_t){.color = color_from_packed32(TEXT_COLOR) });
}

void stats_draw_billboard()
{

  int ticks = get_ticks_ms();
  int secs = ticks/1000;
  int mem = get_memory_size();
  float fps = display_get_fps();




  rdpq_set_mode_standard();

  rdpq_mode_combiner(RDPQ_COMBINER_FLAT);
  rdpq_mode_blender(RDPQ_BLENDER_MULTIPLY);
  rdpq_set_prim_color(RGBA32(255, 0, 255, alpha));
  rdpq_fill_rectangle(x, y, width, height);



  //game stats
  rdpq_text_printf(&(rdpq_textparms_t){}, FONT_BILLBOARD, x+2, y+15, "FPS %f", fps);
  rdpq_text_printf(&(rdpq_textparms_t){}, FONT_BILLBOARD, x+2, y+25, "ticks %d", ticks);
  rdpq_text_printf(&(rdpq_textparms_t){}, FONT_BILLBOARD, x+2, y+35, "secs %d", secs);
  rdpq_text_printf(&(rdpq_textparms_t){}, FONT_BILLBOARD, x+2, y+45, "mem %d", mem);



  //model selection
  int new_menu_center_y = menu_center_y;
  for (int i = 0; i < MODEL_MAPS_LENGTH; i++ ) {
    rdpq_text_printf(&(rdpq_textparms_t){}, FONT_BILLBOARD, 130, new_menu_center_y+(i*10), "Model %i", i+1);
  }

  rdpq_text_printf(&(rdpq_textparms_t){}, FONT_BILLBOARD, 120, selector_start_pos, ">");
  selection_counter = selector_start_pos/10 - 10;

  if (selection_counter == MODEL_MAPS_LENGTH ) {
    not_at_menu_end = false;
  } else {
    not_at_menu_end = true;
  }








  rdpq_sync_pipe(); // Hardware crashes otherwise
}
