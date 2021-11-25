/*
 ============================================================================
 Name        : clienttcp.c
 Author      : Nicolin Damalja, Gianluca Calò
 Version     : 1.0.0
 Copyright   : None
 Description : Esonero TCP Client, C Ansi-style
 ============================================================================
 */

#if defined WIN32
#include <winsock.h>
#else
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <netdb.h>
#define closesocket close
#endif
//...
#include <stdio.h>
#include <stdlib.h>
#include "protocol.h"

/**
 * This function clears the WSAData struct that is used to initialize the socket in Windows
 */
void clearwinsock() {
#if defined WIN32
    WSACleanup();
#endif
}

//Function that read the operator in the String
char readOperator(char* s){
    char* token = strtok(s, " ");
    char op = token[0];
    return op;
}

//Main function
int main(int argc, char* argv[]) {
#if defined WIN32
        // Initialize Winsock
        WSADATA wsa_data;
        int result =
                WSAStartup(MAKEWORD(2, 2), &wsa_data);
        if (result != NO_ERROR) {
            perror("Error at WSAStartup()\n");
            return 0;
        }
#endif

        //creation of the socket
        int c_socket;
        c_socket = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
        if (c_socket < 0) {
            perror("socket creation failed.\n");
            return -1;
        }
        //Assignment ip address and port number to client Socket
        struct sockaddr_in sad;
        sad.sin_family = AF_INET;
        if (argc > 1 && argc < MAX_ARGC){
            sad.sin_addr.s_addr = inet_addr(argv[1]);
            sad.sin_port = htons(atoi(argv[2]));
        }else {
            sad.sin_addr.s_addr = inet_addr(IP_ADDRESS);
            sad.sin_port = htons(PORT);
        }
        //Connection to the Server
        if (connect(c_socket, (struct sockaddr *) &sad, sizeof(sad)) < 0) {
            perror("Failed to connect.\n");
            closesocket(c_socket);
            clearwinsock();
            return -1;
        }
        while(1){
            char input_string[BUFFERSIZE];
            printf("Insert the operator and the number to compute:\n");
            scanf("%[^\n]s", input_string);
            fflush(stdin);
            //Stream of data sent to connected Server
            if (send(c_socket, input_string, strlen(input_string)+1, 0) >= BUFFERSIZE) {
                perror("send() sent a different number of bytes than expected");
                closesocket(c_socket);
                clearwinsock();
                return -1;
            }
            int bytes_rcvd;
            int total_bytes_rcvd = 0;
            char buf[BUFFERSIZE];
            char op = readOperator(input_string);
            printf("Received: ");
            while (total_bytes_rcvd < BUFFERSIZE) {
                //Stream of data received from the connected Server
                if ((bytes_rcvd = recv(c_socket, buf, BUFFERSIZE - 1, 0)) <= 0) {
                    closesocket(c_socket);
                    clearwinsock();
                    if(op=='=') {
                        printf("Connection closed\n");
                        system("pause");
                        return 0;
                    }
                    printf("recv() failed or connection closed prematurely\n");
                    system("pause");
                    return -1;

                }
                total_bytes_rcvd += bytes_rcvd;
                buf[bytes_rcvd] = (char) "\0";
                printf("%s", buf);
            }
            printf("\n");
        }
        //Chiusura della connessione
        closesocket(c_socket);
        clearwinsock();
        printf("\n");
    system("pause");
    return 0;

}
