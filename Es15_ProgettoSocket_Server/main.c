#include "lib/utility_c/utilities.h"
#include "HTTP.c"
#include "DB.c"
#include "Server.c"

#define PORT 1040

NEW_SOCKET(miosock, BUFFER_LEN);

unsigned long ServingThread(void* param) {

    SOCKET socket_clone = miosock.ClientSocket;

    HTTPRequest req = HTTP.ReadRequestFromSocket(socket_clone);
    HTTPResponse resp;
    
    printf("-> %-15s THREAD %-5x - %-5s %-30s %s\n", (char*)param, GetCurrentThreadId(), req.Method, req.Path, req.Body);

    if(strcmp(req.Path, "/") == 0) {
        resp = HTTP_BUILD_OK_RESPONSE("{\"server_up\":true}");
    }
    else if(strcmp(req.Path, "/login") == 0) {
        resp = SERVER_HANDLE_LOGIN(req);
    } 
    else if(strcmp(req.Path, "/logout") == 0) {
        resp = SERVER_HANDLE_LOGOUT(req);
    } else {
        resp = HTTP_NOT_FOUND;
    }

    HTTP.SendResponseToSocket(socket_clone, resp);
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
