/*
 * Name: Marc Ciruelos Santos
 * Date: 24/05/2023
 * Subject: Operating systems
 * University: Universitat Oberta de Catalunya (UOC)
 * 
 * Description: This program based on C and system calls. It uses fork() and dynamic memory to
 *              calculate the prime numbers form an interval. These intervals are given as 
 *              parameters. Also, it uses the execlp() syscall to check if it the number is Prime.
 *              The main goals are: 
 *                  - Use the children process to check the prime number using the output of the
 *                    isPrime program executed by execlp() Syscall.
 * 
 *                  - To avoid the concurrency and to execute the program waiting the end of the 
 *                    child execution to continue/create another one child.
 * 
 *                  - Use the parent process to manage the concurrency and create the new number
 *                    list with only the prime numbers.
 * 
 */

// SYSCALL INCLUDES
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <errno.h>
#include <unistd.h>

// ADDITIONAL INCLUDES
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

// ERRORS MESSAGES
#define ERROR_INVALID_ARGUMENTS "Error: Invalid number of arguments."
#define ERROR_INVALID_BEGIN_INTERVAL "Error: The begin interval must be greater than 1."
#define ERROR_INVALID_END_INTERVAL "Error: The end interval must be greater or equal than begin interval."
#define ERROR_FORK_MSG "Error: fork() syscall was not completed as expected"
#define ERROR_WRITING "Error: Writing output."
#define ERROR_EXEC_MSG "Error: execlp() execution was not completed as expected"
#define ERROR_MEMORY_ASSIGNED "Error: Can't assign memory to the variable"

// FORK STATUSES
#define FAILED -1
#define CHILD   0


/**
 * This function writes a message error using the standard errors channel (channel 2). 
 * 
 * @message char* Message defined as a general constant.
*/
void errorMessage(char *message)
{
    // Write errors using standard errors channel (channel 2)
    write(2, message, strlen(message));
    write(2, "\n", 1);
}


/**
 * This function writes a message error, but in this case is related directly 
 * to the system error. It uses 'errno' to know the related error. It writes a
 * custom message and the system error using the standard errors channel (channel 2).
 * 
 * Finally ends the execution with EXIT_FAILURE = 1
 * 
 * @message char* Message defined as a general constant.
*/
void errorSystem(char *message)
{
    errorMessage(message);
    write(2, strerror(errno), strlen(strerror(errno)));
    write(2, "\n", 1);
    exit(EXIT_FAILURE);
}

/**
*  This method writes the primes numbers through the standard output. Also, checks if the write syscall
*  works fine. Otherwise, returns an error message. 
*/
void writeOutput(int* primesNumberList, int size) { 

    // Variables declaration
    char buffer[16];
    ssize_t bytesWritten;

    // This loop checks each number of the interval introduced by the user as a parameter. 
    for (int counter = 0; counter < size-1; counter++) {

        // It converts a prime number to character string buffer. 
        snprintf(buffer, sizeof(long int), "%d \n" ,primesNumberList[counter]);

        // It writes the information of the buffer and it shows the content in a standard output. 
        // In addition, it assigns the value returned in the bytesWritten variable.
        bytesWritten = write(STDOUT_FILENO, buffer, sizeof(int));

        // The bytesWritten variable is used to check if the write system call is working fine. 
        // When the bytesWritten is (-1), writes an error message using the errorSystem method and exits. 
        // Otherwise, when the bytes written are diferent that readed, the error (message) changes. 
        if (bytesWritten < 0) 
        errorMessage(ERROR_WRITING);
    }
}

/**
 * This function receive TWO ELEMENTS as programs parameters (<begin_interval>, <end_interval>).
 *        Then, it checks all the numbers of the intervals provided, to achieve the main goals: 
 *      - Use the children process to check the prime number using the output of the isPrime program
 *        executed by execlp() Syscall.
 * 
 *      - To avoid the concurrency and to execute the program waiting the end of the child execution 
 *        to continue/create another one child.
 * 
 *      - Use the parent process to manage the concurrency and create the new number list with only 
 *        the prime numbers.
 * 
 * Moreover, this task is done using fork() syscall trying split in two process the main goals. If there
 * is any error, it will return an error message. 
 *      
 * @argc int     Number of parameters inserted into this program.
 * @argv char*   Array with the content of the arguments inserted into the program. 
*/
int main(int argc, char *argv[]) {

    // Variables declaration
    int fileDescriptor, beginInterval, endInterval, number, status; 
    int size = 1, j = 0;
    int* primesNumberList = NULL; 
    char buffer[16];

    // WRITE OUTPUT FILE EXCLUDED! In this exercise is not mandatory. 
    // Checks if the arguments are different of 3 --> "/.argument1 argument2 argument3"
    // When it fails, writes an error message with errorMessage method and exits.
    if (argc != 3) {
        errorMessage(ERROR_INVALID_ARGUMENTS);
        exit(EXIT_FAILURE);
    } 

    // It converts the <begin_interval> and <end_interval> from the arguments to integer. 
    beginInterval = atoi(argv[1]);
    endInterval = atoi(argv[2]);

    // It checks if the interval is greater than 1. Otherwise, returns an error with a custom message.
    if (beginInterval <= 1) {
        errorMessage(ERROR_INVALID_BEGIN_INTERVAL);
        exit(EXIT_FAILURE);
    }

    // It checks if the end interval is greater than the begin interval. Otherwise, returns an error 
    // with a custom message.
    if (endInterval < beginInterval) {
        errorMessage(ERROR_INVALID_END_INTERVAL);
        exit(EXIT_FAILURE);
    }

   // It assigns memory to the primesNumberList to save all the prime numbers. 
    primesNumberList = (int*)malloc(size * sizeof(int));    

    // It checks if the primesNumberList got memory enough. Otherwise, returns an error and finish the program. 
    if (primesNumberList == NULL) {
        errorMessage(ERROR_MEMORY_ASSIGNED);
        exit(EXIT_FAILURE);
    }

    /**
    * This loop checks each number of the interval introduced by the user as a parameter using
    * the fork() Syscall to distribute the tasks. 
    *        - Child: Execute the IsPrime program and save the returned value.
    *        - Parent: Receive the returned value by the child and save (or not) the prime number.
    */
    for (number = beginInterval; number <= endInterval; number++) {
        
        /**
        *  First, switch executes the fork() syscall, which it returns a value. Then, depending 
        *  of this value, there are three different cases to be executed. 
        * 
        *       1. Failed: When the fork() not works right returns an error (-1) and it shows an 
        *                  error custom message. Otherwise, the fork() syscall creates a child 
        *                  process succesfully (cases 1 & 2).
        * 
        *       2. Child:  For the child process, zero is assigned as the PID by default. Hence, 
        *                  this case is only executed by the child process.  
        * 
        *       3. Parent: For the parent process, it has a number greater than zero as the PID. 
        *                  Usually the PID of the child process. Therefore, this process is executed 
        *                  by the parent process. 
        */ 
        switch(fork()) {

            // (FAILED = -1). The fork() syscall didn't work and returns an error custom message. 
            case FAILED:
                errorMessage(ERROR_FORK_MSG);
                break;

            // (CHILD = 0). This case is only for the child process. 
            case CHILD:
                
                // It converts a prime number to character string buffer. 
                snprintf(buffer, sizeof(long int),"%d", number);

                // Replaces the image of the current program with the "date" program. 
                execlp("./IsPrime", "IsPrime", buffer, NULL);

                // Error with a custom message using the error() method.
                errorSystem(ERROR_EXEC_MSG);
                break;

            // PARENT
            default:

                /**
                *  It waits for the child process is finished. In this case, the value returned in the child
                *  process is saved in the "status" variable for manage it, because if it's true, the number will
                *  be added to the primesNumberList array. 
                */
                wait(&status);
                   
                /**
                 * It checks the value of the "status" variable. This value is related to the "return value"
                 * of the program isPrime executed in the child process. If the value returned is true, it 
                 * means that the number was prime.
                 */
                if (WEXITSTATUS(status)){

                    // The size counter of the primesNumberList is incremented by one. 
                    size++;

                    // It assigns more memory to the primesNumberList to save another prime number. 
                    primesNumberList = realloc(primesNumberList, size * sizeof(int));

                    // It checks if the primesNumberList got memory enough. Otherwise, returns an error and finish the program. 
                    if (primesNumberList == NULL) {
                        errorMessage(ERROR_MEMORY_ASSIGNED);
                        exit(EXIT_FAILURE);
                    }

                    // It assings the prime number to the j position in the array of prime numbers.
                    primesNumberList[j] = number; 
                    j++;
                }
                break;
        }
    }

    // This funcion is explained in the header. It writes the primes number trough the "SOut"
    writeOutput(primesNumberList, size); 

    // It frees the memory allocated for the primesNumberList array. 
    free(primesNumberList);
   
    return 0; 
}