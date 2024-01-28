#include "header.hpp"

extern PCB_table PCB;

string joinStrings(int size, string arr[]) {
    string result;

    for (int i = 0; i < size; i++) {
        if (i > 0) {
            result += " ";  // Add a space between strings
        }
        result += arr[i];  // Concatenate the current string
    }

    return result;
}

int str_to_num(string str) {
    try {
        return stoi(str);
    } catch (const invalid_argument& e) {
        cerr << "invalid argument: " << e.what() << endl;
        return -1;
    }
}

string elapsedTime(int pid) {
    char command[30];
    snprintf(command, sizeof(command), "ps -o etimes= -p %d", pid);
    
    FILE* pipe = popen(command, "r");
    if (!pipe) {
        perror("popen");
        return "pipe failure";
    }
    char buffer[30];
    string result = "";
    while(!feof(pipe)) {
        if (fgets(buffer, 30, pipe) != nullptr) {
            result += buffer;
        }
    }
    pclose(pipe);

    istringstream iss(result);
    iss >> result;

    return result;
}

void printUsage(void) {
    struct rusage usage;
    if (getrusage(RUSAGE_CHILDREN, &usage) == 0) {
        cout << "User time =" << setw(7) << right << usage.ru_utime.tv_sec << " seconds" << endl;
        cout << "Sys  time =" << setw(7) << right << usage.ru_stime.tv_sec << " seconds" << endl;
    } else {
        perror("getrusage");
    }
}

void updateBGProcess(int signum) {
    int status;
    int pid;
    pid = waitpid(-1, &status, WNOHANG);
    if (pid > 0) {
        PCB.processes.erase(pid);
    }
}

int createPCBEntry(int pid, int& argc, string argv[]) {
    Process process;
    process.pid = pid;
    process.status = 'R';
    process.command = joinStrings(argc, argv);
    PCB.processes[pid] = process;
    
    return 0;
}
