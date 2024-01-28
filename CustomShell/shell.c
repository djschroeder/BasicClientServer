#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <ctype.h>
#include <sys/types.h>
#include <signal.h>
#include <sys/resource.h>
#include <sys/wait.h>

#define LINE_LENGTH 100
#define MAX_ARGS 7
#define MAX_LENGTH 20
#define MAX_PT_ENTRIES 32

typedef struct {
    pid_t pid;
    char status;
    char command[LINE_LENGTH];
} ProcessEntry;

ProcessEntry processTable[MAX_PT_ENTRIES];
int nextFreeIndex = 0;


int addToProcessTable(pid_t pid, char* command) {
    // Add process to the process table
    if (nextFreeIndex < MAX_PT_ENTRIES) {
        processTable[nextFreeIndex].pid = pid;
        strcpy(processTable[nextFreeIndex].command, command);
        processTable[nextFreeIndex].status = 'R';
        nextFreeIndex++;
        return nextFreeIndex - 1;
    }
    return -1; // Return an index where the process was added or an error code
}

void removeFromProcessTable(pid_t pid) {
    for (int i = 0; i < nextFreeIndex; i++) {
        if (processTable[i].pid == pid) {
            // Shift all entries to fill the gap
            for (int j = i; j < nextFreeIndex - 1; j++)
                processTable[j] = processTable[j + 1];
            nextFreeIndex--;
            return;
        }
    }
}

void updateProcessStatus(pid_t pid, char status) {
    for (int i = 0; i < nextFreeIndex; i++) {
        if (processTable[i].pid == pid) {
            processTable[i].status = status;
            return;
        }
    }
}

void backgroundCheck(char* args[], int* bg_flag) {
    int lastArgIndex = 0;
    while(args[lastArgIndex] != NULL) lastArgIndex++;

    if (lastArgIndex > 0 && strcmp(args[lastArgIndex - 1], "&") == 0) {
        *bg_flag = 1;
        args[lastArgIndex - 1] = NULL;  // Remove '&' from arguments passed to exec
    }
}

void handleIOredirection(char* args[], int* inIndex, int* outIndex) {
    *inIndex = -1;
    *outIndex = -1;

    for (int i = 0; args[i] != NULL; i++) {
        if (args[i][0] == '<') {
            *inIndex = i;
        } else if (args[i][0] == '>') {
            *outIndex = i;
        }
    }

    // Optional: Check if filenames are provided for the redirections.
    if (*inIndex != -1 && strlen(args[*inIndex]) < 2) {
        printf("Error: No input file specified for redirection.\n");
        *inIndex = -1;
    }

    if (*outIndex != -1 && strlen(args[*outIndex]) < 2) {
        printf("Error: No output file specified for redirection.\n");
        *outIndex = -1;
    }
}

void printUsage(void) {
    struct rusage usage;
    if (getrusage(RUSAGE_CHILDREN, &usage) == 0) {
        printf("User time = %ld seconds\n", usage.ru_utime.tv_sec);
        printf("Sys  time = %ld seconds\n", usage.ru_stime.tv_sec);
    } else {
        perror("getrusage");
    }
}

char* elapsedTime(int pid) {
    static char result[30];
    char command[50];

    sprintf(command, "ps -o etimes= -p %d", pid);
    FILE* pipe = popen(command, "r");
    if (!pipe) {
        perror("popen");
        strcpy(result, "pipe failure");
        return result;
    }
    if (fgets(result, sizeof(result), pipe)) {
        char *newline = strchr(result, '\n'); // find newline
        if (newline) *newline = '\0'; // replace newline with null terminator
    }
    pclose(pipe);

    return result;
}

void handleJobs() {
    int active_processes = 0;

    printf("Running processes:\n");
    for (int i = 0; i < nextFreeIndex; i++) {
        printf("# %d: %d %c %s %s seconds\n", i, processTable[i].pid, processTable[i].status, processTable[i].command, elapsedTime(processTable[i].pid));
        active_processes++;
    }
    printf("Processes = %d active\n", active_processes);
    printf("Completed processes:\n");
    printUsage();
}

void handleExit() {
    for (int i = 0; i < nextFreeIndex; i++) {
        waitpid(processTable[i].pid, NULL, 0);
    }
    printf("Completed processes:\n");
    printUsage();
    exit(0);
}

void handleKill(pid_t pid) {
    kill(pid, SIGTERM); // Sends the termination signal
    removeFromProcessTable(pid); // Assumes the process will be terminated
}

void handleResume(pid_t pid) {
    kill(pid, SIGCONT); // Sends the continue signal
    updateProcessStatus(pid, 'R');
}

void handleSuspend(pid_t pid) {
    kill(pid, SIGSTOP); // Sends the stop signal
    updateProcessStatus(pid, 'S');
}

void handleWait(pid_t pid) {
    waitpid(pid, NULL, 0);
    removeFromProcessTable(pid);
}

void runCommand(char* cmd, char* args[], char* input) {
    pid_t pid;
    int inIndex, outIndex;
    int background = 0; // Flag to check if the process should run in the background

    backgroundCheck(args, &background);
    handleIOredirection(args, &inIndex, &outIndex);

    if ((pid = fork()) == 0) { // Child process
        // I/O redirection, if specified
        if (inIndex != -1) {
            freopen(args[inIndex] + 1, "r", stdin);
        }
        if (outIndex != -1) {
            freopen(args[outIndex] + 1, "w", stdout);
        }
        execvp(cmd, args);
        perror("execvp"); // If execvp returns, it's an error.
        exit(EXIT_FAILURE);
    } else if (pid < 0) {
        perror("fork");
    } else { // Parent process
        addToProcessTable(pid, input);
        if (!background) {
            waitpid(pid, NULL, 0);
            removeFromProcessTable(pid);
        }
    }
}

void childHandler(int signo) {
    int status;
    pid_t pid;
    while ((pid = waitpid(-1, &status, WNOHANG)) > 0) {
        removeFromProcessTable(pid);
    }
}

void parseInput(char *input) {
    // 1. Trim the input, remove any leading/trailing whitespaces or newline characters.
    char* end;
    char input_save[LINE_LENGTH];
    strcpy(input_save, input);
    // Trim leading space
    while(isspace((unsigned char)*input)) input++;
    // Trim trailing space
    end = input + strlen(input) - 1;
    while(end > input && isspace((unsigned char)*end)) end--;
    *(end+1) = 0;

    // 2. Tokenize the input based on spaces to split the command and its arguments.
    char* tokens[MAX_ARGS];
    int i = 0;
    tokens[i] = strtok(input, " ");
    while(tokens[i] != NULL) {
        i++;
        tokens[i] = strtok(NULL, " ");
    }

    // 3. Determine which command it is (e.g., 'exit', 'jobs', 'kill', etc.)
    if(tokens[0] == NULL) {
        return; // empty command
    } else if(strcmp(tokens[0], "exit") == 0) {
        handleExit();
    } else if(strcmp(tokens[0], "jobs") == 0) {
        handleJobs();
    } else if(strcmp(tokens[0], "kill") == 0 && tokens[1] != NULL) {
        handleKill(atoi(tokens[1]));
    } else if(strcmp(tokens[0], "resume") == 0 && tokens[1] != NULL) {
        handleResume(atoi(tokens[1]));
    } else if(strcmp(tokens[0], "suspend") == 0 && tokens[1] != NULL) {
        handleSuspend(atoi(tokens[1]));
    } else if(strcmp(tokens[0], "wait") == 0 && tokens[1] != NULL) {
        handleWait(atoi(tokens[1]));
    } else {
        runCommand(tokens[0], tokens, input_save); 
    }
}

int main() {
    // child Signal Handler
    struct sigaction sa;
    sa.sa_handler = childHandler;
    sa.sa_flags = SA_RESTART | SA_NOCLDSTOP;
    if (sigaction(SIGCHLD, &sa, 0) == -1) {
        perror("sigaction");
        exit(1);
    }

    char input[LINE_LENGTH];
    while(1) {
        printf("SHELL379: ");
        fflush(stdout);
        fgets(input, LINE_LENGTH, stdin);
        parseInput(input);
    }
    return 0;
}