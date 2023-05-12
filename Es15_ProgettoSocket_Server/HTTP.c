#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define BUFFER_LEN 2048

#define MAX_HEADERS 16

typedef struct t_HTTPRequest {

    char Method[8];
    char Path[255];
    char Body[BUFFER_LEN];

} HTTPRequest;

typedef struct t_HTTPResponse {

    int StatusCode;
    char StatusText[32];

    const char* Headers[MAX_HEADERS][2];

    char Body[BUFFER_LEN];

} HTTPResponse;

typedef struct t_HTTPMethods {

    HTTPRequest (*ReadRequestFromSocket)(unsigned long socket_handle);
    void (*SendResponseToSocket)(unsigned long socket_handle, HTTPResponse response);

} HTTPMethods;

void HTTP_SEND_RESPONSE_TO_SOCKET(unsigned long socket_handle, HTTPResponse response) {
    char resptext[BUFFER_LEN] = "";

    sprintf(resptext, "HTTP/1.1 %i %s\r\n", response.StatusCode, response.StatusText);
    
    int i = 0;
    while(response.Headers[i][0] != NULL) {
        sprintf(resptext + strlen(resptext),"%s: %s\r\n", response.Headers[i][0], response.Headers[i][1]);
        i++;
    }

    strcat(resptext, "\r\n");
    strcat(resptext, response.Body);

    send(socket_handle, resptext, strlen(resptext), 0);
    closesocket(socket_handle);
}

HTTPRequest HTTP_READ_REQUEST_FROM_SOCKET(unsigned long socket_handle) {
    HTTPRequest req;

    char buffer[BUFFER_LEN] = "";
    recv(socket_handle, buffer, BUFFER_LEN, 0);

    char* current_ptr = buffer;

    strcpy(req.Method, "");
    strcpy(req.Path, "");
    strcpy(req.Body, "");

    char* method_ptr = req.Method;
    char* path_ptr = req.Path;

    // METODO (GET/POST)
    while(*current_ptr != ' ') *method_ptr++ = *current_ptr++;
    *method_ptr = 0;

    current_ptr++;

    // PERCORSO RISORSA RICHIESTA
    while(*current_ptr != ' ') *path_ptr++ = *current_ptr++;
    *path_ptr = 0;

    // BODY RICHIESTA
    while(!(current_ptr[0] == '\r' && current_ptr[1] == '\n' && current_ptr[2] == '\r' && current_ptr[3] == '\n'))
        current_ptr++;

    strcpy(req.Body, current_ptr + 4);

    return req;
}

HTTPMethods HTTP = {
    &HTTP_READ_REQUEST_FROM_SOCKET,
    &HTTP_SEND_RESPONSE_TO_SOCKET
};