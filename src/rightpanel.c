//
// Created by stefkoff on 6/4/24.
//

#include <unistd.h>
#include <math.h>
#include "../include/rightpanel.h"

RightPanel* right_panel;

void init_where_condition()
{
    right_panel->where_condition = NULL;
    right_panel->where_condition = (char*) malloc(sizeof(char) * 1024);
    memset(right_panel->where_condition, '\0', 1024);
}

void right_panel_on_active(int is_active)
{
    right_panel->is_active = is_active;

    if(right_panel->is_active == 0) {
        right_panel->active_item = 1;
    }
}

void right_panel_on_key_event(int key)
{
    switch(key) {
        // ESC
        case 27:
            if(right_panel->is_active ) {
                unload_view_modal_data();
            }
            break;
        case KEY_F(3):
            if(right_panel->is_active) {
                if(right_panel->show_view_modal == 1) {
                    unload_view_modal_data();
                    break;
                }
                load_view_modal_data();
            }

            break;
        case KEY_DOWN:
        case KEY_UP:
            if (right_panel->is_active == 1) {
                int incr = (key == KEY_UP ? -1 : 1);
                if(right_panel->show_view_modal) {
                    right_panel->view_modal_row_offset += incr;

                    if(right_panel->view_modal_row_offset <= 0) {
                        right_panel->view_modal_row_offset = 0;
                    } else if(right_panel->view_modal_row_offset >= LINES - 2) {
                        right_panel->view_modal_row_offset = LINES - 2;
                    }
                    break;
                }
                right_panel->active_item += incr;
                if (right_panel->active_item <= 0) {
                    right_panel->active_item = right_panel->items_len;
                } else if (right_panel->active_item > right_panel->items_len) {
                    right_panel->active_item = 1;
                }
            } else {
                right_panel->last_index = 0;
                load_data();
            }
            break;
        case KEY_LEFT:
        case KEY_RIGHT:
            int incr_col = key == KEY_RIGHT ? 1 : -1;
            if (right_panel->is_active == 1) {
                right_panel->columns_offset += incr_col;
                if (right_panel->columns_offset <= 0) {
                    right_panel->columns_offset = 0;
                } else if (right_panel->columns_offset > right_panel->connection->database->tables[right_panel->connection->database->active_table]->columns_size - 1) {
                    right_panel->columns_offset = right_panel->connection->database->tables[right_panel->connection->database->active_table]->columns_size - 1;
                }
            }
            break;
        case KEY_HOME:
            if(right_panel->is_active) {
                right_panel->active_item = 1;
            } else {
                right_panel->last_index = 0;
                load_data();
            }
            break;
        case KEY_END:
            if(right_panel->is_active){
                right_panel->active_item = right_panel->items_len;
            } else {
                right_panel->last_index = 0;
                load_data();
            }
            break;
        case 338:
            if(right_panel->is_active) {
                right_panel->active_item+=10;

                if (right_panel->active_item > right_panel->items_len) {
                    right_panel->active_item = right_panel->items_len;
                }
            } else {
                right_panel->last_index = 0;
                load_data();
            }
            break;
        case 339:
            if(right_panel->is_active){
                right_panel->active_item-=10;
                if(right_panel->active_item <= 0) {
                    right_panel->active_item = 1;
                }
            } else {
                right_panel->last_index = 0;
                load_data();
            }
            break;
    }
}

int load_data()
{
    if(right_panel->items != NULL) {
        free(right_panel->items);
        right_panel->items = NULL;
    }
    if((query_all(right_panel->connection, right_panel->connection->database->tables[right_panel->connection->database->active_table], right_panel->per_page, right_panel->last_index, right_panel->where_condition)) != 0) {
        return 1;
    }

    if(right_panel->connection->result->row_count <= 0) {
        right_panel->items_len = 0;
        mysql_free_result(right_panel->connection->result);
        return 0;
    }

    right_panel->items = (char***) malloc(sizeof(char) * (right_panel->connection->result->row_count * right_panel->per_page * MAX_COLUMN_SIZE + 1));
    if(right_panel->items == NULL) {
        mysql_free_result(right_panel->connection->result);
        return 1;
    }
    uint16_t num_fields = mysql_num_fields(right_panel->connection->result);

    MYSQL_ROW row;
    int index = 0;
    int max_column_size = MAX_COLUMN_SIZE;
    if(right_panel->connection->database->tables[right_panel->connection->database->active_table]->columns_size < (right_panel->content->width / MAX_COLUMN_SIZE)) {
        max_column_size = right_panel->content->width / right_panel->connection->database->tables[right_panel->connection->database->active_table]->columns_size;
    }

    while((row = mysql_fetch_row(right_panel->connection->result))) {
        right_panel->items[index] = (char**) malloc(sizeof(char) * (num_fields * MAX_COLUMN_SIZE));
        if(right_panel->items[index] == NULL) {
            continue;
        }
        right_panel->items[index][0] = (char*) malloc(sizeof(char) * (max_column_size + 1));
        if(right_panel->items[index][0] == NULL) {
            continue;
        }
        right_panel->last_index++;
        snprintf(right_panel->items[index][0], max_column_size, "%d", right_panel->last_index);
        for(int f = 0; f < num_fields; f++) {
            right_panel->items[index][f + 1] = (char*) malloc(sizeof(char) * (max_column_size + 1));
            if(right_panel->items[index][f + 1] == NULL) {
                continue;
            }
            if(row[f] == NULL) {
                snprintf(right_panel->items[index][f + 1], max_column_size, "(NULL)");
                continue;
            }
            snprintf(right_panel->items[index][f + 1], max_column_size, "%.*s", COLS, row[f]);
        }
        index++;
    }

    right_panel->items_len = index;
    mysql_free_result(right_panel->connection->result);

//    right_panel->connection->database->tables[right_panel->connection->database->active_table]->total_rows = index;

    return 0;
}

int init_right_panel(Content* content, Connection* connection)
{
    right_panel = (RightPanel*) malloc(sizeof( struct RIGHT_PANEL));

    if(right_panel == NULL) {
        return 1;
    }

    right_panel->active_item = 1;
    right_panel->connection = connection;
    right_panel->content = content;
    right_panel->is_active = 0;
    right_panel->columns_offset = 0;
    right_panel->last_index = 0;
    right_panel->per_page = MAX_ROWS;
    init_where_condition();
    right_panel->show_view_modal = 0;
    right_panel->view_modal_row_offset = 0;
    right_panel->items = NULL;

    load_data();

    return 0;
}

void draw_right_window(Content* content)
{
    char* title = (char*) malloc(sizeof(char) * 1024);
    uint16_t total_rows = 0;
    if(right_panel->connection->database->tables[right_panel->connection->database->active_table] != NULL) {
        total_rows = right_panel->connection->database->tables[right_panel->connection->database->active_table]->total_rows;
    }

    int current_page, total_pages;
    double cur_d = (double) right_panel->last_index / right_panel->per_page;
    current_page = ceil(cur_d);
    total_pages = total_rows / right_panel->per_page;
    sprintf(title, "Table %s showing %d of %d page. Total: %d. Per page: %d", right_panel->connection->database->tables[right_panel->connection->database->active_table]->name, current_page == 0 ? 1 : current_page, total_pages == 0 ? 1 : total_pages + 1, total_rows, right_panel->per_page);
    int starting = (content->width / 2) - (strlen(title) / 2) - 1;
    mvwprintw(content->window, 1, starting, "%s", title);
    // first row is for the title above, and we are on the second row at second column (first is the border)
    wmove(content->window, 2, 1);
    whline(content->window, '-', content->width - 2);
}

void print_header()
{
    if(
            right_panel == NULL ||
            right_panel->connection == NULL ||
            right_panel->content == NULL ||
            right_panel->connection->database->tables[right_panel->connection->database->active_table] == NULL
            ) {
        return;
    }

    int y,x;
    int max_column_size = MAX_COLUMN_SIZE;
    if(right_panel->connection->database->tables[right_panel->connection->database->active_table]->columns_size < (right_panel->content->width / MAX_COLUMN_SIZE)) {
        max_column_size = right_panel->content->width / right_panel->connection->database->tables[right_panel->connection->database->active_table]->columns_size;
    }

    int max_columns = right_panel->content->width / max_column_size;

    getyx(right_panel->content->window, y, x);
    int index = 0;
    for(int i = right_panel->columns_offset; i < right_panel->connection->database->tables[right_panel->connection->database->active_table]->columns_size; i++) {
        if(index > max_columns - 1) {
            break;
        }
        wattron(right_panel->content->window, A_REVERSE);
        char* to_print = (char*) malloc(sizeof(char) * (max_column_size + 1));
        if(to_print == NULL) {
            continue;
        }
        snprintf(to_print, max_column_size, "%s", right_panel->connection->database->tables[right_panel->connection->database->active_table]->columns[i]->name);
        mvwprintw(right_panel->content->window, y + 1, 1 + (index++ * max_column_size), "%s", to_print);
        wattroff(right_panel->content->window,A_REVERSE);
        free(to_print);
    }
    wmove(right_panel->content->window, 4, 1);
    whline(right_panel->content->window, '-', right_panel->content->width - 2);
}

void print_rows()
{
    if(
            right_panel == NULL ||
            right_panel->connection == NULL ||
            right_panel->content == NULL ||
            right_panel->connection->database->tables[right_panel->connection->database->active_table] == NULL
    ) {
        return;
    }

    int y,x;
    int max_column_size = MAX_COLUMN_SIZE;
    int from = 0;
    if(right_panel->connection->database->tables[right_panel->connection->database->active_table]->columns_size < (right_panel->content->width / MAX_COLUMN_SIZE)) {
        max_column_size = right_panel->content->width / right_panel->connection->database->tables[right_panel->connection->database->active_table]->columns_size;
    }

    int max_columns = right_panel->content->width / max_column_size;

    if(right_panel->active_item > (LINES - 6)) {
        from = right_panel->active_item - (LINES - 6);
    }

    getyx(right_panel->content->window, y, x);
    int index = 0;
    for(int item = from; item < right_panel->items_len; item++) {
        char** row = right_panel->items[item];
        int isHighlited = right_panel->active_item == item + 1 ? 1 : 0;
        wattroff(right_panel->content->window, A_REVERSE);
        if(isHighlited) {
            wattron(right_panel->content->window, A_REVERSE);
        }

        int colIndex = 0;
        for(int i = right_panel->columns_offset; i < right_panel->connection->database->tables[right_panel->connection->database->active_table]->columns_size; i++) {
            if(colIndex > max_columns - 1) {
                break;
            }

            uint16_t row_len;
            row_len = strlen(row[i]);
            for(int c = 0; c < max_column_size || c < row_len; c++) {
                if(!row[i][c]) {
                    break;
                }
                if(row[i][c] == '\n' || (row[i][c] < 0 || row[i][c] > 127)) {
                    mvwprintw(right_panel->content->window, y + index + 1, 1 + c + (colIndex * max_column_size), "%c", '?');
                } else {
                    mvwprintw(right_panel->content->window, y + index + 1, 1 + c + (colIndex * max_column_size), "%c", row[i][c]);
                }
            }
            colIndex++;
        }
        if(isHighlited){
            wattroff(right_panel->content->window,A_REVERSE);
        }

        if(++index >= (LINES - 6)) {
            break;
        }

    }

}

void render_right_panel(Content* content, int is_active, WIN* win)
{
    wclear(content->window);
    box(content->window, 0, 0);
    draw_right_window(content);
    print_header();
    print_rows();
    wrefresh(content->window);
    save_state(win, "row", right_panel->active_item);

    if(right_panel->show_view_modal) {
        right_panel->view_window = newwin(LINES, COLS - 1, 0, 0);
        if(right_panel->view_window == NULL) {
            return;
        }

        render_view_panel();
    }
}

void render_view_panel() {
    wclear(right_panel->view_window);
    box(right_panel->view_window, 0, 0);
    draw_view_panel();
    wrefresh(right_panel->view_window);
}

int find_token(char* str, char token, int* numFound)
{
    int index = 0;

    while(str != NULL && *str != '\0') {
        if(*str == token) {
            *numFound = index + 1;
            return str[index];
        }
        str++;
        index++;
    }

    return -1;
}

void draw_view_panel()
{
    MYSQL_ROW row;

    uint16_t num_fields = mysql_num_fields(right_panel->connection->result);
    uint16_t lines_printed = 0;
    uint16_t print_index = 0;
    MYSQL_ROW_OFFSET curr_offset = mysql_row_tell(right_panel->connection->result);
    int last_x_at = COLS - 2;
    while((row = mysql_fetch_row(right_panel->connection->result))) {
        int index = 1;
        for(int f = 0; f < num_fields; f++) {
            char* last_post;
            int found_index;
            int num_found;
            last_post = row[f];
            found_index = find_token(last_post, '\n', &num_found);

            if(found_index == -1) {
                if(right_panel->view_modal_row_offset > 0 && print_index < right_panel->view_modal_row_offset) {
                    print_index++;
                    lines_printed++;
                    continue;
                }

                if(last_post == NULL) {
                    continue;
                }

                uint16_t text_size;
                text_size = strlen(last_post);

                if(text_size > last_x_at) {
                    uint16_t printed;
                    int int_print;

                    int_print = 0;
                    printed = 0;
                    while(printed < text_size) {
                        printed+=last_x_at;
                        int num_to_print;

                        // TODO: what is this below?
                        num_to_print = (int_print * last_x_at) + last_x_at;
                        char* to_print = (char*) malloc(sizeof(char) * num_to_print);
                        snprintf(to_print, num_to_print, "%s", last_post);
                        mvwprintw(right_panel->view_window, index++, 1, "%s", to_print);
                        last_post+=(num_to_print - 1);
                    }

                    continue;
                }
                mvwprintw(right_panel->view_window, index++, 1, "%s", last_post);

                print_index++;
                if(lines_printed++ > (LINES - 4) + right_panel->view_modal_row_offset) {
                    break;
                }
                continue;
            }

            while(found_index != -1) {
                int move_next = -1;
                if(right_panel->view_modal_row_offset > 0 && print_index < right_panel->view_modal_row_offset) {
                    print_index++;
                    lines_printed++;
                    continue;
                }
                if(num_found > last_x_at) {
                    move_next = num_found - last_x_at;
                    num_found = last_x_at;

                }
                char* to_print = (char*) malloc(sizeof(char) * num_found);
                snprintf(to_print, num_found, "%s", last_post);
                mvwprintw(right_panel->view_window, index++, 1, "%s", to_print);
                if(move_next > -1) {
                    last_post+=(num_found-1);
                } else {
                    last_post+=num_found;
                }
                found_index = find_token(last_post, '\n', &num_found);
                print_index++;

                if(lines_printed++ > (LINES - 4) + right_panel->view_modal_row_offset) {
                    break;
                }
            }

            if(lines_printed > (LINES - 4) + right_panel->view_modal_row_offset) {
                break;
            }
        }
    }

    mysql_row_seek(right_panel->connection->result, curr_offset);
}

int on_right_panel_command(char* command) {
    if(right_panel->is_active == 0) {
        return 0;
    }
    if(strcmp(command, "next") == 0 || strcmp(command, "ext") == 0) {
        right_panel->active_item = 1;
        load_data();
        return 1;
    } else if(strcmp(command, "prev") == 0) {
        right_panel->active_item = 1;
        right_panel->last_index-=(2 * right_panel->per_page); // return back 100 and one more

        if(right_panel->last_index <= 0) {
            right_panel->last_index = 0;
        }
        load_data();
        return 1;
    } else if(strcmp(command, "first") == 0) {
        right_panel->active_item = 1;
        right_panel->last_index = 0;
        load_data();
        return 1;
    } else if(strcmp(command, "last") == 0) {
        right_panel->active_item = 1;
        uint16_t total_rows = right_panel->connection->database->tables[right_panel->connection->database->active_table]->total_rows;
        right_panel->last_index = (total_rows / right_panel->per_page) * right_panel->per_page;
        load_data();
        return 1;
    } else if(strstr(command, "perpage") != NULL) {
        char* pFound;
        pFound = strstr(command, "perpage");

        if(pFound != NULL) {
            pFound+=8;
            char* pEnd;
            uint16_t per_page = strtol(pFound, &pEnd, 10);

            if(per_page > 0) {
                right_panel->per_page = per_page;
                right_panel->last_index = 0;
                right_panel->active_item = 1;
                load_data();
                return 1;
            }
        }
    } else if(strstr(command, "where") != NULL) {
        mvwprintw(stdscr, 0, 0, "Where: %s", right_panel->where_condition);
        char* pFound;
        pFound = strstr(command, "where");

        if(pFound != NULL) {
            pFound+=6;
            strcpy(right_panel->where_condition, pFound);
            right_panel->last_index = 0;
            right_panel->active_item = 1;
            load_data();

            return 1;
        }
    } else if(strcmp(command, "reset") == 0) {
        right_panel->active_item = 1;
        right_panel->last_index = 0;
        right_panel->per_page = MAX_ROWS;
        init_where_condition();
        load_data();

        return 1;
    } else if(strstr(command, "view") != NULL) {
        load_view_modal_data();
        return 1;
    } else if(strcmp(command, "close") == 0) {
        unload_view_modal_data();
        return 1;
    }

    return 0;
}

void load_view_modal_data() {
    int per_page = right_panel->per_page;

    if(right_panel->last_index < right_panel->per_page) {
        per_page = right_panel->last_index;
    }
    int offset = right_panel->last_index - per_page + right_panel->active_item - 1;
    int limit = 1;
    if((query_all(right_panel->connection, right_panel->connection->database->tables[right_panel->connection->database->active_table], limit, offset, right_panel->where_condition)) != 0) {
        return;
    }
    right_panel->show_view_modal = 1;
}

void unload_view_modal_data()
{
    if(right_panel->show_view_modal == 1) {
        mysql_free_result(right_panel->connection->result);
    }
    right_panel->show_view_modal = 0;
    right_panel->view_modal_row_offset = 0;
}

int right_panel_activate_key(void) {
    return KEY_F(2);
};