#include "fighter.h"


#ifndef STATS_H
#define STATS_H

extern rdpq_font_t *fontBillboard;

#define BILLBOARD_YOFFSET   15.0f
#define FONT_BILLBOARD      2
#define TEXT_COLOR    0x6CBB3CFF


void load_font(void);
void stats_draw_billboard(fighter_data data);


#endif