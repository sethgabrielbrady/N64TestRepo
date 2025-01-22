
#ifndef MENU_H
#define MENU_H

extern rdpq_font_t *fontBillboard;
extern bool show_menu;

#define BILLBOARD_YOFFSET   15.0f
#define FONT_BILLBOARD      2
#define TEXT_COLOR    0x6CBB3CFF



void load_font(void);
void stats_draw_billboard();


#endif