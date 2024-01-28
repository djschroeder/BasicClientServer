#include <pthread.h>
#include <stdio.h>
#include <time.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/time.h>

// Global Variable Declarations
int *work;
int jobs;
int finished;
struct timeval start_time;
FILE* logFile;
pthread_mutex_t log_mutex;
pthread_mutex_t work_mutex;
pthread_cond_t add_work;

void Trans( int n );
void Sleep( int n );
int createLog(int argc, char *argv[]);
void Log(int id, int job_qty, char* activity, int work_no);
void logSummary(int nThreads);