# **MiniShell**

## **Overview**
MiniShell is a command-line interface (CLI) shell designed for educational purposes. It offers a minimalistic set of shell functionalities with a simple interface to understand the basics of systems programming, process management, and inter-process communication.

## **Features**
- Execution of a predefined set of commands (`exit`, `jobs`, `kill`, `resume`, `sleep`, `suspend`, `wait`).
- Ability to run commands in the background.
- Input and output redirection support.
- Process Table management for tracking running and completed processes.

## **Building and Running**
- Use `make` to compile the shell with provided `Makefile`.
- Run with `./shell379` after compilation.

## **Usage**
- Execute commands directly or with arguments.
- Manage processes and view their status using shell commands.
- Redirect input and output using `<` and `>` followed by file names.

## **Implementation Notes**
- Implemented in C.
- Utilizes system calls like `exec`, `fork`, `getrusage`, and `kill`.
- Adheres to specific constants for line lengths, argument counts, and process table entries.

## **Disclaimer**
The shell was built following an assignment specification and is not intended for production use.

## **Acknowledgements**
Developed as part of a CMPUT379 assignment.
