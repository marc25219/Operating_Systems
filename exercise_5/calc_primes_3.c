/*
 * Name: Marc Ciruelos Santos
 * Date: 24/05/2023
 * Subject: Operating systems
 * University: Universitat Oberta de Catalunya (UOC)
 * 
 * Description: This program based on C and system calls. It uses fork() and dynamic memory to
 *              calculate the prime numbers form an interval. These intervals are given as 
 *              parameters. The main goals is: 
 *                  - Show the prime numbers through the standard output. (Binary format)
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
#include "primes.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

// ERRORS MESSAGES
#define ERROR_INVALID_ARGUMENTS "Error: Invalid number of arguments. \n"
#define ERROR_WRITING "Error: Writing output.\n"
#define ERROR_MEMORY_ASSIGNED "Error: Can't assign memory to the variable\n"

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
void writeOutput(long int* primesNumberList, int size) { 

    // Variables declaration
    char buffer[16];
    ssize_t bytesWritten;

    // This loop checks each number of the interval introduced by the user as a parameter. 
    for (int counter = 0; counter < size-1; counter++) {

        // It writes the information of the buffer and it shows the content in a standard output. 
        // In addition, it assigns the value returned in the bytesWritten variable.
        bytesWritten = write(STDOUT_FILENO, &primesNumberList[counter], sizeof(primesNumberList[counter]));

        // The bytesWritten variable is used to check if the write system call is working fine. 
        // When the bytesWritten is (-1), writes an error message using the errorSystem method and exits. 
        // Otherwise, when the bytes written are diferent that readed, the error (message) changes. 
        if (bytesWritten < 0) 
        errorMessage(ERROR_WRITING);
    }
}

/**
 * This function receive three elements as programs parameters (<begin interval>, <end interval> and
 * <output_name>. Then, it check all the numbers of the intervals provided, to achieve the main goal: 
 *      - Show the prime numbers through standard output (Binary format).
 * 
 * Moreover, this task is done using fork() syscall trying split in two process the main goals. If there
 * is any error, it will return an error message. 
 *      
 * @argc int     Number of parameters inserted into this program.
 * @argv char*   Array with the content of the arguments inserted into the program. 
*/
int main(int argc, char *argv[]) {

    // Variables declaration
    int fileDescriptor, beginInterval, endInterval, status; 
    long int number; 
    int size = 1, j = 0;
    long int* primesNumberList = NULL; 

    // It checks if the arguments are different of 3 --> "/.argument1 argument2 argument3"
    // When it fails, writes an error message with errorMessage method and exits.
    if (argc != 3) {
        errorMessage(ERROR_INVALID_ARGUMENTS);
        exit(EXIT_FAILURE);
    } 

    // It converts the <begin_interval> and <end_interval> from the arguments to integer. 
    beginInterval = atoi(argv[1]);
    endInterval = atoi(argv[2]);

    // It assigns memory to the primesNumberList to save the prime numbers. 
    primesNumberList = (long int*)malloc(size * sizeof(long int));    

    // It checks if the primesNumberList got memory enough. Otherwise, returns an error and finish the program. 
    if (primesNumberList == NULL) {
        errorMessage(ERROR_MEMORY_ASSIGNED);
        exit(EXIT_FAILURE);
    }


    // It inserts the prime numbers into the primesNumberList. Also, it counts the length of the primesNumberList 
    // variable. Finally, if the number is prime, it gives more memory to the variable through realloc(). 
    for (number = beginInterval; number <= endInterval; number++) {

        // It checks if the number is prime using the boolean function of "isPrime.h" file. 
        if (isPrime(number)){
            
            // The size counter of the primesNumberList is incremented by one. 
            size++;

            // It assigns more memory to the primesNumberList array to save another prime number. 
            primesNumberList = realloc(primesNumberList, size * sizeof(long int));
            
            // It checks if the primesNumberList got memory enough. Otherwise, returns an error and finish the program. 
            if (primesNumberList == NULL) {
                errorMessage(ERROR_MEMORY_ASSIGNED);
                exit(EXIT_FAILURE);
            }

            // It assings the prime number to the j position in the array of prime numbers.
            primesNumberList[j] = number; 
            j++;
        }
    }

    // This funcion is explained in the header. It writes the primes number trough the "SOut"
    writeOutput(primesNumberList, size); 

    // It frees the memory allocated for the primesNumberList array. 
    free(primesNumberList);
    
    return 0; 
}