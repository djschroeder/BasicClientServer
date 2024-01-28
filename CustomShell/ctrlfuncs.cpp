#include "header.hpp"

extern PCB_table PCB;

void quit(void) {
    /* End the execution of shell379. Wait until all processes initiated by the
    shell are complete. Print out the total user and system time for all
    processes run by the shell. */
    while (!PCB.processes.empty()) {
    }
    cout << "Resources used" << endl;
    printUsage();
}

void jobs(void) {
    /*  Display the status of all running processes spawned by shell379. See
    the print format below in the example. */
    int processNum = 0;

    cout << "Running processes:" << endl;
    cout << "#     PID S SEC COMMAND" << endl;
    for (const auto& process : PCB.processes) {
        cout << processNum << ": ";
        cout << process.second.pid << " ";
        cout << process.second.status << " ";
        cout << setw(3) << right << elapsedTime(process.second.pid) << " ";
        cout << process.second.command << endl;
        processNum++;
    }
    cout << "Processes =" << setw(7) << right << processNum << " active" << endl;
    cout << "Completed processes:" << endl;
    printUsage();
}

void kill(string pid) {
    /* Kill process <pid> */
    int kill_pid = str_to_num(pid);
    if(PCB.processes.count(kill_pid)) {
        int result = kill(kill_pid, SIGTERM);
        if (result != 0) {
            cout << "kill failed" << endl;
        } else {
            PCB.processes.erase(kill_pid);
            cout << kill_pid << " terminated" << endl;
        }
    } else {
        cout << "invalid pid" << endl;
    }
}

void resume(string pid) {
    /* Resume the execution of process <pid>. This undoes a suspend. */
    int resume_pid = str_to_num(pid);
    if(PCB.processes.count(resume_pid)) {
        int result = kill(resume_pid, SIGCONT);
        if (result != 0) {
            cout << "resume failed" << endl;
        } else {
            PCB.processes[resume_pid].status = 'R';
            cout << resume_pid << " resumed" << endl;
        }
    } else {
        cout << "invalid pid" << endl;
    }
}

void suspend(string pid) {
    /* Suspend execution of process <pid>. A resume will reawaken it. */
    int suspend_pid = str_to_num(pid);
    if(PCB.processes.count(suspend_pid)) {
        int result = kill(suspend_pid, SIGSTOP);
        if (result != 0) {
            cout << "suspend failed" << endl;
        } else {
            PCB.processes[suspend_pid].status = 'S';
            cout << suspend_pid << " suspended" << endl;
        }
    } else {
        cout << "invalid pid" << endl;
    }
}

int wait(string pid) {
    /* Wait until process <pid> has completed execution. */
    int id = str_to_num(pid);
    if(PCB.processes.count(id)) {
        int status;
        waitpid(id, &status, 0);
        PCB.processes.erase(id);
    } else {
        cout << "invalid pid" << endl;
    }
    return 0;
}

int newProcess(int& argc, string argv[]) {
    /* Spawn a process to execute command */
    bool background = false;
    string input_file, output_file;

    if (argc > 0 && argv[argc - 1] == "&") {
        background = true;
        argc--;
    } 
    for (int i = 1; i < argc; i++) {
        if (argv[i].find("<") == 0) {
            input_file = argv[i].substr(1);
            argc -= 1;
            break;
        } else if (argv[i].find(">") == 0) {
            output_file = argv[i].substr(1);
            argc -= 1;
            break;
        }
    }

    int child_pid = fork();
    if (child_pid < 0) {
        cout << "fork failed" << endl; 
        exit(1);
    } else if (child_pid == 0) {
        if (!input_file.empty()) {
            freopen(input_file.c_str(), "r", stdin);
        }
        if (!output_file.empty()) {
            freopen(output_file.c_str(), "w", stdout);
        } 

        const char *c_argv[MAX_ARGC + 1] = {};
        for (int i = 0; i < argc; i++) {
            c_argv[i] = argv[i].c_str();
        }
        if (execvp(c_argv[0], const_cast<char* const*>(c_argv)) == -1) {
            cerr << "unknown command or invalid arguments" << endl;
            exit(1);
        }
    } else {
        createPCBEntry(child_pid, argc, argv);
        if (!background) {
            if(waitpid(child_pid, NULL, 0) > 0);
            PCB.processes.erase(child_pid);
        }
    }
    return 0;
}
