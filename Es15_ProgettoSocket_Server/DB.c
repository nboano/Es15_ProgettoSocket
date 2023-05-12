#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "lib/utility_c/utilities.h"

#include "C:\Program Files\MySQL\MySQL Server 8.0\include\mysql.h"
#pragma comment (lib, "C:\\Program Files\\MySQL\\MySQL Server 8.0\\lib\\libmysql.lib")

const char* DB_HOST = "127.0.0.1";
const char* DB_USER = "4c_inf_progetto-sistemi";
const char* DB_NAME = "4c_inf_progetto-sistemi";
const char* DB_PASSWORD = "4c_inf_progetto-sistemi";

MYSQL* DB_Init() {

    MYSQL* conn = mysql_init(NULL);
    
    if(!mysql_real_connect(conn, DB_HOST, DB_USER, DB_PASSWORD, DB_NAME, 0, NULL, 0)) {
        MsgBox.Error(mysql_error(conn), "ERRORE MYSQL");
        exit(0);
    };

    return conn;
}