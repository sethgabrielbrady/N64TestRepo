
#ifndef MENU_H
#define MENU_H

extern rdpq_font_t *fontBillboard;
extern bool show_menu;
extern int selector_start_pos;
extern int menu_center_y;
extern int selection_counter;
extern bool not_at_menu_end;



#define BILLBOARD_YOFFSET   15.0f
#define FONT_BILLBOARD      2
#define TEXT_COLOR    0x6CBB3CFF



void load_font(void);
void stats_draw_billboard();


#endif