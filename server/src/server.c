/*
 ============================================================================
 Name        : server.c
 Author      : Nicolin Damalja, Gianluca Calò
 Version     : 1.0.0
 Copyright   : None
 Description : Esonero TCP Server, C Ansi-style
 ============================================================================
 */

//socket library
#ifdef WIN32
#include <winsock.h>
#else
#define closesocket close
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#endif
//Standard library
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "protocol.h"

/**
 * This function clears the WSAData struct that is used to initialize the socket in Windows
 */
void clearwinsock() {
#ifdef WIN32
    WSACleanup();
#endif
}

//Function that sum two numbers
int add(int op1, int op2) {
    return (float)(op1)+(float)(op2);
}
//Function that make the sub between two numbers
int sub(int op1, int op2){
    return (float)(op1)-(float)(op2);
}
//Function that multiply two numbers
int mul(int op1, int op2){
    return (float)(op1)*(float)(op2);
}
//Function that make the division between two numbers
float division(int op1, int op2){
    return (float)(op1)/(float)(op2);
}
/**Function that split the string in three tokens: operator, number_one and number_two.These last two tokens are
converted in integer values and, then, in according to the operator is called one of the four function above.*/
float computeString(char* s){
    char op[1], number_one[5], number_two[5];
    int first, second;
    char operator;
    float result;
    char *token = strtok(s, " ");
    int i=0;
    while (token != NULL){
        if(i==0){
            strcpy(op,token);
        } else if(i==1){
            strcpy(number_one, token);
        }
        else if (i==2){
            strcpy(number_two, token);
        }
        token = strtok(NULL, " ");
        i++;
    }
    first = atoi(number_one);
    second = atoi(number_two);
    operator = op[0];
    switch (operator){
        case '+':
            result = add(first, second);
            return result;
        case '-':
            result = sub(first, second);
            return result;
        case 'x':
            result = mul(first, second);
            return result;
        case '/':
            if ((first == 0 && second == 0)){
                return NAN;
            } else if (second == 0){
                return INFINITY;
            }
            result = division(first, second);
            return result;
        default:
            return NAN;
    }
}
//Function that read the operator in the String
char readOperator(char* s){
    char* token = strtok(s, " ");
    char op = token[0];
    return op;
}

//Main function
int main(int argc, char **argv) {
	//Initialize WinSock
	#ifdef WIN32
		WSADATA wsa_data;
		int result = WSAStartup(MAKEWORD(2, 2), &wsa_data);
		if (result != NO_ERROR) {
			perror("Error at WSAStartup()\n");
			system("pause");
			return 0;
		}
	#endif
	//Creation of Server Socket
    int socketserver;
    socketserver = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (socketserver < 0) {
        perror("socket creation failed. \n");
        clearwinsock();
        system("pause");
        return -1;
    }
    // Assignment ip address and port number to Server Socket
    struct sockaddr_in localaddress;

    bzero(&localaddress, sizeof(localaddress));
    localaddress.sin_family = AF_INET;

    if (argc > 1 && argc <= MAX_ARGC){
        localaddress.sin_addr.s_addr = inet_addr(argv[1]);
        localaddress.sin_port = htons(atoi(argv[2]));

    } else {
        localaddress.sin_addr.s_addr = inet_addr(IP_ADDRESS);
        localaddress.sin_port = htons(PORT);
    }

    /**
     	 Converts values between the host and
     	 network byte order. Specifically, htons () converts 16 bit quantities
     	 from host byte order to network byte order .
     **/
    //Binding a unique local name to the socket with descriptor socket
    if (bind(socketserver, (struct sockaddr *) &localaddress, sizeof(localaddress)) < 0) {
        perror("bind() failed.\n");
        clearwinsock();
		#ifdef WIN32
        system("pause");
		#endif
        return -1;
    }

//Setting of the listening socket
    if (listen(socketserver, QLEN) < 0) {
        perror("listen() failed.\n");
        closesocket(socketserver);
        clearwinsock();
        system("pause");
        return -1;
    }

    struct sockaddr_in clientaddress; //client address struct
    int socketclient; //describer of client socket
    int clientsize; //size in byte of client address struct
    printf("Waiting for a client to connect...\n");
    //true while loop until is not established the connection with client
    while(1) {
        clientsize = sizeof(clientaddress); //set the size of the client address
        //Accepting a new connection
        if ((socketclient = accept(socketserver, (struct sockaddr *) &clientaddress, &clientsize)) < 0) {
            perror("accept() failed.\n");
            //Closing the connection
            closesocket(socketclient);
            clearwinsock();
            system("pause");
            return 0;
        }
        printf("Connection established with %s:%u \n", inet_ntoa(clientaddress.sin_addr),
               ntohs(clientaddress.sin_port));
        while (1) {
            char message[BUFFERSIZE];
            //Stream of data received from a Client
            if (recv(socketclient, message, BUFFERSIZE, 0) <= 0) {
                perror("recv() failed or connection with client closed prematurely");
                closesocket(socketclient);
                //clearwinsock();
                break;
            }
            char message2[BUFFERSIZE];
            strcpy(message2, message);
            char op = readOperator(message2);
            if (op != '=') {
                float res = computeString(message);
                char message_to_send[BUFFERSIZE];
                if (res == INFINITY){
                    sprintf(message_to_send, "%s", "INF");
                } else if (isnan(res)) {
                    strcpy(message_to_send, "NaN");
                }else{
                    sprintf(message_to_send, "%.2f", res);
                }
                //Stream of data sent by Server to a Client
                if (send(socketclient, message_to_send, BUFFERSIZE, 0) != BUFFERSIZE) {
                    perror("send() sent a different number of bytes than expected");
                    closesocket(socketclient);
                    //clearwinsock();
                    break;
                }
            } else {
                perror("The operator sent by client is = , so the connection with it will be closed.\n");
                closesocket(socketclient);
                break;
            }

        }
    }//End while loop
//END MAIN
    clearwinsock();
    closesocket(socketserver);
    system("pause");
    return 0;
}

