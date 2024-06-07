//
// Created by stefkoff on 6/7/24.
//

#ifndef DBTERM_DBPANEL_H
#define DBTERM_DBPANEL_H

#include "win.h"

#define DB_PANEL_HEIGHT (LINES - 10 - (LINES / 4) - 1)
#define DB_PANEL_WIDTH ((COLS / 2) - 1)

typedef struct DB_PANEL {
    Content* content;
    Connection* connection;
    uint8_t is_visible;
    uint16_t active_item;
    char** items;
    uint16_t items_len;
    WIN* win;
} DbPanel;

void render_db_panel(Content*, int, WIN*);
int init_db_panel(Content*, Connection*);
void db_panel_on_key_event(int);
void db_panel_on_activate(int);
int on_db_panel_command(char*);
void draw_db_panel();
void load_databases();

#endif //DBTERM_DBPANEL_H
