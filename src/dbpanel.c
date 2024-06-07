//
// Created by stefkoff on 6/7/24.
//
#include "../include/dbpanel.h"

DbPanel* db_panel;

int init_db_panel(Content* content, Connection* conn)
{
    db_panel = (DbPanel*) malloc(sizeof(struct DB_PANEL));

    if(db_panel == NULL) {
        return 1;
    }

    db_panel->content = content;
    db_panel->connection = conn;
    db_panel->is_visible = 0;
    db_panel->active_item = 1;

    load_databases();

    return 0;
}

void db_panel_on_activate(int is_active)
{
    db_panel->is_visible = is_active;
}

void db_panel_on_key_event(int key)
{
    switch (key) {
        // ESC
        case 27:
            if(db_panel->is_visible ) {
                db_panel->is_visible = 0;
            }
            break;
        case KEY_F(5):
            if(db_panel->is_visible == 0) {
                break;
            }

            db_panel->is_visible = 0;
            strcpy(db_panel->connection->db_name, db_panel->items[db_panel->active_item - 1]);
            connect_mysql(db_panel->connection);
            load_tables(db_panel->connection);

            if(db_panel->win != NULL) {
                db_panel->win->re_init = 1;
            }
            break;
        case KEY_DOWN:
        case KEY_UP:
            if (db_panel->is_visible == 1) {
                int incr = (key == KEY_UP ? -1 : 1);
                db_panel->active_item += incr;
                if (db_panel->active_item <= 0) {
                    db_panel->active_item = db_panel->items_len;
                } else if (db_panel->active_item > db_panel->items_len) {
                    db_panel->active_item = 1;
                }
            }
            break;
    }
}

int on_db_panel_command(char* command)
{
    if(db_panel->items_len <= 0) {
        return 0;
    }
    mvwprintw(stdscr, 1, 1, "%s", command);
    if(strstr(command, "db") != NULL) {
        char* pFound;
        pFound = strstr(command, "db");

        if(pFound != NULL) {
            pFound+=3;

            for(int i = 0; i < db_panel->items_len; i++) {
                if(strcmp(pFound, db_panel->items[i]) == 0) {
                    db_panel->is_visible = 0;
                    strcpy(db_panel->connection->db_name, db_panel->items[i]);
                    connect_mysql(db_panel->connection);
                    load_tables(db_panel->connection);

                    if(db_panel->win != NULL) {
                        db_panel->win->re_init = 1;
                    }
                    return 1;
                }
            }

            return 0;
        }
    }
    return 0;
}

void render_db_panel(Content* content, int is_active, WIN* window)
{
    db_panel->win = window;
    if(db_panel->is_visible == 0)
    {
        return;
    }

    wclear(content->window);
    box(content->window, 0, 0);
    draw_db_panel();
    wrefresh(content->window);
}

void draw_db_panel() {
    if(db_panel->items_len <= 0) {
        return;
    }

    int from;
    int index;

    from = 0;

    if(db_panel->active_item > (DB_PANEL_HEIGHT - 2)) {
        from = db_panel->active_item - (DB_PANEL_HEIGHT - 2);
    }
    index = 0;
    for(int i = from; i < db_panel->items_len; i++) {
        if(++index >= DB_PANEL_HEIGHT - 1) {
            break;
        }

        int isHighlited = (db_panel->active_item - from == index) ? 1 : 0;
        wattroff(db_panel->content->window, A_REVERSE);
        if(isHighlited) {
            wattron(db_panel->content->window, A_REVERSE);
        }

        // TODO: handle line limiting
        mvwprintw(db_panel->content->window, index, 1, "%s", db_panel->items[i]);
        if(isHighlited){
            wattroff(db_panel->content->window,A_REVERSE);
        }
    }

}

void load_databases()
{

    db_panel->connection->result = mysql_list_dbs(db_panel->connection->connection, "%");

    if(db_panel->connection->result == NULL) {
        return;
    }

    db_panel->items = (char**) malloc(sizeof(char) * MAX_DATABASE_SIZE * db_panel->connection->result->row_count);

    if(db_panel->items == NULL) {
        mysql_free_result(db_panel->connection->result);
        return;
    }

    MYSQL_ROW row;
    int index;


    db_panel->items_len = db_panel->connection->result->row_count;
    index = 0;
    while((row = mysql_fetch_row(db_panel->connection->result)) != NULL) {
        db_panel->items[index] = (char*) malloc(sizeof(char) * strlen(row[0]));
        if(db_panel->items[index] == NULL){
            continue;
        }
        strcpy(db_panel->items[index++], row[0]);
    }

    mysql_free_result(db_panel->connection->result);
}