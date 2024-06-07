//
// Created by stefkoff on 6/3/24.
//

#include <ncurses.h>
#include <stdlib.h>

#include "database.h"

#ifndef NCOURSESTEST_WIN_H
#define NCOURSESTEST_WIN_H

struct _CONTENT;
struct _WIN;

typedef void (*RENDER_FUNC)(struct _CONTENT*, int, struct _WIN*);
typedef int (*INIT_FUNC)(struct _CONTENT*, Connection*);
typedef void (*KEY_EVENT)(int);
typedef void (*ON_ACTIVE)(int);
typedef int (*HANDLE_COMMAND)(char*);
typedef int (*ACTIVATE_KEY)(void);

typedef struct WIN_STATES {
    char* name;
    int value;
} WinState;

typedef struct CALLBACKS {
    RENDER_FUNC render;
    INIT_FUNC init;
    KEY_EVENT key_event;
    ON_ACTIVE on_active;
    HANDLE_COMMAND on_command;
    ACTIVATE_KEY activate_key;
} WinCallback;

typedef struct _CONTENT {
    WINDOW* window;
    WinCallback callbacks;
    uint8_t is_active;
    int width;
    int height;
    int startX;
    int startY;
    char* name;
} Content;


typedef struct _WIN {
    Content** contents;
    WinState** states;
    uint8_t skip_key_event;
    int contents_size;
    char* command;
    int num_states;
    uint8_t re_init;
} WIN;

Content * create_content(int height, int width, int startY, int startX, WinCallback callbacks);
void destroy_content(Content*);
void attach_content(WIN*, Content*);
void destroy_win(WIN*);
Content* get_content_by_name(char*, WIN*);
int save_state(WIN*, char*, int);
int get_state(WIN*, char*);

#endif //NCOURSESTEST_WIN_H
