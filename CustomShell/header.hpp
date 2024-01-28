#ifndef HEADER
#define HEADER

#include <iostream>
#include <iomanip>
#include <map>
#include <string>
#include <sstream>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/resource.h>
using namespace std;

#define MAX_ARGC 8

struct Process {
    int pid;
    char status;
    string command;
};

class PCB_table {
    // used to have more attributes and functions
    // dont want to bother changing data types now
    public:
        map<int, Process> processes;

};

string joinStrings(int size, std::string arr[]);
int str_to_num(std::string str);
string elapsedTime(int pid);
void printUsage(void);
void updateBGProcess(int signum);
int createPCBEntry(int pid, int& argc, std::string argv[]);
void quit(void);
void jobs(void);
void kill(std::string pid);
void resume(std::string pid);
void suspend(std::string pid);
int wait(std::string pid);
int createPCBEntry(int pid, int& argc, std::string argv[]);
int newProcess(int& argc, std::string argv[]);
int commandVector(int& argc, std::string argv[]);
void prompt(int& argc, std::string argv[]);

#endif