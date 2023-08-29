#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include "primes.h"

#define DConcurrentTasks 10

// ERROR MESSAGES
#define ERROR_INVALID_BEGIN_INTERVAL "Error: The begin interval must be greater than 1. \n"
#define ERROR_INVALID_END_INTERVAL "Error: The end interval must be greater or equal than begin interval. \n"

int main(int argc, char *argv[])
{

    // Variables declaration
    char msg[256];
    char pgm_help[]="calc_conc_primes <begin> <end>\n";
    char *endptr1=NULL, *endptr2=NULL;
    long int begin; // Beginning of the range of numbers to process (first argument of the program)
    long int end;   // End of the range of numbers to be processed (second argument of the program)
    long int process_begin, process_end, remaining_numbers, prime;
    int pipe_in[2], pid, status;
    char buffer[16];
    int bytesReaded, counter = 1; 

    // Process arguments (ToDo)
    if (argc<3)
    {
        sprintf(msg,"Error number of arguments (%d arguments passed, 3 required).\n",argc);
        write(2,msg,strlen(msg));
        write(2,pgm_help,strlen(pgm_help));
        exit(1);
    }


    // Create pipe for comunication (ToDo)
    if (pipe(pipe_in) < 0){
        sprintf(msg, "Error creating the pipe.\n");
        write(2, msg, strlen(msg));
        exit(EXIT_FAILURE);
    }

    // It converts the <begin_interval> and <end_interval> from the arguments to integer. 
    begin = atoi(argv[1]);
    end = atoi(argv[2]);

    // It checks if the interval is greater than 1. Otherwise, returns an error with a custom message.
    if (begin <= 1) {
        write(2, ERROR_INVALID_BEGIN_INTERVAL, strlen(ERROR_INVALID_BEGIN_INTERVAL));
        exit(EXIT_FAILURE);
    }

    // It checks if the end interval is greater than the begin interval. Otherwise, returns an error 
    // with a custom message.
    if (end < begin) {
        write(2, ERROR_INVALID_END_INTERVAL, strlen(ERROR_INVALID_END_INTERVAL));
        exit(EXIT_FAILURE);
    }

    // Calculate the process number
    remaining_numbers = end-begin + 1;
    process_begin = begin;
    process_end = end; 

    // Message to indicate the remaining intervals
    sprintf(msg,"Interval numbers [%ld-%ld] -> Remanining: %ld.\n", process_begin, process_end, remaining_numbers);
    write(1,msg,strlen(msg));

    // Create DConcurrentTasks process and assign tasks
    for (int p=0; p<DConcurrentTasks; p++) 
    {
        // Variables declaration
        int pid;
        long int process_numbers;
        char process_begin_str[20], process_end_str[20];

        // Calculate process sub-interval numbers to check
        process_numbers = remaining_numbers / (DConcurrentTasks-p);
        process_end = process_begin + process_numbers-1;
        remaining_numbers -= process_numbers;        

        sprintf(msg,"Creatinng child %d to process innterval numbers [%ld-%ld].\n",p+1, process_begin, process_end);
        write(1,msg,strlen(msg));

        // Create concurrent process p+1 (ToDo). 
        pid = fork();

        if (pid < 0) {
            sprintf(msg, "Error creating the fork.\n");
            write(2, msg, strlen(msg));
            exit(EXIT_FAILURE);
        }

        // if child_process  (ToDo) 
        if (pid == 0)
        {   // Child process.

            // Redirect pipe in to child's stdout (ToDo)
            dup2(pipe_in[1], STDOUT_FILENO);

            // Closes pipe descriptrs not used. (ToDo)
            close(pipe_in[0]);

            // Convert child argumets to string. (ToDo) 
            sprintf(process_begin_str, "%ld", process_begin);
            sprintf(process_end_str, "%ld", process_end);

            // Child process executes calc_primes program (ToDo)
            execlp("./calc_primes_3", "calc_primes_3", process_begin_str, process_end_str, NULL);

            // Execlp error. (ToDo)
            sprintf(msg, "Error executing Execlp program.\n");
            write(STDERR_FILENO, msg, strlen(msg));
            exit(EXIT_FAILURE);
            break;
        }
        process_begin = process_end + 1;
    }

    // Close not used pipe descriptors.
    close(pipe_in[1]);

    // Receiving prime results (ToDo)
    while ((bytesReaded = read(pipe_in[0], buffer, sizeof(buffer))) > 0) {
        
        // Bytes readed. If it fails, returns an error. 
        if(bytesReaded < 0) 
            write(STDOUT_FILENO, "Error: Reading file.\n", strlen("Error: Reading file.\n"));


        // Check all bytes readed to find the prime numbers
        for (int i = 0; i < bytesReaded; i++) {
            prime = buffer[i]; 
            
            // Show the prime numbers using the standard output. 
            if (prime != 0){
                sprintf(msg, "Found prime number %d: %ld \n", counter, prime);
                counter++;
                write(STDOUT_FILENO, msg, strlen(msg));
            }
        }
    }

    // Wait child process finish (ToDo).
    for (int p = 0; p < DConcurrentTasks; p++) {
        if (waitpid(-1, &status, 0) < 0) {
            sprintf(msg, "Error waiting for the child process.\n");
            write(2, msg, strlen(msg));
            exit(EXIT_FAILURE);
        }    
    }

    return 0;
}