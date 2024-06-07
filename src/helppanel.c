//
// Created by stefkoff on 6/7/24.
//

#include "../include/helppanel.h"

HelpPanel* help_panel;

int init_help_panel(Content* content, Connection* conn)
{

    if(help_panel != NULL) {
        free(help_panel->lines);
        free(help_panel);
    }
    help_panel = (HelpPanel*) malloc(sizeof(struct HELP_PANEL));

    if(help_panel == NULL) {
        return 1;
    }

    help_panel->is_visible = 0;
    help_panel->content = content;
    help_panel->line_offset = 0;
    help_panel->lines_len = 0;
    help_panel->file_loaded = 0;

    FILE* fp;
    uint64_t index;
    char* line = NULL;
    size_t len = 0;
    ssize_t read;

    fp = fopen("help.txt", "r");

    if(fp == NULL) {
        free(help_panel);
        return 1;
    }

    index = 0;
    while((read = getline(&line, &len, fp)) != -1) {
        if(read == 1) {
            continue;
        }
        if(help_panel->lines == NULL) {
            help_panel->lines = (char**) malloc(sizeof(char) * read);
        } else {
            help_panel->lines = (char**) realloc(help_panel->lines, sizeof(char) * read * (index + 1));
        }

        help_panel->lines[index] = (char *) malloc(sizeof(char*) * read);

        snprintf(help_panel->lines[index], read, "%s", line);
        if(help_panel->lines[index][read] == '\n') {
            help_panel->lines[index][read] = '\0';
        }

        index++;
    }

    help_panel->lines_len = index;
    help_panel->file_loaded = 1;

    return 0;
}

void help_panel_on_activate(int is_active)
{
    help_panel->is_visible = is_active;
}

void help_panel_on_key_event(int key)
{
    switch (key) {
        // ESC
        case 27:
            if(help_panel->is_visible == 1) {
                help_panel->is_visible = 0;
            }
            break;
        case KEY_DOWN:
        case KEY_UP:
            if (help_panel->is_visible == 1) {
                int incr = (key == KEY_UP ? -1 : 1);
                help_panel->line_offset += incr;
                if (help_panel->line_offset <= 0) {
                    help_panel->line_offset = 0;
                } else if (help_panel->line_offset > help_panel->lines_len) {
                    help_panel->line_offset = help_panel->lines_len;
                }
            }

            break;
    }
}

void render_help_panel(Content* content, int is_active, WIN* window)
{
    if(help_panel->is_visible == 0 || help_panel->file_loaded == 0) {
        return;
    }

    wclear(content->window);
    box(content->window, 0, 0);
    draw_help_panel();
    wrefresh(content->window);
}

void draw_help_panel() {
    if(help_panel->lines_len <= 0 || help_panel->lines == NULL) {
        return;
    }

    uint64_t index;

    index = 0;
    for(uint64_t i = help_panel->line_offset; i < help_panel->lines_len; i++) {
        if(index >= HELP_PANEL_HEIGHT - 2) {
            break;
        }

        uint16_t text_size;
        text_size = strlen(help_panel->lines[i]);

        if(text_size > HELP_PANEL_WIDTH - 2) {
            uint16_t printed;
            int int_print;

            int_print = 0;
            printed = 0;
            char* to_print = help_panel->lines[i];
            while(printed < text_size) {
                printed+=(HELP_PANEL_WIDTH - 2);

                mvwprintw(help_panel->content->window, 1 + (index++), 1, "%.*s", HELP_PANEL_WIDTH - 2, to_print);
                to_print+=printed;
            }

            continue;
        }

        mvwprintw(help_panel->content->window, index++ + 1, 1, "%s", help_panel->lines[i]);
    }
}

int on_help_panel_command(char* command) {
    return 0;
}