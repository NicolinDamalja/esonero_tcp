#ifndef PROTOCOL_H_
#define PROTOCOL_H_

#define IP_ADDRESS "127.0.0.2"//ip address
#define PORT 60000 //default protocol number
#define QLEN 5 //size of request queue
#define BUFFERSIZE 512 //size of String
#define MAX_ARGC 3 // max arguments

//Function that sum two numbers
int add(int op1, int op2);
//Function that make the sub between two numbers
int sub(int op1, int op2);
//Function that multiply two numbers
int mul(int op1, int op2);
//Function that make the division between two numbers
float division(int op1, int op2);

#endif /* PROTOCOL_H_ */
