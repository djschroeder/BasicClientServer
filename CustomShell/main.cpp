#include "header.hpp"

PCB_table PCB;

int commandVector(int& argc, string argv[]) {
    string command = argv[0];
    if (command == "exit") {
        quit();
        return 0;
    } else if (command == "jobs") {
        jobs();
    /* } else if (command == "sleep") {
        sleep(argv[1]); */
    } else if (command == "kill") {
        kill(argv[1]);
    } else if (command == "resume") {
        resume(argv[1]);
    } else if (command == "suspend") {
        suspend(argv[1]);
    } else if (command == "wait") {
        wait(argv[1]);
    } else {
        newProcess(argc, argv);
    }
    return 1;
}

void prompt(int& argc, string argv[]) {
    string input;
    string word;
    argc = 0;
    
    do {
    // bugfix to prevent infinite loop when bg process finishes
        cout << "\033[1mSHELL379\033[0m: ";
        if (getline(cin, input)) {
            break;
        }
        cin.clear();
    } while (cin.good() && !cin.eof());

    istringstream iss(input);
    while (iss >> word && argc < MAX_ARGC) {
        argv[argc] = word;
        argc++;
    }
}

int main(void) {
    int argc;
    string argv[MAX_ARGC];
    int iter = 1;
    
    struct sigaction sa;
    sa.sa_flags = 0;
    sa.sa_handler = updateBGProcess;
    sigemptyset(&sa.sa_mask);
    sigaction(SIGCHLD, &sa, NULL);

    while(iter) {
        prompt(argc, argv);
        iter = commandVector(argc, argv);
    }

    return 0;
}