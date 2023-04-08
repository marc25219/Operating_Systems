/*
 * Name: Marc Ciruelos Santos
 * Date: 07/04/2023
 * Subject: Operating systems
 * University: Universitat Oberta de Catalunya (UOC)
 * 
 * Description: This program based on C uses the execl() method to execute "ls -l" command. 
 */

// INCLUDE
#include <unistd.h> 
#include <errno.h>

// ADDITIONAL INCLUDES
#include <string.h>
#include <stdlib.h>

// ERRORS MESSAGES
#define ERROR_EXEC "Error: Execl execution was not complete as expected"

/**
 * This function writes a message error, but in this case is related directly 
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
 *  This function executes the comand "ls -l" of the program "ls" using the execl() function.
 *  When it fails, then writes an error using error() method.
*/
int main() {

    // Execl function: 
    // 1 Argument: Program path
    // 2 & 3 Arguments: execution arguments (ls -l)
    // 4 Argument: NULL indicates the end of arguments received in this function.
    execl("/bin/ls", "ls", "-l", NULL);

    // Error with a custom message using the error() method.
    error(ERROR_EXEC);

    return 0;
}