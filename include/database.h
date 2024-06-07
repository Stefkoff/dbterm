//
// Created by stefkoff on 6/3/24.
//

#include <mysql/mysql.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#ifndef NCOURSESTEST_DATABASE_H
#define NCOURSESTEST_DATABASE_H

#define MAX_HOST_SIZE 255
#define MAX_USER_SIZE 32
#define MAX_PASSWORD_SIZE 32
#define MAX_DATABASE_SIZE 64

#define MAX_ROWS 100

typedef struct _COLUMN {
    char* name;
    uint16_t name_size;
    int value_size;
} Column;

typedef struct _TABLE {
    Column** columns;
    char* name;
    uint16_t columns_size;
    uint16_t total_rows;
} Table;

typedef struct _DATABASE {
    Table** tables;
    int tables_size;
    int active_table;
} Database;

typedef struct _CONNECTION {
    MYSQL* connection;
    MYSQL_RES* result;
    Database* database;
    char* host;
    char* user;
    char* password;
    char* db_name;
    uint8_t is_connected;
} Connection;

Connection* create_connection(char*, char*, char*, char*);
uint8_t connect_mysql(Connection*);
void load_database(Connection* connection);
int load_tables(Connection*);
int load_columns(Connection*, Table*);
void destroy_connection(Connection*);
int query_all(Connection *conn, Table* table, int, int, char*);
uint8_t get_total_rows(Connection*, Table*, int, int, char*);

#endif //NCOURSESTEST_DATABASE_H
