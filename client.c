/*
 * client.c -- a stream socket client demo
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define KNRM  "\x1B[0m"
#define KRED  "\x1B[31m"
#define KGRN  "\x1B[32m"
#define KYEL  "\x1B[33m"
#define KBLU  "\x1B[34m"
#define KMAG  "\x1B[35m"
#define KCYN  "\x1B[36m"
#define KWHT  "\x1B[37m"

#define PORT "3490" // the port client will be connecting to

#define MAXDATASIZE 100 // max nubmer of bytes we can get at once

// function prototypes
void *get_in_addr(const struct sockaddr *sa);


int main(int argc, char *argv[])
{
    if (argc != 2) {
        fprintf(stderr, "usage: client hostname\n");
        return 1;
    }


    struct addrinfo hints;
    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;

    struct addrinfo *serverinfo;
    int rv = getaddrinfo(argv[1], PORT, &hints, &serverinfo);
    if (rv != 0) {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
        return 2;
    }


    int sockfd;
    struct addrinfo *p;
    for (p = serverinfo; p != NULL; p = p->ai_next) {
        sockfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol);
        if (sockfd == -1) {
            perror("client: socket");
            continue;
        }

        rv = connect(sockfd, p->ai_addr, p->ai_addrlen);
        if (rv == -1) {
            close(sockfd);
            perror("client: connect");
            continue;
        }

        break;
    }

    if (p == NULL) {
        fprintf(stderr, "client: failed to connect\n");
        return 3;
    }
    

    void *src_addr = get_in_addr((struct sockaddr *)p->ai_addr);
    char addr_str[INET6_ADDRSTRLEN];
    inet_ntop(p->ai_family, src_addr, addr_str, sizeof addr_str);
    printf("%sclient:%s connecting to %s\n", KMAG, KNRM, addr_str);

    freeaddrinfo(serverinfo);


    char buf[MAXDATASIZE];
    int numbytes = recv(sockfd, buf, MAXDATASIZE - 1, 0);
    if (numbytes == -1) {
        perror("recv");
        return 4;
    }
    buf[numbytes] = '\0';

    printf("%sclient:%s received:\n%s\n", KMAG, KNRM, buf);

    close(sockfd);

    return 0;
}


/* Get sockaddr, IPv4 or IPv6 */
void *get_in_addr(const struct sockaddr *sa)
{
    if (sa->sa_family == AF_INET) {
        return &(((struct sockaddr_in *)sa)->sin_addr);
    }
    return &(((struct sockaddr_in6 *)sa)->sin6_addr);
}
