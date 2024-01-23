#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <time.h>
#include "tands.h"


int main(int argc, char* argv[]) {
    // Check if the correct number of arguments is provided
    if (argc != 3) {
        printf("Usage: %s <ip of server> <port>\n", argv[0]);
        exit(-1);
    }

    // Convert port argument to integer and validate
    int port = atoi(argv[2]);
    if (port <= 0 || port > 65535) {
        printf("Invalid port number.\n");
        exit(-1);
    }

    // Setup the server address
    struct sockaddr_in serv_addr;
    memset(&serv_addr, 0, sizeof(serv_addr)); // Initialize the address structure with zeros
    serv_addr.sin_family = AF_INET; // Set address family to Internet (IPv4)
    serv_addr.sin_port = htons(port); // Convert port number to network byte order

    // Convert server IP address from string to network format
    if (inet_pton(AF_INET, argv[1], &serv_addr.sin_addr) <= 0) {
        printf("inet_pton error\n");
        exit(-1);
    }

    // Create a socket for communication
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        perror("Error: Could not create socket\n");
        exit(-1);
    }

    int transCount = 0; // Counter for transactions
    char recvBuff[10]; // Buffer for receiving data from server

    // Main loop to process input commands
    while (fgets(recvBuff, sizeof(recvBuff), stdin)) {
        recvBuff[strcspn(recvBuff, "\n")] = 0; // Remove newline character from input
        char *number = recvBuff + 1; // Point to the number part in the buffer

        // Handle 'T'ransaction command
        if (recvBuff[0] == 'T') {
            transCount++;
            struct timespec ts;
            clock_gettime(CLOCK_REALTIME, &ts); // Get current time
            printf("%ld.%02ld: Send (T%3s)\n", ts.tv_sec, ts.tv_nsec / 10000000, number); // Print sending message with timestamp

            // Connect to the server
            if (connect(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
                perror("Error: Connect Failed\n");
                continue; // Skip to next iteration on connection failure
            }

            write(sockfd, number, strlen(number)); // Send the data to server

            // Read response from the server
            int bytesread = read(sockfd, recvBuff, sizeof(recvBuff) - 1);
            recvBuff[bytesread] = '\0'; // Null-terminate the received data

            clock_gettime(CLOCK_REALTIME, &ts); // Get current time again
            printf("%ld.%02ld: Recv (D%3s)\n", ts.tv_sec, ts.tv_nsec / 10000000, recvBuff); // Print received message with timestamp

            close(sockfd); // Close the connection

            // Re-create the socket for the next transaction
            sockfd = socket(AF_INET, SOCK_STREAM, 0);
            if (sockfd < 0) {
                perror("Error: Could not re-create socket\n");
                exit(-1);
            }
        } 
        // Handle 'S'leep command
        else if (recvBuff[0] == 'S') {
            printf("Sleep %s units\n", number); // Print sleep message
            Sleep(atoi(number) * 10000000);
        }
    }

    printf("Sent %d transactions\n", transCount); // Print total transactions sent
    close(sockfd); // Close the socket at the end
    return 0;
}


