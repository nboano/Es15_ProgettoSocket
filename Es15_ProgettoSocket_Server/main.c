#include "lib/utility_c/utilities.h"
#include "HTTP.c"
#include "DB.c"

#define PORT 1040

#define JSON_RESPONSE_HEADERS {\
        {"Access-Control-Allow-Origin", "*"},\
        {"Content-Type", "application/json; charset=UTF-8"},\
        {"Connection", "Close"}\
    }\

const HTTPResponse HTTP_NOT_FOUND = {
    404, "NOT FOUND", JSON_RESPONSE_HEADERS,
    "{\"error\":{\"code\":404,\"message\":\"Risorsa non trovata.\"}}"
};

const HTTPResponse HTTP_METHOD_NOT_ALLOWED = {
    405, "METHOD NOT ALLOWED", JSON_RESPONSE_HEADERS,
    "{\"error\":{\"code\":405,\"message\":\"Metodo HTTP non consentito.\"}}"
};

NEW_SOCKET(miosock, BUFFER_LEN);

unsigned long ServingThread(void* param) {

    SOCKET socket_clone = miosock.ClientSocket;

    HTTPRequest req = HTTP.ReadRequestFromSocket(socket_clone);
    HTTPResponse resp;
    

    printf("%-15s THREAD %-5x - %-5s %-30s %s\n", (char*)param, GetCurrentThreadId(), req.Method, req.Path, req.Body);

    if(strcmp(req.Path, "/") == 0) {

        if(strcmp(req.Method, "POST") == 0) {
            HTTPResponse ok_resp = {
                200, "OK",  JSON_RESPONSE_HEADERS
            };
            sprintf(ok_resp.Body, "{\"ok\":true,\"data\":{\"ip\":\"%s\"}}", (char*)param);

            resp = ok_resp;
        } else {
            resp = HTTP_METHOD_NOT_ALLOWED;
        }

    } else if(0) {

    } else {
        resp = HTTP_NOT_FOUND;
    }

    HTTP.SendResponseToSocket(socket_clone, resp);
}

void login(const char* username, const char* password) {
    Database.AddParameter("@username", username);
    Database.AddParameter("@password", password);
    MYSQL_RES* result = Database.ExecuteQuery("SELECT username FROM autisti WHERE username = @username AND password = @password");

    MYSQL_ROW row = mysql_fetch_row(result);
    if(row) {
        printf("LOGIN OK: %s\n", username);
    } else {
        printf("LOGIN FALLITO: %s\n", username);
    }

    mysql_free_result(result);
}

int main() 
{
    DB_Init();

    login("consegne001", "consegne001");

    login("consegne002", "consegne002");

    login("consegne005", "consegne005");

    char message[50] = "";
    sprintf(message, "In attesa sulla porta %i.", PORT);

    Console.SetCursorPosition(Console.GetWidth() - strlen(message) ,0);

    printf(message);

    Console.SetCursorPosition(0,1);

    miosock.Init();
    miosock.Listen(PORT);
    
    while(1) {
        Sleep(10);

        char* connected_ip = miosock.WaitForConnection();

        HANDLE serving_thread = CreateThread(NULL, 0, ServingThread, (void*)connected_ip, 0, NULL);
    }
}
