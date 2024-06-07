//
// Created by stefkoff on 6/3/24.
//

#include "../include/win.h"

Content* create_content(int h, int w, int y, int x, WinCallback callback)
{
    Content* content = (Content*) malloc(sizeof(struct _CONTENT));
    WINDOW* window = newwin(h, w, y, x);

    if(window == NULL) {
        return NULL;
    }

    content->window = window;
    content->width = w;
    content->height = h;
    content->startX = x;
    content->startY = y;
    content->callbacks = callback;
    content->is_active = 0;

    return content;
}

Content* get_content_by_name(char* name, WIN* win) {
    if(win->contents == NULL) {
        return NULL;
    }

    for(int c = 0; c < win->contents_size; c++) {
        if(win->contents[c]->name != NULL && strcmp(win->contents[c]->name, name) == 0) {
            return win->contents[c];
        }
    }

    return NULL;
}

void attach_content(WIN* win, Content* content) {
    if(win->contents_size <= 0) {
        win->contents = (Content**) malloc(sizeof(struct _CONTENT*));
        win->contents_size = 1;

        win->contents[win->contents_size - 1] = content;
        return;
    }

    win->contents = realloc(win->contents, sizeof(struct _CONTENT*) * (win->contents_size + 1));
    win->contents[win->contents_size++] = content;
}

void destroy_content(Content* content)
{
    wclear(content->window);
    delwin(content->window);
    free(content);
}

void destroy_win(WIN* win)
{
    for(int i = 0; i < win->contents_size; i++){
        destroy_content(win->contents[i]);
    }

    free(win);
}

int save_state(WIN* win, char* name, int value)
{
    if(!win->num_states || win->num_states == 0) {
        win->num_states = 1;
        if(win->states == NULL) {
            return 1;
        }
        win->states[win->num_states - 1] = (WinState*) malloc(sizeof(struct WIN_STATES));

        if(win->states[win->num_states - 1] == NULL) {
            free(win->states);
            return 2;
        }
        win->states[win->num_states - 1]->name = (char*) malloc(sizeof(char) * 256);

        if(win->states[win->num_states - 1]->name == NULL) {
            free(win->states[win->num_states - 1]);
            free(win->states);
            return 3;
        }
        strcpy(win->states[win->num_states - 1]->name, name);
        win->states[win->num_states - 1]->value = value;

        return 0;
    }

    for(int s = 0; s < win->num_states; s++) {
        if(strcmp(win->states[s]->name, name) == 0) {
            win->states[s]->value = value;
            return 0;
        }
    }

    win->num_states++;
    win->states = (WinState**) realloc(win->states, sizeof(struct WIN_STATES*) * win->num_states);
    if(win->states == NULL) {
        return 1;
    }

    win->states[win->num_states - 1] = (WinState*) malloc(sizeof(struct WIN_STATES));
    if(win->states[win->num_states - 1] == NULL) {
        free(win->states);
        return 2;
    }

    win->states[win->num_states - 1]->name = (char*) malloc(sizeof(char) * 256);
    if(win->states[win->num_states - 1]->name == NULL) {
        free(win->states[win->num_states - 1]);
        free(win->states);
        return 3;
    }
    strcpy(win->states[win->num_states - 1]->name, name);
    win->states[win->num_states - 1]->value = value;

    return 0;
}

int get_state(WIN* win, char* name) {
    if(win->states == NULL || win->num_states <= 0) {
        return -1;
    }

    for(int s = 0; s < win->num_states; s++) {
        if(strcmp(win->states[s]->name, name) == 0) {
            return win->states[s]->value;
        }
    }

    return -1;
}