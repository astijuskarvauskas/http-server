#include <stdio.h>
#include <netdb.h>
#include <unistd.h>
#include <string.h>

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

    int result;
    if ((result = getaddrinfo(NULL, "8080", &address_info, &res)) == 1) {
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
    if ((result = listen(sock_fd, 1)) == 0) {
        printf("Listening on socket with fd: %d \n", sock_fd);
    } else {
        const char *error = gai_strerror(result);
        fprintf(stderr, "ERROR: %s \n", error);
    }
    
    const size_t BUFFER_SIZE = 10 * 1024;
    char buffer[BUFFER_SIZE];

    // form the response
    const char* response = { 
        "HTTP/1.1 200 OK \r\n"
        "Content-Length: 200 \r\n"
        "Content-Type: text/html \r\n"
        "\r\n"
        "<p> Hello World! </p> \r\n"
    };

    // accepting connection, receiving request, sending response
    while (1) {
        int fd = accept(sock_fd, NULL, NULL);
        recv(fd, buffer, BUFFER_SIZE, 0);
        send(fd, response, strlen(response), 0); 
    }

    freeaddrinfo(res);
}
