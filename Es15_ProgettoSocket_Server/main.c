#include "lib/utility_c/utilities.h"
#include "HTTP.c"
#include "DB.c"
#include "Server.c"

#define PORT 1040

NEW_SOCKET(miosock, BUFFER_LEN);

unsigned long ServingThread(void* param) {

    SOCKET socket_clone = miosock.ClientSocket;

    HTTPRequest req = HTTP.ReadRequestFromSocket(socket_clone);

    time_t t = time(NULL);
    struct tm* tm = localtime(&t);
    printf("%d-%02d-%02d %02d:%02d:%02d ", tm->tm_year + 1900, tm->tm_mon + 1, tm->tm_mday, tm->tm_hour, tm->tm_min, tm->tm_sec);
    printf("%-15s THREAD %-5x - %-5s %-15s\n", (char*)param, GetCurrentThreadId(), req.Method, req.Path);

    HTTPResponse resp = SERVER_HANDLE_REQUEST(req);

    HTTP.SendResponseToSocket(socket_clone, resp);
}

void print_vline() {
    int console_h = Console.GetHeight();
    int l = Console.GetWidth() - 32;

    Console.SetCursorPosition(l, 0);
    for(int i = 0; i < console_h; i++) {
        printf("%c", 221);
        Console.SetCursorPosition(l, i);
    }
}

int main() 
{

    print_vline();

    Database.Connect();

    Console.SetCursorPosition(Console.GetWidth() - 30, 1);

    printf("DATABASE : ");
    Color.Set(Color.Green);
    printf("CONNESSO");
    Color.Reset();

    miosock.Init();
    miosock.Listen(PORT);

    Console.SetCursorPosition(Console.GetWidth() - 30, 0);
    printf("SERVER   : ");
    Color.Set(Color.Green);
    printf("OPERATIVO @ ");
    Color.Set(Color.Aqua);
    printf("%i", PORT);
    Color.Reset();

    Console.SetCursorPosition(0,0);
    
    while(true) {
        Sleep(10);

        char* connected_ip = miosock.WaitForConnection();

        HANDLE serving_thread = CreateThread(NULL, 0, ServingThread, (void*)connected_ip, 0, NULL);
    }
}
