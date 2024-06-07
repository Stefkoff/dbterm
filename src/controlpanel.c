//
// Created by stefkoff on 6/4/24.
//

#include "../include/controlpanel.h"

ControlPanel* control_panel;

int init_control_panel(Content* content, Connection* connection)
{
    control_panel = (ControlPanel*) malloc(sizeof(struct CONTROL_PANEL));

    if(control_panel == NULL) {
        return 1;
    }

    control_panel->content = content;
    control_panel->is_visible = 0;

    return 0;
}

void control_panel_on_active(int is_active)
{
    control_panel->is_visible = is_active;
}

void control_panel_on_key_event(int key)
{
    switch (key) {
        case ':':
            control_panel->is_visible = 1;
            break;
    }
}

void render_control_panel(Content* content, int is_active, WIN* window)
{
    if(control_panel->is_visible == 0) {
        return;
    }
    wclear(content->window);
    box(content->window, 0, 0);
    mvwprintw(content->window, 1, 1, "%c", ':');
    wrefresh(content->window);
    curs_set(1);
    echo();
    wgetstr(content->window, window->command);
    curs_set(0);
    noecho();
    refresh();
    control_panel->is_visible = 0;
    window->skip_key_event = 1;
}

int on_control_panel_command(char* command)
{
    return;
}