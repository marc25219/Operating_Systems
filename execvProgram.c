/*
 * Name: Marc Ciruelos Santos
 * Date: 08/04/2023
 * Subject: Operating systems
 * University: Universitat Oberta de Catalunya (UOC)
 * 
 * Description: This program based on C uses the execv() method to execute "ls -l" command. 
 */

// INCLUDE
#include <unistd.h> 
#include <errno.h>

// ADDITIONAL INCLUDES
#include <string.h>
#include <stdlib.h>

// ERRORS MESSAGES
#define ERROR_EXEC "Error: execv() execution was not complete as expected"

/**
 * This method writes a message error, but in this case is related directly 
 * to the system error. It uses 'errno' to know the related error. It writes a
 * custom message and the system error using the standard errors channel (channel 2).
 * 
 * Finally ends the execution with EXIT_FAILURE = 1
 * 
 * @message char* Message defined as a general constant.
*/
void error(char *message)
{
    // Write errors using standard errors channel (channel 2)
    write(2, message, strlen(message));
    write(2, "\n", 1);
    write(2, strerror(errno), strlen(strerror(errno)));
    write(2, "\n", 1);
    exit(EXIT_FAILURE);
}


/**
 *  This function executes the comand "ls -l" of the program "ls" using the execv() function.
 *  When it fails, then writes an error using error() method.
*/
int main() {

    // Array of pointers to charactes with the arguments (ls -l)
    char *args[] = { "ls", "-l", NULL };

    /**
     * Execl function: 
     *  1st parameter: Program path 
     *  2nd parameter: Array of pointers to charactes to give to the program.
     *      - Command to execute (ls)
     *      - Command to execute (-l)
     *      - NULL (last argument as warning)
    */
    execv("/bin/ls", args);

    // Error with a custom message using the error() method.
    error(ERROR_EXEC);

    return 0;
}
