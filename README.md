CMPUT 379 - Assignment 3: Client-Server / Monitoring
Introduction

This repository contains the implementation of a client-server architecture for CMPUT 379's Assignment 3. It simulates a simple client-server model where clients send transactions to the server for processing, showcasing basic networking and multi-threading concepts.
Features

    Client-Server Architecture: Implementing a basic communication model between clients and a server.
    Transaction Processing: Clients generate and send transactions (T<n>) to the server.
    Multi-Client Handling: Server processes transactions and manages multiple client connections simultaneously.
    Command-Line Interface: Both server and client are operated via command-line arguments.

Requirements

    Linux-based Operating System
    GCC Compiler
    Standard C Libraries

Installation and Running the Program

    Clone the Repository:

    bash

git clone [Repository URL]

Compile the Programs:

gcc -o server server.c
gcc -o client client.c

Run the Server:

bash

./server [port]

Run the Client(s):

css

    ./client [port] [server-ip-address]

Usage

Clients can send two types of commands to the server:

    Transaction Request (T<n>): Requests the server to process a transaction.
    Status Request (S<n>): Requests the server's current status.

The server processes these commands and returns appropriate responses.
Sample Output

Expect the server to log transactions and client connections. Clients will receive acknowledgments of processed requests.
Testing and Limitations

    Extensively tested in a Linux environment.
    Known limitation: Does not support concurrent processing of multiple transactions from the same client.

Contributions

    David Schroeder

License

Apache 2.0

Acknowledgments

    Special thanks to CMPUT 379 course staff and fellow students.
