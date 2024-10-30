#include <stdio.h> 
#include <pthread.h> 
#include <unistd.h> 
#include <stdlib.h> 
#include <signal.h> 
#include <semaphore.h>
#include <sys/time.h> 
#include <stdbool.h> 
#include <string.h> 
#include <sys/wait.h> 

#define BUFFER 100 // Size for user input buffer
#define MAX_PROCESSES 100 // Max number of processes we can track
#define OUTPUT_SIZE 1000 // Output buffer size

typedef enum {READY, RUNNING, WAITING} PROCESS_STATE; // Process states

typedef struct {
    char *process; // Name of the process
    int pid; // Process ID
    struct timeval start_time; // Start time of the process
    struct timeval end_time; // End time of the process
    long waittime; // How long it waited
    char output[OUTPUT_SIZE]; // Buffer for process output
    bool completed; // Is the process done?
    PROCESS_STATE state; // Current state of the process
    int priority; // Process priority
} processdata; // Struct for process info

processdata p_history[MAX_PROCESSES]; // Array to hold process info
int history_count = 0; // Counter for processes
int NCPU = 1; // Default number of CPU cores
int TSLICE = 1000; // Default time slice in milliseconds
int ready_queue[MAX_PROCESSES]; // Queue for ready processes
int queue_count = 0; // Count of processes in the queue
bool running = true; // Flag to keep shell running
sem_t semaphore; // Semaphore for controlling access

// Function to display process info
void display_command_info() {
    printf("\n************* Process Information Summary *************\n");
    printf("-------------------------------------------------------\n");
    for (int i = 0; i < history_count; i++) { 
        printf("** Process #%d **\n", i + 1);
        printf("-------------------------------------------------------\n");
        printf("  Name:           %s\n", p_history[i].process); 
        printf("  PID:            %d\n", p_history[i].pid); 
        printf("  Priority:       %d\n", p_history[i].priority); // Show priority

        // Show start and end times
        printf("  Start Time:     %ld seconds and %06ld microseconds\n", 
               (long)p_history[i].start_time.tv_sec, 
               (long)p_history[i].start_time.tv_usec);
        printf("  Completion Time: %ld seconds and %06ld microseconds\n", 
               (long)p_history[i].end_time.tv_sec, 
               (long)p_history[i].end_time.tv_usec);
        printf("  Wait Time:      %ld milliseconds\n", p_history[i].waittime); 
        printf("  Output:\n    %s\n", p_history[i].output); 
        printf("-------------------------------------------------------\n");
        free(p_history[i].process); 
    }
    printf("*******************************************************\n");
}

// Handle Ctrl+C signal
void sigint_handler(int sig) {
    running = false; 
}

// Load and execute a process
void *load_process(void *arg) {
    int index = *(int *)arg; 
    processdata *cmd_info = &p_history[index]; 

    int pipefd[2]; 
    if (pipe(pipefd) == -1) { 
        perror("Piping failed"); 
        cmd_info->completed = true;
        sem_post(&semaphore); 
        return NULL; 
    }

    pid_t child_pid = fork(); 
    if (child_pid == 0) { // Child process
        dup2(pipefd[1], STDOUT_FILENO); 
        close(pipefd[0]); 
        close(pipefd[1]); 

        // Execute the command
        execlp(cmd_info->process, cmd_info->process, (char *) NULL);
        perror("execute failed to run"); 
        exit(1); 
    } else if (child_pid < 0) { // If fork fails
        perror("Fork failed"); 
        cmd_info->completed = true;
        sem_post(&semaphore); 
        return NULL; 
    } else { // Parent process
        cmd_info->pid = child_pid; 
        cmd_info->state = RUNNING; 
        printf("Running Command '%s' (PID: %d) with Priority: %d\n", 
               cmd_info->process, cmd_info->pid, cmd_info->priority);
        
        gettimeofday(&cmd_info->start_time, NULL); 

        close(pipefd[1]); 
        
        int status;
        waitpid(child_pid, &status, 0); // Wait for child process

        // Read output from the pipe
        ssize_t bytes_read = read(pipefd[0], cmd_info->output, OUTPUT_SIZE - 1);
        if (bytes_read >= 0) {
            cmd_info->output[bytes_read] = '\0'; 
        } else {
            perror("Failed to read from pipe"); 
        }
        close(pipefd[0]); 

        // Get end time after process finishes
        gettimeofday(&cmd_info->end_time, NULL);
        cmd_info->waittime = (cmd_info->end_time.tv_sec - cmd_info->start_time.tv_sec) * 1000 +
                             (cmd_info->end_time.tv_usec - cmd_info->start_time.tv_usec) / 1000;

        cmd_info->completed = true; 
        cmd_info->state = WAITING; 
        sem_post(&semaphore); // Signal that process is done
    }
    return NULL;
}

// Function for priority-based scheduling
void priority_scheduling() {
    while (queue_count > 0) {
        // Sort the queue based on priority (lower number is higher priority)
        for (int i = 0; i < queue_count - 1; i++) {
            for (int j = 0; j < queue_count - i - 1; j++) {
                if (p_history[ready_queue[j]].priority > p_history[ready_queue[j + 1]].priority) {
                    // Swap the process indices in the queue
                    int temp = ready_queue[j];
                    ready_queue[j] = ready_queue[j + 1];
                    ready_queue[j + 1] = temp;
                }
            }
        }

        for (int i = 0; i < NCPU && i < queue_count; i++) {
            pthread_t thread;
            pthread_create(&thread, NULL, load_process, &ready_queue[i]);
            pthread_detach(thread); // Detach thread for automatic cleanup
        }
        for (int i = 0; i < NCPU && i < queue_count; i++) {
            sem_wait(&semaphore); // Wait for process to complete
        }
        // Shift ready_queue
        for (int i = 0; i < queue_count - 1; i++) {
            ready_queue[i] = ready_queue[i + 1];
        }
        queue_count--; // Decrease count after processing
    }
}

int main(int argc, char *argv[]) {
    if (argc != 3) {
        fprintf(stderr, "Usage: %s <NCPU> <TSLICE>\n", argv[0]);
        exit(1); // Exit if wrong arguments
    }
    NCPU = atoi(argv[1]); // Get number of CPUs from arguments
    TSLICE = atoi(argv[2]); // Get time slice from arguments

    sem_init(&semaphore, 0, 0); // Initialize semaphore
    signal(SIGINT, sigint_handler); // Handle Ctrl+C

    while (running) {
        printf("SimpleShell:~$ "); 
        char user_input[BUFFER]; 
        if (fgets(user_input, BUFFER, stdin) == NULL) break; // Read user input
        user_input[strcspn(user_input, "\n")] = 0; // Remove newline

        if (strcmp(user_input, "history") == 0) { // Check for history command
    printf("\n************* Process History *************\n");
    printf("Index | Process Name                | Priority\n");
    printf("------|------------------------------|---------\n");
    for (int i = 0; i < history_count; i++) {
        printf("%-6d| %-28s | %d\n", i + 1, p_history[i].process, p_history[i].priority);
    }
    printf("********************************************\n");
    continue; // Skip rest of loop
} else if (strncmp(user_input, "submit ", 7) == 0) {
            // Parsing user input for command and priority
            char *command = strtok(user_input + 7, " ");
            char *priority_str = strtok(NULL, " ");
            int priority = 1; // Default priority

            if (priority_str != NULL) {
                priority = atoi(priority_str);
                if (priority < 1 || priority > 4) {
                    printf("Priority must be between 1 and 4. Defaulting to 1.\n");
                    priority = 1;
                }
            }

            if (history_count < MAX_PROCESSES) {
                p_history[history_count].process = strdup(command); // Store process name
                p_history[history_count].completed = false; 
                p_history[history_count].state = READY; // Set to READY
                p_history[history_count].priority = priority; // Set priority
                ready_queue[queue_count++] = history_count; // Add to queue
                history_count++; // Increment history count
            } else {
                printf("Command history limit reached.\n"); // Limit reached message
            }
        }
    }
    
    priority_scheduling(); // Start processing after Ctrl+C

    display_command_info(); // Show all process info
    sem_destroy(&semaphore); // Clean up semaphore
    return 0; // Exit program
}
