#include <sys/types.h> 
#include <sys/socket.h> 
#include <netinet/in.h> 
#include <stdlib.h> 
#include <stdio.h> 
#include <ctype.h> 
#include <string.h> 
#define LEN sizeof(struct sockaddr_in) 
#define LBUF 70 
char buffer[LBUF]; 
char *msg = "message"; 
int main(int argc, char *argv[]) 
{ 
    struct sockaddr_in client, server; 
    int n, sock, s_len, c_len; 
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) 
    { 
        printf("CLIENT: unable to create socket.\n"); 
        exit(1); 
    } 
    server.sin_family = AF_INET; 
    server.sin_addr.s_addr = inet_addr(argv[1]); 
    server.sin_port = atoi(argv[2]); 
    if ((connect(sock, (struct sockaddr *) &server, LEN)) < 0) 
    { 
        printf("CLIENT: unable to connect.\n"); 
        exit(1); 
    } 
    if ((write(sock, msg, strlen(msg))) != strlen(msg)) 
    { 
        printf("CLIENT: unable to send.\n"); 
        exit(1); 
    } 
    while((n = read(sock, buffer, LBUF)) > 0) 
    { 
        printf(buffer); 
        memset(buffer, 0, LBUF); 
    } 
    if (n < 0) 
    { 
        printf("CLIENT: unable to recieve.\n"); 
        exit(1); 
    } 
    close(sock); 
    exit(0); 
}
