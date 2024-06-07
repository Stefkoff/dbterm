//
// Created by stefkoff on 6/4/24.
//

#include "win.h"

#ifndef NCOURSESTEST_RIGHTPANEL_H
#define NCOURSESTEST_RIGHTPANEL_H

#define MAX_COLUMN_SIZE 15

typedef struct RIGHT_PANEL {
    Content* content;
    Connection* connection;
    WINDOW* view_window;
    char*** items;
    char* where_condition;
    uint8_t is_active;
    int active_item;
    int items_len;
    int columns_offset;
    int last_index;
    int per_page;
    uint8_t show_view_modal;
    int view_modal_row_offset;
} RightPanel;

void draw_right_window(Content*);
void render_right_panel(Content*, int, WIN*);
int init_right_panel(Content*, Connection*);
void right_panel_on_active(int);
void right_panel_on_key_event(int);
int load_data();
void print_rows();
void render_view_panel();
void draw_view_panel();
void load_view_modal_data();
void unload_view_modal_data();
int find_token(char*,char,int*);

int on_right_panel_command(char*);
int right_panel_activate_key(void);


#endif //NCOURSESTEST_RIGHTPANEL_H
