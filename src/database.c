//
// Created by stefkoff on 6/3/24.
//

#include "../include/database.h"



Connection* create_connection(char* host, char* user, char* password, char* database)
{
    Connection* conn = (Connection*) malloc(sizeof(struct _CONNECTION));

    if(conn == NULL) {
        printf("Could not allocate memory for connection\n");
        free(conn);
        return NULL;
    }

    conn->is_connected = 0;

    conn->host = (char*) malloc(sizeof(char) * MAX_HOST_SIZE);

    if(conn->host == NULL) {
        printf("Could not allocate memory\n");
        free(conn);
        return NULL;
    }

    conn->user = (char*) malloc(sizeof(char) * MAX_USER_SIZE);

    if(conn->user == NULL) {
        printf("Could not allocate memory\n");
        free(conn->host);
        free(conn);
        return NULL;
    }

    conn->password = (char*) malloc(sizeof(char) * MAX_PASSWORD_SIZE);

    if(conn->password == NULL) {
        printf("Could not allocate memory\n");
        free(conn->user);
        free(conn->host);
        free(conn);

        return NULL;
    }

    conn->db_name = (char*) malloc(sizeof(char) * MAX_DATABASE_SIZE + 1);

    if(conn->db_name == NULL) {
        printf("Could not allocate memory\n");
        free(conn->password);
        free(conn->user);
        free(conn->host);
        free(conn);
        return NULL;
    }

    strcpy(conn->db_name, database);
    strcpy(conn->host, host);
    strcpy(conn->user, user);
    strcpy(conn->password, password);

    conn->connection = mysql_init(NULL);

    if(conn->connection == NULL) {
        printf("Could not init MySQL\n");
        free(conn);
        return NULL;
    }

    uint8_t connected;
    connected = connect_mysql(conn);

    if(connected != 0) {
        printf("Could not connect to database server\n");
        free(conn);
        return NULL;
    }

    return conn;
}

uint8_t connect_mysql(Connection* conn)
{
    if(conn->is_connected == 1) {
        mysql_close(conn->connection);

        conn->connection = mysql_init(NULL);
    }
    conn->connection = mysql_real_connect(conn->connection, conn->host, conn->user, conn->password, conn->db_name, 0, NULL, 0);

    if(conn->connection == NULL) {
        return 1;
    }

    conn->is_connected = 1;

    return 0;
}

int load_tables(Connection * conn)
{
    if(conn->database != NULL) {
        free(conn->database);
        conn->database = NULL;
    }
    conn->database = (Database* ) malloc(sizeof(struct _DATABASE));

    conn->result = mysql_list_tables(conn->connection, "%");
    if(conn->result == NULL) {
        printf("Could not list tables\n");
        return 1;
    }

    MYSQL_ROW row;
    int index = 0;

    conn->database = (Database* ) malloc(sizeof(struct _DATABASE));
    if(conn->database == NULL) {
        printf("Could not allocate memory\n");
        return 2;
    }
    conn->database->active_table = 0;
    conn->database->tables = (Table**)malloc(sizeof(struct _TABLE) * (conn->result->row_count * 50));
    if(conn->database->tables == NULL) {
        printf("Could not allocate memory\n");
        return 3;
    }
    while((row = mysql_fetch_row(conn->result)) != NULL) {
        conn->database->tables[index] = (Table *) malloc(sizeof(struct _TABLE));
        if(conn->database->tables[index] == NULL) {
            printf("Could not allocate memory\n");
            free(conn->database->tables);
            free(conn->database);
            mysql_free_result(conn->result);
            return 3;
        }
        conn->database->tables[index]->name = (char*) malloc(sizeof(char) * strlen(row[0]) + 1);
        conn->database->tables[index]->total_rows = 0;
        if(conn->database->tables[index]->name == NULL) {
            printf("Could not allocate memory\n");
            free(conn->database->tables[index]);
            free(conn->database->tables);
            free(conn->database);
            mysql_free_result(conn->result);
            return 4;
        }
        strcpy(conn->database->tables[index]->name, row[0]);

        index++;
    }
    mysql_free_result(conn->result);
    conn->database->tables_size = index;

    for(int i = 0; i < conn->database->tables_size; i++) {
        if((load_columns(conn, conn->database->tables[i])) != 0) {
            continue;
        }
    }

    return 0;
}

int query_all(Connection *conn, Table* table, int limit, int offset, char* where)
{
    if(get_total_rows(conn, table, limit, offset, where) != 0) {
        return 1;
    }

    char* query = (char*) malloc(sizeof(char) * 2048);
    sprintf(query, "SELECT * FROM `%s`", table->name);

    if(where != NULL && strlen(where) > 0) {
        sprintf(query, "%s WHERE %s", query, where);
    }

    sprintf(query, "%s LIMIT %d OFFSET %d;", query, limit, offset);

    if(mysql_query(conn->connection, query)) {
        return 1;
    }

    conn->result = mysql_store_result(conn->connection);

    return 0;
}

uint8_t get_total_rows(Connection* conn, Table* table, int limit, int offset, char* where)
{
    char* query = (char*) malloc(sizeof(char) * 2048);
    sprintf(query, "SELECT COUNT(*) FROM `%s`", table->name);

    if(where != NULL && strlen(where) > 0) {
        sprintf(query, "%s WHERE %s", query, where);
    }

    sprintf(query, "%s LIMIT %d OFFSET %d;", query, limit, offset);

    if(mysql_query(conn->connection, query)) {
        printf("Could not make query\n");
        return 1;
    }


    conn->result = mysql_store_result(conn->connection);

    if(conn->result == NULL) {
        printf("Could not store result\n");
        return 2;
    }

    MYSQL_ROW row = mysql_fetch_row(conn->result);

    if(row != NULL) {
        char* pEnd;
        table->total_rows = strtol(row[0], &pEnd, 10);
    }

    mysql_free_result(conn->result);

    return  0;
}

int load_columns(Connection *conn, Table* table)
{
    char query[256];
    sprintf(query, "DESCRIBE `%s`;", table->name);

    if(mysql_query(conn->connection, query)) {
        printf("Could not make query\n");
        return 1;
    }


    conn->result = mysql_store_result(conn->connection);

    if(conn->result == NULL) {
        printf("Could not store result\n");
        return 2;
    }

    MYSQL_ROW row;
    int index = 0;
    table->columns_size = conn->result->row_count + 1;
    table->columns = (Column **) malloc(sizeof(struct _COLUMN) * conn->result->row_count + 1);

    if(table->columns == NULL) {
        printf("Could not allocate memory\n");
        mysql_free_result(conn->result);
        return 3;
    }

    // add the default Number columns at start
    table->columns[index] = (Column *) malloc(sizeof(struct _COLUMN));
    if(table->columns[index] == NULL) {
        return 3;
    }
    table->columns[index]->name_size = 2;
    table->columns[index]->name = (char*) malloc(sizeof(char) * 64);
    if(table->columns[index]->name == NULL) {
        free(table->columns[index]);
        return 4;
    }
    snprintf(table->columns[index]->name, 63, "%s", "N");
    table->columns[index++]->value_size = 255; // TODO: find a way to do this dynamic
    // ---- end here

    while((row = mysql_fetch_row(conn->result)) != NULL ) {
        table->columns[index] = (Column *) malloc(sizeof(struct _COLUMN));
        if(table->columns[index] == NULL) {
            table->columns_size--;
            continue;
        }
        table->columns[index]->name_size = strlen(row[0]);
        table->columns[index]->name = (char*) malloc(sizeof(char) * (strlen(row[0]) + 1));
        if(table->columns[index]->name == NULL) {
            free(table->columns[index]);
            table->columns_size--;
            continue;
        }
        snprintf(table->columns[index]->name, 63, "%s", row[0]);
        table->columns[index]->value_size = 255; // TODO: find a way to do this dynamic
        index++;
    }

    mysql_free_result(conn->result);

    return get_total_rows(conn, table, MAX_ROWS, 0, NULL);
}

void destroy_connection(Connection* conn)
{
    mysql_close(conn->connection);

    free(conn->database);
    free(conn);
}
