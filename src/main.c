#include <ncurses.h>
#include <locale.h>
#include <getopt.h>
#include "../include/win.h"
#include "../include/leftpanel.h"
#include "../include/controlpanel.h"
#include "../include/rightpanel.h"
#include "../include/dbpanel.h"
#include "../include/helppanel.h"


int main(int argc, char** argv)
{
    char* hostname;
    char* user;
    char* password;
    char* database;

    hostname = (char*) malloc(sizeof(char*) * MAX_HOST_SIZE);
    if(hostname == NULL) {
        printf("Could not allocate memory\n");
        return 1;
    }

    strcpy(hostname, "localhost");
    user = (char*) malloc(sizeof(char) * MAX_USER_SIZE);

    if(user == NULL) {
        printf("Could not allocate memory\n");
        free(hostname);
        return 1;
    }

    char *userFromEnv;
    if((userFromEnv = getenv("USER")) != NULL) {
        strcpy(user, userFromEnv);
    }
    password = (char*) malloc(sizeof(char) * MAX_PASSWORD_SIZE);

    if(password == NULL) {
        free(hostname);
        free(user);
        printf("Could not allocate memory\n");
        return 1;
    }

    database = (char*) malloc(sizeof(char) * MAX_DATABASE_SIZE);

    if(database == NULL) {
        free(hostname);
        free(user);
        free(password);
        printf("Could not allocate memory\n");
        return 1;
    }

    strcpy(database, "mysql");


    int option;
    while((option = getopt(argc, argv, "h:u:p:d:")) != -1) {
        switch (option) {
            case 'h':
                memset(hostname, '\0', MAX_HOST_SIZE);
                strcpy(hostname, optarg);
                break;
            case 'u':
                memset(user, '\0', MAX_USER_SIZE);
                strcpy(user, optarg);
                break;
            case 'p':
                memset(password, '\0', MAX_PASSWORD_SIZE);
                strcpy(password, optarg);
                break;
            case 'd':
                memset(database, '\0', MAX_DATABASE_SIZE);
                strcpy(database, optarg);
                break;
        }
    }

    /**
     * TODO: prompt user to enter password, if not provided
     */

    Connection* mysql = create_connection(hostname, user, password, database);

    if(mysql == NULL) {
        free(hostname);
        free(user);
        free(password);
        free(database);
        return 2;
    }

    load_tables(mysql);


    setlocale(LC_CTYPE, "");


    initscr();			/* Start curses mode 		*/
    clear();
    cbreak();			/* Line buffering disabled, Pass on
					 * everty thing to me 		*/
    noecho();
    keypad(stdscr, TRUE);		/* I need that nifty F1 	*/
    curs_set(0);
    refresh();
    noecho();

    WIN* window;
    window = (WIN*) malloc(sizeof(struct _WIN));
    if(window == NULL) {
        printf("Could not allocate memory\n");
        exit(1);
    }
    window->states = (WinState**) malloc(sizeof(struct WIN_STATES*));
    window->num_states = 0;
    window->command = (char*) malloc(sizeof(char) * 20);
    window->skip_key_event = 0;
    window->contents_size = 0;
    window->re_init = 0;

    WinCallback callbacks = {
            .render = render_left_panel,
            .init = init_left_panel,
            .key_event = left_panel_on_key_event,
            .on_active = left_panel_on_active,
            .on_command = on_left_panel_command,
            .activate_key = left_panel_activate_key
    };
    Content* left_content_content = create_content(LINES, 30, 0, 0, callbacks);
    left_content_content->is_active = 1;
    left_content_content->name = (char*) malloc(sizeof(char) * 5);
    if(left_content_content->name == NULL) {
        printf("Could not allocate memory\n");
        exit(1);
    }
    sprintf(left_content_content->name, "left");
    attach_content(window, left_content_content);

    WinCallback control_callbacks = {
            .render = render_control_panel,
            .init = init_control_panel,
            .on_active = control_panel_on_active,
            .key_event = control_panel_on_key_event,
            .on_command = on_control_panel_command
    };

    Content* control_content = create_content(3, COLS - 2, LINES - 4, 1, control_callbacks);
    attach_content(window, control_content);

    WinCallback right_callbacks = {
            .render = render_right_panel,
            .init = init_right_panel,
            .on_active = right_panel_on_active,
            .key_event = right_panel_on_key_event,
            .on_command = on_right_panel_command,
            .activate_key = right_panel_activate_key
    };
    Content* right_content = create_content(LINES, COLS - 30, 0, 30, right_callbacks);
    right_content->name = (char*) malloc(sizeof(char) * 5);
    if(right_content->name == NULL) {
        printf("Could not allocate memory\n");
        exit(1);
    }
    sprintf(right_content->name, "right");
    attach_content(window, right_content);

    WinCallback db_panel_callbacks = {
            .render = render_db_panel,
            .init = init_db_panel,
            .on_active = db_panel_on_activate,
            .key_event = db_panel_on_key_event,
            .on_command = on_db_panel_command
    };

    Content* db_content = create_content(DB_PANEL_HEIGHT, DB_PANEL_WIDTH, LINES / 4, (COLS / 4), db_panel_callbacks);
    db_content->name = (char*) malloc(sizeof(char) * 10);
    if(db_content->name == NULL) {
        printf("Could not allocate memory\n");
        exit(1);
    }
    sprintf(db_content->name, "databases");
    attach_content(window, db_content);

    WinCallback help_panel_callbacks = {
            .render = render_help_panel,
            .init = init_help_panel,
            .on_active = help_panel_on_activate,
            .key_event = help_panel_on_key_event,
            .on_command = on_help_panel_command
    };

    Content* help_content = create_content(HELP_PANEL_HEIGHT, HELP_PANEL_WIDTH, 5, 5, help_panel_callbacks);
    help_content->name = (char*) malloc(sizeof(char) * 5);
    if(help_content->name == NULL) {
        printf("Could not allocate memory\n");
        exit(1);
    }
    sprintf(help_content->name, "help");
    attach_content(window, help_content);

    for(int w = 0; w < window->contents_size; w++) {
        window->contents[w]->callbacks.init(window->contents[w], mysql);
    }
    for(int w = 0; w < window->contents_size; w++) {
        window->contents[w]->callbacks.render(window->contents[w], 0, window);
    }
    refresh();
    while(1)
    {
        if(window->re_init == 1) {
            for(int w = 0; w < window->contents_size; w++) {
                window->contents[w]->callbacks.init(window->contents[w], mysql);

                if(w == 0) {
                    window->contents[w]->callbacks.on_active(1);
                }
            }
            window->re_init = 0;
        }
        for(int w = 0; w < window->contents_size; w++) {
            window->contents[w]->callbacks.render(window->contents[w], 0, window);
        }
        if(window->skip_key_event == 0) {
            int c = getch();
            int found_activate_key = 0;
            for(int w = 0; w < window->contents_size; w++) {
                if(window->contents[w]->callbacks.activate_key == NULL) {
                    continue;
                }
                int activate_key = window->contents[w]->callbacks.activate_key();
                if(activate_key == c) {
                    found_activate_key = 1;
                    for(int wa = 0; wa < window->contents_size; wa++) {
                        window->contents[wa]->callbacks.on_active(wa == w ? 1 : 0);
                    }
                }
            }

            if(found_activate_key == 1) {
                refresh();
                continue;
            }

            for(int w = 0; w < window->contents_size; w++) {
                window->contents[w]->callbacks.key_event(c);
            }
        } else {
            window->skip_key_event = 0;
        }



        if(strlen(window->command) > 0) {
            if(strcmp(window->command, "exit") == 0) {
                free(hostname);
                free(user);
                free(password);
                free(database);
                destroy_connection(mysql);
                destroy_win(window);
                endwin();			/* End curses mode		  */
                exit(0);
            } else {
                int command_applied = 0;
                for(int c = 0; c < window->contents_size; c++) {
                    command_applied = window->contents[c]->callbacks.on_command(window->command);
                    if(command_applied == 1) {
                        break;
                    }
                }

                if(command_applied == 1) {
                    strncpy(window->command, "", 20);
                    refresh();
                    continue;
                }

                Content* target_content = get_content_by_name(window->command, window);

                if(target_content == NULL) {
                    refresh();
                    continue;
                }

                for(int c = 0; c < window->contents_size; c++) {
                    window->contents[c]->callbacks.on_active(window->contents[c] == target_content ? 1 : 0);
                }
            }
        }

        strncpy(window->command, "", 20);
        refresh();
    }
}