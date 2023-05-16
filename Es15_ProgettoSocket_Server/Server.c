#pragma once

#include "lib/utility_c/utilities.h"
#include "HTTP.c"
#include "DB.c"

#define JSON_RESPONSE_HEADERS {\
        {"Access-Control-Allow-Origin", "*"},\
        {"Content-Type", "application/json; charset=UTF-8"},\
        {"Connection", "Close"}\
    }\

typedef struct t_userdata {
    char Username[50];
    char Nome[50];
    char Cognome[50];
    int Ruolo;
} UserData;

typedef struct t_sessionhandle {
    UserData UserData;
    char Token[50];
} SessionHandle;

LLIST_DECLTYPE(SessionHandle)

LLIST_INIT(SessionList, SessionHandle);

char** perform_login(const char* username, const char* password) {

    Database.AddParameter("@username", username);
    Database.AddParameter("@password", password);

    MYSQL_RES* result = Database.ExecuteQuery("SELECT username, ruolo, nome, cognome FROM autisti WHERE username = @username AND password = @password");

    MYSQL_ROW row = mysql_fetch_row(result);

    mysql_free_result(result);

    return row;
}

char* generate_id(char *str, int size) {
    srand(time(NULL));

    const char charset[] = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJK1234567890#!-+/=@";
    size--;
    for (int n = 0; n < size; n++) {
        int key = rand() % (int) (sizeof charset - 1);
        str[n] = charset[key];
    }
    str[size] = '\0';
    return str;
}

char* glob_username_q;

int Session_FindCurrentUser_CB(SessionHandle sh) {
    return strcmp(sh.UserData.Username, glob_username_q);
}

const HTTPResponse HTTP_NOT_FOUND = {
    404, "NOT FOUND", JSON_RESPONSE_HEADERS,
    "{\"ok\":false,\"error\":{\"code\":404,\"message\":\"Risorsa non trovata.\"}}"
};

const HTTPResponse HTTP_METHOD_NOT_ALLOWED = {
    405, "METHOD NOT ALLOWED", JSON_RESPONSE_HEADERS,
    "{\"ok\":false,\"error\":{\"code\":405,\"message\":\"Metodo HTTP non consentito.\"}}"
};

const HTTPResponse HTTP_UNAUTHORIZED = {
    401, "UNAUTHORIZED", JSON_RESPONSE_HEADERS,
    "{\"ok\":false,\"error\":{\"code\":401,\"message\":\"Credenziali errate o mancanti.\"}}"
};

const HTTPResponse HTTP_ALREADY_LOGGED_IN = {
    400, "BAD REQUEST", JSON_RESPONSE_HEADERS,
    "{\"ok\":false,\"error\":{\"code\":400,\"message\":\"Utente gi&agrave; loggato.\"}}"
};

HTTPResponse HTTP_BUILD_OK_RESPONSE(const char* data) {
    HTTPResponse OkResponse = {200, "OK", JSON_RESPONSE_HEADERS};
    sprintf(OkResponse.Body, "{\"ok\":true,\"data\":%s}", data);
    return OkResponse;
}

HTTPResponse SERVER_HANDLE_LOGIN(HTTPRequest req) {
    HTTPResponse resp;
    if(strcmp(req.Method, "POST") == 0) {

        char* username = strtok(req.Body, ";");
        char* password = strtok(NULL, ";");
        glob_username_q = username;

        SessionHandle* current_session_ptr = SessionList.Find(Session_FindCurrentUser_CB);
        SessionHandle current_session;

        if(current_session_ptr) {
            current_session = *current_session_ptr;

            printf("SESS %s - ", current_session.Token);
            Color.Set(Color.Yellow);
            printf("UTENTE %s HA GIA' EFFETTUATO IL LOGIN\n", current_session.UserData.Username);
            Color.Reset();

            resp = HTTP_ALREADY_LOGGED_IN;
        } else {

            char** login_row = perform_login(username, password);

            if(login_row) {
                strcpy(current_session.UserData.Username, login_row[0]);
                current_session.UserData.Ruolo = atoi(login_row[1]);
                strcpy(current_session.UserData.Nome, login_row[2]);
                strcpy(current_session.UserData.Cognome, login_row[3]);

                generate_id(current_session.Token, 25);

                SessionList.Append(current_session);

                printf("SESS %s - ", current_session.Token);
                Color.Set(Color.Green);
                printf("UTENTE %s HA EFFETTUATO IL LOGIN\n", current_session.UserData.Username);
                Color.Reset();

                char resp_bf[512] = "";
                sprintf(resp_bf, "{\"Token\":\"%s\",\"Username\":\"%s\",\"Ruolo\":%i,\"Nome\":\"%s\",\"Cognome\":\"%s\",\"Status\":\"Login eseguito correttamente.\"}", current_session.Token, current_session.UserData.Username, current_session.UserData.Ruolo, current_session.UserData.Nome, current_session.UserData.Cognome);
                
                resp = HTTP_BUILD_OK_RESPONSE(resp_bf);
            } else {

                Color.Set(Color.Red);
                printf("TENTATIVO LOGIN CON CREDENZIALI ERRATE O MANCANTI\n");
                Color.Reset();

                resp = HTTP_UNAUTHORIZED;
            }

        }
    } else {
        resp = HTTP_METHOD_NOT_ALLOWED;
    }
    return resp;
}

HTTPResponse SERVER_HANDLE_LOGOUT(HTTPRequest req) {
    HTTPResponse resp;
    if(strcmp(req.Method, "POST") == 0) {
        char* username = strtok(req.Body, ";");
        glob_username_q = username;

        int current_session_index = SessionList.FindIndex(Session_FindCurrentUser_CB);

        if(current_session_index != -1) {

            printf("SESS %s - ", SessionList.At(current_session_index)->Token);

            SessionList.RemoveAt(current_session_index);

            Color.Set(Color.Red);
            printf("UTENTE %s HA EFFETTUATO IL LOGOUT\n", username, current_session_index);
            Color.Reset();

            resp = HTTP_BUILD_OK_RESPONSE("{\"Status\":\"Logout eseguito correttamente.\"}");
        } else {

            Color.Set(Color.Red);
            printf("TENTATIVO LOGOUT CON CREDENZIALI ERRATE O MANCANTI\n");
            Color.Reset();

            resp = HTTP_UNAUTHORIZED;
        }
    } else {
        resp = HTTP_METHOD_NOT_ALLOWED;
    }
    return resp;
}
