//
// Created by stefkoff on 6/4/24.
//

#ifndef NCOURSESTEST_CONTROLPANEL_H
#define NCOURSESTEST_CONTROLPANEL_H

#include "win.h"

typedef struct CONTROL_PANEL {
    Content* content;
    uint8_t is_visible;
} ControlPanel;

void render_control_panel(Content*, int, WIN*);
int init_control_panel(Content*, Connection*);
void control_panel_on_key_event(int);
void control_panel_on_active(int);
int on_control_panel_command(char*);

#endif //NCOURSESTEST_CONTROLPANEL_H
