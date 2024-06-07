//
// Created by stefkoff on 6/3/24.
//

#ifndef NCOURSESTEST_LEFTPANEL_H
#define NCOURSESTEST_LEFTPANEL_H

#include "win.h"
#include "database.h"

typedef struct LEFT_PANEL {
    Content* content;
    Connection* connection;
    int active_item;
    int is_active;
} LeftPanel;

void draw_window(Content*);

void render_left_panel(Content*, int, WIN*);
int init_left_panel(Content*, Connection*);
void print_tables();
void left_panel_on_key_event(int);
void left_panel_on_active(int);
int on_left_panel_command(char*);
int left_panel_activate_key(void);

#endif //NCOURSESTEST_LEFTPANEL_H
