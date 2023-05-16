#include "lib/utility_c/utilities.h"
#include "HTTP.c"
#include "DB.c"

#define PORT 1040

#define JSON_RESPONSE_HEADERS {\
        {"Access-Control-Allow-Origin", "*"},\
        {"Content-Type", "application/json; charset=UTF-8"},\
        {"Connection", "Close"}\
    }\

typedef struct t_sessionhandle {
    char Username[50];
    char Token[50];
} SessionHandle;

LLIST_DECLTYPE(SessionHandle)

LLIST_INIT(SessionList, SessionHandle);

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

HTTPResponse HTTP_BUILD_OK_RESPONSE(const char* data) {
    HTTPResponse OkResponse = {200, "OK", JSON_RESPONSE_HEADERS};
    sprintf(OkResponse.Body, "{\"ok\":true,\"data\":%s}", data);
    return OkResponse;
}

NEW_SOCKET(miosock, BUFFER_LEN);

char** perform_login(const char* username, const char* password);

char* generate_id(char *str, int size)
{
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

unsigned long ServingThread(void* param) {

    SOCKET socket_clone = miosock.ClientSocket;

    HTTPRequest req = HTTP.ReadRequestFromSocket(socket_clone);
    HTTPResponse resp;
    

    printf("%-15s THREAD %-5x - %-5s %-30s %s\n", (char*)param, GetCurrentThreadId(), req.Method, req.Path, req.Body);

    if(strcmp(req.Path, "/") == 0) {

        if(strcmp(req.Method, "POST") == 0) {

            char* username = strtok(req.Body, ";");
            char* password = strtok(NULL, ";");

            char** login_row = perform_login(username, password);

            if(login_row) {
                SessionHandle sess_member;
                strcpy(sess_member.Username, login_row[0]);
                generate_id(sess_member.Token, 25);

                SessionList.Append(sess_member);

                char resp_bf[512] = "";
                sprintf(resp_bf, "{\"token\":\"%s\",\"username\":\"%s\",\"ruolo\":%s,\"nome\":\"%s\",\"cognome\":\"%s\",\"status\":\"Login successful.\"}", sess_member.Token, sess_member.Username, login_row[1], login_row[2], login_row[3]);
                
                resp = HTTP_BUILD_OK_RESPONSE(resp_bf);
            } else {
                resp = HTTP_UNAUTHORIZED;
            }

        } else {
            resp = HTTP_METHOD_NOT_ALLOWED;
        }

    } else if(0) {

    } else {
        resp = HTTP_NOT_FOUND;
    }

    HTTP.SendResponseToSocket(socket_clone, resp);
}

char** perform_login(const char* username, const char* password) {

    Database.AddParameter("@username", username);
    Database.AddParameter("@password", password);
    MYSQL_RES* result = Database.ExecuteQuery("SELECT username, ruolo, nome, cognome FROM autisti WHERE username = @username AND password = @password");

    MYSQL_ROW row = mysql_fetch_row(result);

    mysql_free_result(result);

    return row;
}

int main() 
{
    Database.Connect();

    char message[50] = "";
    sprintf(message, "In attesa sulla porta %i.", PORT);

    Console.SetCursorPosition(Console.GetWidth() - strlen(message) ,0);

    printf(message);

    Console.SetCursorPosition(0,1);

    miosock.Init();
    miosock.Listen(PORT);
    
    while(true) {
        Sleep(10);

        char* connected_ip = miosock.WaitForConnection();

        HANDLE serving_thread = CreateThread(NULL, 0, ServingThread, (void*)connected_ip, 0, NULL);
    }
}
