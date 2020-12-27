#include <sys/types.h> 
#include <string.h> 
#include <sys/socket.h> 
#include <netinet/in.h> 
#include <stdlib.h> 
#include <stdio.h> 
#include <ctype.h> 
#include <time.h> 
#include <fcntl.h> 
#include <unistd.h> 
#include <signal.h> 
#include <arpa/inet.h> 
#define LBUF 40 
#define LBUF2 20 
#define LEN sizeof(struct sockaddr_in) 
#define LOGFILE "server.log" 
#define SENDFILE "server.send" 
 
char buffer[LBUF]; 
char buffer2[LBUF2]; 
char timemsg[10]; 
char filebuffer[100]; 
int nsock; 
 
void s_action(int sig) 
{ 
    close(nsock); 
    printf("SERVER: got SIGPIPE.\n"); 
} 
 
int main(int argc, char *argv[]) 
{ 
    struct sockaddr_in client, server; 
    int n, sock, s_len, fil, fil2; 
    int c_len = LEN; 
    time_t t; 
    struct tm *timeinfo; 
    static struct sigaction sact; 
    sact.sa_handler = s_action; 
    sigfillset(&(sact.sa_mask)); 
    sigaction(SIGPIPE, &sact, NULL); 
 
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) 
    { 
        printf("SERVER: unable to create socket.\n"); 
        exit(1); 
    } 
 
    bzero(&server, LEN); 
    server.sin_family = AF_INET; 
    server.sin_port = 1305; 
    server.sin_addr.s_addr = INADDR_ANY; 
    int optval = 1; 
 
    setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval)); 
    if ((bind(sock, (struct sockaddr *) &server, LEN)) < 0) 
    { 
        printf("SERVER: unable to bind socket.\n"); 
        exit(1); 
    } 
    if ((fil = open(LOGFILE, O_RDWR | O_CREAT | O_TRUNC, 0660)) < 0) 
    { 
        printf("SERVER: unable to init file.\n"); 
        exit(1); 
    } 
    close(fil); 
    if ((listen(sock, 5)) < 0) 
    { 
        printf("SERVER: unable to create queue.\n"); 
        exit(1); 
    } 
    for (;;) 
    { 
        if ((nsock = accept(sock, (struct sockaddr *) &client, &c_len)) < 0) 
        { 
            printf("SERVER: unable to pull from queue.\n"); 
            continue; 
        } 
        if (fork() == 0) 
        { 
            memset(buffer, 0, LBUF); 
            n = read(nsock, buffer, LBUF); 
            if (n < 0) 
            { 
                    printf("SERVER: recieving error.\n"); 
                    exit(1); 
            } 
            time(&t); 
            timeinfo = localtime(&t); 
            strftime(timemsg, 10, "%T", timeinfo); 
            sprintf(filebuffer, "From %s:%d at %s: %s\n", inet_ntoa(client.sin_addr), client.sin_port, timemsg, buffer); 
            if ((fil = open(LOGFILE, O_RDWR | O_APPEND, 0660)) < 0) 
            { 
                printf("SERVER: unable to open file again.\n"); 
                exit(1); 
            } 
            write(fil, filebuffer, strlen(filebuffer)); 
            close(fil); 
            if ((fil2 = open(SENDFILE, O_RDONLY, 0660)) < 0) 
            { 
                printf("SERVER: unable to open .send file"); 
                exit(1); 
            } 
            while((n = read(fil2, buffer2, LBUF2)) > 0) 
            { 
                if ((write(nsock, buffer2, n)) != n) 
                { 
                    printf("SERVER: unable to send.\n"); 
                    exit(1); 
                } 
            } 
            close(fil2); 
            close(nsock); 
            exit(0); 
        } 
        close(nsock); 
    } 
} 
