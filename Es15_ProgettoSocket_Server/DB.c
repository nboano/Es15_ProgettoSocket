#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "lib/utility_c/utilities.h"

#include "C:\Program Files\MySQL\MySQL Server 8.0\include\mysql.h"
#pragma comment (lib, "C:\\Program Files\\MySQL\\MySQL Server 8.0\\lib\\libmysql.lib")

typedef struct t_database {
    const char* Host;
    const char* Username;
    const char* DatabaseName;
    const char* Password;

    MYSQL* Connection;

    void(*Connect)();
    
    MYSQL_RES* (*ExecuteQuery)(const char*);

    void (*AddParameter)(const char*, const char*);
} DatabaseHandler;

DatabaseHandler Database;

#define DB_HOST     "127.0.0.1"
#define DB_USER     "4c_inf_progetto-sistemi"
#define DB_NAME     "4c_inf_progetto-sistemi"
#define DB_PASSWORD "4c_inf_progetto-sistemi"

void DB_Init() {

    Database.Connection = mysql_init(NULL);
    
    if(!mysql_real_connect(Database.Connection, Database.Host, Database.Username, Database.Password, Database.DatabaseName, 0, NULL, 0)) {
        MsgBox.Error(mysql_error(Database.Connection), "ERRORE CONNESSIONE MYSQL");
        exit(0);
    };
}

MYSQL_RES* DB_ExecuteQuery(const char* query) {
    mysql_query(Database.Connection, query);
    return mysql_store_result(Database.Connection);
}

void DB_AddParam(const char* param_name, const char* param_value) {
    char command_bf[255] = "";
    sprintf(command_bf, "SET %s = '%s'", param_name, param_value);
    Database.ExecuteQuery(command_bf);
}

DatabaseHandler Database = {
    DB_HOST,
    DB_USER,
    DB_NAME,
    DB_PASSWORD,
    NULL,
    &DB_Init,
    &DB_ExecuteQuery,
    &DB_AddParam
};