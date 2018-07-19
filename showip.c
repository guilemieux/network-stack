/*
 * showip.c -- show IP addresses for a host given on the command line
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <netinet/in.h>

// function prototypes
void get_all_addresses(const char *hostname, struct addrinfo **res);
void print_all_addresses(struct addrinfo *addresses);
void print_address(const struct addrinfo *address_info);

int main(int argc, char *argv[])
{
    if (argc != 2) {
        fprintf(stderr, "usage: showip hostname\n");
        exit(EXIT_FAILURE);
    }
    char *hostname = argv[1];
    struct addrinfo *addresses;
    get_all_addresses(hostname, &addresses);
    printf("IP addressses for %s: \n", hostname);
    print_all_addresses(addresses);
    freeaddrinfo(addresses); // free the linked list
    return 0;
}

void get_all_addresses(const char *host, struct addrinfo **res)
{
    struct addrinfo hint;
    memset(&hint, 0, sizeof hint);
    hint.ai_family = AF_UNSPEC; // AF_INET or AF_INET6 to force version
    hint.ai_socktype = SOCK_STREAM;
    int status = getaddrinfo(host, NULL, &hint, res);
    if (status != 0) {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(status));
        exit(EXIT_FAILURE);
    }
}

void print_all_addresses(struct addrinfo *addresses)
{
    struct addrinfo *addrinfo_ptr;
    for (addrinfo_ptr = addresses; addrinfo_ptr != NULL; addrinfo_ptr = addrinfo_ptr->ai_next) {
        print_address(addrinfo_ptr);
    }
}

void print_address(const struct addrinfo *address_info)
{
    void *addr;
    char *ip_version;
    if (address_info->ai_family == AF_INET) {
        struct sockaddr_in *ipv4 =
                (struct sockaddr_in *) address_info->ai_addr;
        addr = &(ipv4->sin_addr);
        ip_version = "IPv4";
    } else {
        struct sockaddr_in6 *ipv6 =
                (struct sockaddr_in6 *) address_info->ai_addr;
        addr = &(ipv6->sin6_addr);
        ip_version = "IPv6";
    }
    char ip_str[INET6_ADDRSTRLEN];
    inet_ntop(address_info->ai_family, addr, ip_str, sizeof ip_str);
    printf(" - %s: %s\n", ip_version, ip_str);
}
