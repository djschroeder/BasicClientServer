#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <netdb.h>
#include <time.h>
#include "tands.h"

// Function to print output including the client's address
int printOutput(int task, char job[], struct sockaddr_in clt_addr) {
    struct timespec current_time;
    clock_gettime(CLOCK_REALTIME, &current_time);
    char client[NI_MAXHOST];

    // Get the client's host name
    if (getnameinfo((struct sockaddr *)&clt_addr, sizeof(clt_addr), client, sizeof(client), NULL, 0, NI_NAMEREQD)) {
        perror("\ngetnameinfo error\n");
        return -1;
    }

    // Print the task information with the client's address
    printf("%ld.%02ld: # %2d (%4s) from %s\n", current_time.tv_sec, current_time.tv_nsec / 10000000, task, job, client);
    return 0;
}

int main(int argc, char* argv[]) {
    // Check if the correct number of arguments is provided
    if (argc != 2) {
        printf("\n Usage: %s <port> \n", argv[0]);
        exit(-1);
    }

    // Create a socket
    int listenfd;
    if ((listenfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("\nsocket\n");
        exit(-1);
    }

    // Setup the server's address
    struct sockaddr_in serv_addr;
    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = htonl(INADDR_ANY); // Listen on any interface
    int port = atoi(argv[1]);
    if (port <= 0 || port > 65535) {
        printf("\nInvalid port number.\n");
        exit(-1);
    }
    serv_addr.sin_port = htons(port); // Convert port to network byte order

    // Bind the socket to the address
    if (bind(listenfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
        perror("\nbind error\n");
        exit(-1);
    }

    // Listen for incoming connections
    if (listen(listenfd, 10) < 0) {
        perror("\n listen error \n");
        exit(-1);
    }

    int task = 0; // Task counter
    printf("Using port %d\n", port);

    // Server's main loop to accept and process requests
    while (1) {
        struct sockaddr_in clnt_addr;
        socklen_t clnt_len = sizeof(clnt_addr);

        // Accept an incoming connection
        int connfd = accept(listenfd, (struct sockaddr *)&clnt_addr, &clnt_len);
        if (connfd < 0) {
            perror("\n connection accept error \n");
            continue; // Continue to the next iteration if there's an error
        }

        char readBuff[1024]; // Buffer to read the client's request
        memset(readBuff, 0, sizeof(readBuff));
        int bytesRead = read(connfd, readBuff, sizeof(readBuff) - 1);
        if (bytesRead <= 0) {
            perror("\n read error \n");
            close(connfd);
            continue; // Continue to the next iteration if there's an error
        }

        readBuff[bytesRead] = '\0'; // Null-terminate the received string
        char job[10]; // Buffer to store the job descriptor
        snprintf(job, sizeof(job), "T%3s", readBuff); // Format the job descriptor
        printOutput(++task, job, clnt_addr); // Print job start information

        Trans(atoi(readBuff)); // Perform the simulated computation

        printOutput(task, "Done", clnt_addr); // Print job completion information

        char sendBuff[10]; // Buffer to send the response back to the client
        snprintf(sendBuff, sizeof(sendBuff), "%d", task); // Format the response
        write(connfd, sendBuff, strlen(sendBuff)); // Send the response

        close(connfd); // Close the connection
    }
    return 0;
}
