#include <stdio.h>
#include <netdb.h>
#include <string.h>
#include <unistd.h>

int main() {
    struct addrinfo address_info = {
        .ai_flags = AI_PASSIVE,
        .ai_family = AF_INET,
        .ai_socktype = SOCK_STREAM,
        .ai_protocol = 0,
    };
    struct addrinfo *res = NULL;

    int result = getaddrinfo(NULL, "8080", &address_info, &res);
    if (result != 0) {
        const char *error = gai_strerror(result);
        fprintf(stderr, "ERROR: %s \n", error);
        return -1;
    }

    // getting socket file descriptor
    struct addrinfo *curr = res;
    struct addrinfo *next = res->ai_next;
    int sock_fd; 
    
    while (next != NULL) {
        sock_fd = socket(curr->ai_family, curr->ai_socktype, curr->ai_protocol);
       
        if (sock_fd != -1) {
            if (bind(sock_fd, curr->ai_addr, curr->ai_addrlen) != -1) break; 
            close(sock_fd); 
        } else {
            curr = next;
            next = next->ai_next;
        }
    }

    freeaddrinfo(res);

    // TODO: Check node pointers curr and next
}
