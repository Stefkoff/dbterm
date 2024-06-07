//
// Created by stefkoff on 6/3/24.
//

#include "../include/leftpanel.h"

LeftPanel* left_content;

void left_panel_on_active(int is_active)
{
    left_content->is_active = is_active;
}

void left_panel_on_key_event(int key)
{
    int incr;
    switch(key) {
        case KEY_DOWN:
        case KEY_UP:
            incr = key == KEY_UP ? -1 : 1;
            if (left_content->is_active == 1) {
                left_content->active_item += incr;
                if (left_content->active_item <= 0) {
                    left_content->active_item = left_content->connection->database->tables_size;
                } else if (left_content->active_item > left_content->connection->database->tables_size) {
                    left_content->active_item = 1;
                }
            }
            left_content->connection->database->active_table = left_content->active_item - 1;
            break;
        case KEY_HOME:
            if(left_content->is_active) {
                left_content->active_item = 1;
                left_content->connection->database->active_table = left_content->active_item - 1;
            }

            break;
        case KEY_END:
            if(left_content->is_active){
                left_content->active_item = left_content->connection->database->tables_size;
                left_content->connection->database->active_table = left_content->active_item - 1;
            }
            break;
        case 338:
            if(left_content->is_active) {
                left_content->active_item+=10;

                if (left_content->active_item > left_content->connection->database->tables_size) {
                    left_content->active_item = left_content->connection->database->tables_size;
                }
                left_content->connection->database->active_table = left_content->active_item - 1;
            }
            break;
        case 339:
            if(left_content->is_active){
                left_content->active_item-=10;
                if(left_content->active_item <= 0) {
                    left_content->active_item = 1;
                }
                left_content->connection->database->active_table = left_content->active_item - 1;
            }
            break;
        default:
            break;
    }
}

int init_left_panel(Content* content, Connection* connection)
{
    left_content = (LeftPanel*) malloc(sizeof(struct LEFT_PANEL));

    if(left_content == NULL) {
        return 1;
    }

    left_content->content = content;
    left_content->connection = connection;
    left_content->is_active = 1;
    left_content->active_item = 1;

    return 0;
}

void draw_window(Content* content)
{
    char *title = "Tables";
    uint16_t starting = (content->width / 2) - (strlen(title) / 2) - 1;
    mvwprintw(content->window, 1, starting, "%s", title);
    // first row is for the title above, and we are on the second row at second column (first is the border)
    wmove(content->window, 2, 1);
    whline(content->window, '-', content->width - 2);
}

void print_tables()
{
    if(
        left_content == NULL ||
        left_content->connection == NULL ||
        left_content->content == NULL
    ) {
        return;
    }

    int y,x;
    int from = 0;

    if(left_content->active_item > (left_content->content->height - 6)) {
        from = left_content->active_item - (left_content->content->height - 6);
    }

    getyx(left_content->content->window, y, x);
    int index = 0;
    for(int i = from; i < left_content->connection->database->tables_size; i++) {
        if (i > left_content->connection->database->tables_size - 1) {
            break;
        }

        int isHighlited = left_content->active_item == i + 1 ? 1 : 0;
        wattroff(left_content->content->window, A_REVERSE);
        if(isHighlited) {
            wattron(left_content->content->window, A_REVERSE);
        }

        for(int c = 0; c < left_content->content->width - 2; c++) {
            if(!left_content->connection->database->tables[i]->name[c]) {
                break;
            }
            if(left_content->connection->database->tables[i]->name[c] == '\n' || (left_content->connection->database->tables[i]->name[c] < 0 || left_content->connection->database->tables[i]->name[c] > 127)) {
                mvwprintw(left_content->content->window, y + 1 + (index + 1), x + c, "%c", '?');
            } else {
                mvwprintw(left_content->content->window, y + 1 + (index + 1), x + c, "%c", left_content->connection->database->tables[i]->name[c]);
            }
        }

        if(isHighlited){
            wattroff(left_content->content->window,A_REVERSE);
        }

        if(index++ >= (left_content->content->height - 6)) {
            break;
        }
    }
}

void render_left_panel(Content* content, int is_active, WIN* window)
{
    WINDOW* win = content->window;
    wclear(win);
    box(win, 0 , 0);
    draw_window(content);
    print_tables();

    wrefresh(win);
}

int on_left_panel_command(char* command)
{
    return 0;
}

int left_panel_activate_key(void) {
    return KEY_F(1);
}