i#include "prodcon.h"

// Summary Variables - declared in prodcon.c
extern int Work;
extern int Ask;
extern int Receive;
extern int Complete;
extern int Sleeps;
extern int *transactionCounts;

int createLog(int argc, char *argv[]) {
    // Initialize log file with an optional custom ID if provided.
    int logID = 0;
    if (argc > 2 && atoi(argv[2]) != 0) {
        logID = atoi(argv[2]);
    }
    char filename[15];
    sprintf(filename, "prodcon.%d.log", logID);

    // Open the log file for writing.
    logFile = fopen(filename, "w");
    if (logFile == NULL) {
        return -1;
    }
    return 0;
}

void Log(int id, int job_qty, char* activity, int work_no) {
    // Logs the activity of producers and consumers to a file.

    // Prepares the job quantity string based on the activity type.
    char qty[10] = "    ";
    if (strcmp(activity, "Receive") == 0|| strcmp(activity, "Work") == 0) {
        if (job_qty >= 0) sprintf(qty, "Q= %d", job_qty);
    }

    // Convert work number to a string.
    char snum[10];
    if (work_no > 0) {
        snprintf(snum, sizeof(snum), "%d", work_no);
    }

    // Get the current time and compute the elapsed time since program start.
    struct timeval current_time;
    gettimeofday(&current_time, NULL);
    float elapsed_time = (current_time.tv_sec - start_time.tv_sec) + (current_time.tv_usec - start_time.tv_usec) / 1e6;

    // Format the log line.
    char line[40];
    sprintf(line, "%.3f ID= %d %s %-10s %s\n", elapsed_time, id, qty, activity, work_no > 0 ? snum : " ");

    // Synchronized write to the log file.
    pthread_mutex_lock(&log_mutex);
    fwrite(line, sizeof(char), strlen(line), logFile);
    pthread_mutex_unlock(&log_mutex);
}

void logSummary(int nThreads) {
    // This function and this function only is not my work. 
    // It was mostly written by chatGPT because Im feeling lazy :)
    int totalTransactions = 0;

    // Get the current time and compute the elapsed time since program start.
    struct timeval current_time;
    gettimeofday(&current_time, NULL);
    float duration = (current_time.tv_sec - start_time.tv_sec) + (current_time.tv_usec - start_time.tv_usec) / 1e6;

    // Print the static portion of the summary
    fprintf(logFile, "Summary:\n");
    fprintf(logFile, "    Work     %6d\n", Work);
    fprintf(logFile, "    Ask      %6d\n", Ask);
    fprintf(logFile, "    Receive  %6d\n", Receive);
    fprintf(logFile, "    Complete %6d\n", Complete);
    fprintf(logFile, "    Sleep    %6d\n", Sleeps);

    // Print the number of transactions completed by each thread
    for (int i = 0; i < nThreads; i++) {
        fprintf(logFile, "    Thread %d %6d\n", i + 1, transactionCounts[i]);
        totalTransactions += transactionCounts[i];
    }

    // Calculate and print transactions per second
    float tps = (float)totalTransactions / duration;
    fprintf(logFile, "Transactions per second: %.2f\n", tps);
}
