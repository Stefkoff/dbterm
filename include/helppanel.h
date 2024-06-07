//
// Created by stefkoff on 6/7/24.
//

#ifndef DBTERM_HELPPANEL_H
#define DBTERM_HELPPANEL_H

#include "win.h"

#define HELP_PANEL_HEIGHT (LINES - 10)
#define HELP_PANEL_WIDTH ((COLS) - 10)

typedef struct HELP_PANEL {
    Content* content;
    uint8_t is_visible;
    int64_t line_offset;
    char** lines;
    uint64_t lines_len;
    uint8_t file_loaded;
} HelpPanel;

void render_help_panel(Content*, int, WIN*);
int init_help_panel(Content*, Connection*);
void help_panel_on_key_event(int);
void help_panel_on_activate(int);
int on_help_panel_command(char*);
void draw_help_panel();

#endif //DBTERM_HELPPANEL_H
