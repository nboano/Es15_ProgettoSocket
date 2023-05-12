#include "lib/utility_c/utilities.h"
#include "HTTP.c"
#include "DB.c"

#define PORT 1040

const HTTPResponse HTTP_NOT_FOUND = {
    404, "NOT FOUND",
    {
        {"Content-Type", "text/html; charset=UTF-8"},
        {"Connection", "Close"}
    },
    "<h1>Risorsa non trovata.</h1><hr/>Mi dispiace, ma la risorsa che cerchi non &egrave; stata trovata. Ti invitiamo a cercare da un'altra parte &#129392;&#129392;."
};

MYSQL* db_instance;

NEW_SOCKET(miosock, BUFFER_LEN);

unsigned long ServingThread(void* param) {

    SOCKET socket_clone = miosock.ClientSocket;

    HTTPRequest req = HTTP.ReadRequestFromSocket(socket_clone);
    HTTPResponse resp;

    printf("%-15s THREAD %-5x - %-5s %-30s %s\n", (char*)param, GetCurrentThreadId(), req.Method, req.Path, req.Body);

    if(strcmp(req.Path, "/") == 0) {

        HTTPResponse ok_resp = {
            200, "OK",
            {
                {"Content-Type", "text/html; charset=UTF-8"},
                {"Connection", "Close"}
            }
        };
        sprintf(ok_resp.Body, "<h1>CIAO %s!</h1><hr>Ma cosa fai? &#128128;&#128128;<br><br>Questa &egrave; un'API.", (char*)param);

        resp = ok_resp;

    } else if(0) {

    } else {
        resp = HTTP_NOT_FOUND;
    }

    HTTP.SendResponseToSocket(socket_clone, resp);
}

int main() 
{

    db_instance = DB_Init();

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
