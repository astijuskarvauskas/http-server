#include <stdio.h>
#include <netdb.h>
#include <unistd.h>

int main() {
    // packing custom 'addinfo' type struct...
    // NOTE: all values not explicity set are zeroed out
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
    int sock_fd; 
    
    while (curr != NULL) {
        sock_fd = socket(curr->ai_family, curr->ai_socktype, curr->ai_protocol);
       
        if (sock_fd != -1) {
            if (bind(sock_fd, curr->ai_addr, curr->ai_addrlen) != -1) break; 
            close(sock_fd); 
        } else {
            curr = curr->ai_next;
        }
    }

    if (curr == NULL) {
        printf("Socket file descriptor not found. Exiting!");
        return -1;
    }
    
    // have socket fd now...
    result = listen(sock_fd, 1);
    if (result == 0) {
        printf("Listening on socket with fd: %d \n", sock_fd);
    } else {
        const char *error = gai_strerror(result);
        fprintf(stderr, "ERROR: %s \n", error);
    }

    socklen_t address_length = res->ai_addrlen;
    
    const size_t BUFFER_SIZE = 10 * 1024;
    char buffer[BUFFER_SIZE];

    while (1) {
        accept(sock_fd, address_info.ai_addr, &address_length);
        recv(sock_fd, buffer, BUFFER_SIZE, MSG_WAITALL);
    }

    freeaddrinfo(res);
}
