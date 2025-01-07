#ifndef FIGHTER_H
#define FIGHTER_H



extern float frame_delay;
extern int walk_start_index;
extern int standing_start_index;
extern int jump_start_index;
extern int max_frame;

extern int standing_width;
extern int standing_height;
extern int walking_width;
extern int walking_height;

extern int current_anim_frame_width;


//Animation frame size defines
#define ANIM_FRAME_W current_anim_frame_width
#define ANIM_FRAME_H 98
#define ANIM_IDLE_W 66
#define ANIM_JUMP_W 98


//#define ANIM_FRAME_DELAY 6
#define ANIM_FRAME_DELAY frame_delay

//standing animation frame size defines
#define ANIM_SHEET_STAND_MAX_FRAMES 6
#define ANIM_SHEET_STAND_ROW_MAX 1
#define ANIM_SHEET_STAND_COL_MAX 6
//walking animation frame size defines
#define ANIM_SHEET_WALK_MAX_FRAMES 6
#define ANIM_SHEET_WALK_ROW_MAX 1
#define ANIM_SHEET_WALK_COL_MAX 6
//jumping animation frame size defines
#define ANIM_SHEET_JUMP_MAX_FRAMES 9
#define ANIM_SHEET_JUMP_ROW_MAX 4
#define ANIM_SHEET_JUMP_COL_MAX 5


typedef enum {
    STATE_IDLE,
    STATE_WALKING,
    STATE_JUMPING,
    STATE_BACKING_UP,
    // Add other states as needed
} fighter_state;

typedef struct {
    fighter_state state;
    float x;
    float y;
    bool walking;
    bool idle;
    bool jumping;
    bool flip;
    int time;
    bool reverse_frame;
    bool backing_up;
    int spr_ndx;
    int row_max;
    int anim_frame;

} fighter_data;

extern fighter_data fighter;


void fighter_init(fighter_data data);
void update_frame_direction(fighter_data data);
void fighter_state_machine(fighter_data data);

#endif // FIGHTER_H