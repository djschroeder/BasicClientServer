#include "prodcon.h"

// Global Variables Initialization
int jobs = 0;  // Count of current jobs; protected by work_mutex
int finished = 0;  // Indicator for when processing is done
pthread_mutex_t log_mutex = PTHREAD_MUTEX_INITIALIZER;  // Mutex for writing to the log
pthread_mutex_t work_mutex = PTHREAD_MUTEX_INITIALIZER;  // Mutex for accessing job data
pthread_cond_t add_work = PTHREAD_COND_INITIALIZER;  // Condition variable for indicating new work availability

// Summary Variables
int Work = 0;
int Ask = 0;
int Receive = 0;
int Complete = 0;
int Sleeps = 0;
int *transactionCounts;

void producer(int max_jobs) {
    // Reads jobs from standard input and adds them to the shared work queue for consumers.
    char input[100];
    char *str = input + 1;
    
    // Continuously read lines from stdin.
    while (fgets(input, sizeof(input), stdin)) {
        int number = atoi(str);

        // If the input line starts with 'T', it represents a job.
        if (input[0] == 'T') {
            pthread_mutex_lock(&work_mutex);

                // Wait if the work queue is full.
                while(jobs == max_jobs) {
                    pthread_cond_wait(&add_work, &work_mutex);
                }

                // Add the new job to the work queue.
                work[jobs] = number;
                jobs++;
                Work++;
                Log(0, jobs, "Work", number);

                // Signal that new work has been added.
                pthread_cond_broadcast(&add_work);
            pthread_mutex_unlock(&work_mutex);
        }

        // If the input line starts with 'S', the producer should sleep.
        else if (input[0] == 'S') {
            Sleeps++;
            Log(0, -1, "Sleep", number);
            Sleep(number * 10000000);
        }
    }

    // Signal that the producer is done producing.
    Log(0, -1, "End", 0);
    finished = 1;

    // Wake up any waiting consumers.
    pthread_cond_broadcast(&add_work);
}

void *consumer(void *arg) {
    // Consumer thread function.
    // Continuously tries to fetch and process jobs from the shared work queue.
    int threadID = *((int*)arg);
    int job;

    // Loop until producer signals no more work
    while(1) {
        Ask++;
        Log(threadID, -1, "Ask", 0);
        pthread_mutex_lock(&work_mutex);

            // Wait if there are no jobs.
            while(jobs == 0 && !finished) {
                    pthread_cond_wait(&add_work, &work_mutex);
                }
            
            // If the producer is finished and there are no more jobs, exit.
            if (jobs == 0 && finished) {
                pthread_mutex_unlock(&work_mutex);
                pthread_cond_broadcast(&add_work);
                return NULL;
            }

            // Fetch a job from the end of the work queue.
            jobs--;
            job = work[jobs];

            // Signal producer that a job has been removed and they might add more.
            pthread_cond_signal(&add_work);
        pthread_mutex_unlock(&work_mutex);
        
        Receive++;
        Log(threadID, jobs, "Receive", job);

        // Process the fetched job.
        Trans(job);
        transactionCounts[threadID - 1]++;  // Subtract 1 because thread IDs start from 1
        Complete++;
        Log(threadID, -1, "Complete", job);
        
    }
}

int main(int argc, char *argv[]) {
    // Main function: Initializes resources, spawns consumer threads, and runs the producer.

    gettimeofday(&start_time, NULL); // Capture the start time of the program for logging purposes.

    // Check for sufficient command-line arguments.
    if (argc < 2) { 
        fprintf(stderr, "Usage: %s nthreads [id]", argv[0]);
        exit(1);
    }

    if(createLog(argc, argv) != 0) { // Generate log file.
        perror("Failed to open file for writing");
        exit(1);
    } 
    int nthreads = atoi(argv[1]); // Parse the number of consumer threads to create.
    transactionCounts = calloc(nthreads, sizeof(int)); // Used to store total transactions completed by each thread.
    work = calloc(nthreads * 2, sizeof(int)); // Initialize work queue. 
    // Work queue implemented as a LIFO stack instead of a true queue to the sake of simplicity

    // Create and start the consumer threads.
    pthread_t threads[nthreads];
    int ids[nthreads];
    for (int i = 0; i < nthreads; i++) {
        ids[i] = i + 1; // +1 to start from ID=1 instead of ID=0, which is used by the producer
        pthread_create(&threads[i], NULL, consumer, (void *) &ids[i]);
    }
    producer(nthreads * 2); // Start the producer function with the maximum size of the work queue.

    // Wait for all consumer threads to complete.
    for (int i = 0; i < nthreads; i++) {
        pthread_join(threads[i], NULL);
    }

    // Write summary stats.
    logSummary(nthreads);

    // Clean up resources and exit.
    fclose(logFile);
    pthread_mutex_destroy(&work_mutex);
    pthread_cond_destroy(&add_work);
    free(transactionCounts);
    free(work);
    return 0;
}