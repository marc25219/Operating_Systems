/*
 * Name: Marc Ciruelos Santos
 * Date: 17/05/2023
 * Subject: Operating systems
 * University: Universitat Oberta de Catalunya (UOC)
 * 
 * Description: This program based on C uses the fork() System Call to create a child 
 *              process. Furthermore, it executes each process individually with their
 *              own context: 
 * 
 *              CHILD:  Save the current date into a file ("file.txt"). 
 *              PARENT: Shows a message with the end of the program.
 */

// INCLUDE
#include <unistd.h> 
#include <errno.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <fcntl.h>

// ADDITIONAL INCLUDES
#include <string.h>
#include <stdlib.h>

// ERROR MESSAGES
#define ERROR_EXEC_MSG "Error: execlp() execution was not completed as expected"
#define ERROR_FORK_MSG "Error: fork() syscall was not completed as expected"
#define ERROR_OPENING  "Error: Opening file."
#define ERROR_DOUBLING  "Error: Doubling file."

// FORK STATUSES
#define FAILED -1
#define CHILD   0


/**
 * This function writes an error message using the standard errors channel (channel 2). 
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
 * This function writes an error message, but in this case is related directly 
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
 *  The main creates a child process through the fork() System Call. Then, 
 *  the child process inserts the current date into the "file.txt" file. Also, the 
 *  parent process shows the "End program" message using the standard output. 
*/
int main() {

    // Declaration of variables
    int fileDescriptor, status;

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

        // (CHILD = 0). This case is only for the child process. 
        case CHILD: 

            /**
            *  Open() syscall opens as a write only mode the file and returns the fileDescriptor 
            *  of the "file.txt". Also, manages the file.txt:
            *   - If the file not exists, it is created.
            *   - If the file exists, it is truncated.
            * 
            *   In addition, as a requierement of this exercise, the system("date > ./file.txt") 
            *   creates a new file with the "0664" privileges (-rw-rw-r--). 
            * 
            *   In case of error (-1), it returns an error custom message.
            */
            if ((fileDescriptor = open("file.txt", O_CREAT|O_WRONLY|O_TRUNC, 0664)) < 0) {
                errorSystem(ERROR_OPENING);             
            }

            /**
            *  dup2() Syscall duplicates the fileDescriptor to allow to the standard output (STDOUT_FILENO = 1), 
            *  to appoint to the "file.txt" too.
            *  
            *  Explanation: In this case, the fileDescriptor appoints to the "file.txt", and with this syscall 
            *  execution, it allows to appoint the standard output to the "file.txt" too. 
            * 
            *  Hence, all the content shown in the standard output, will be redirected/inserted to the "file.txt" 
            *  file (and not shown in the standard output). 
            * 
            *  In case of error, an error custom message is shown. 
            */
            if ((dup2(fileDescriptor, STDOUT_FILENO)) < 0) {
                errorSystem(ERROR_DOUBLING);             
            }

            // It closes the old fileDescriptor, because the standard output is now the "new fileDescriptor". 
            close(fileDescriptor);

            // Replaces the image of the current program with the "date" program. 
            execlp("date", "date", NULL);

            // Error with a custom message using the error() method.
            errorSystem(ERROR_EXEC_MSG);
            
            // Finish this case, it is the same as "break;"
            exit(0);

        // PARENT
        default: 

            /**
            *  It waits for the child process is finished. In this case, the value returned in the child
            *  process is saved in the "status" variable for manage it. But in this program is not used.
            *  Also, wait(NULL) will work. 
            */
            wait(&status);

            // It shows the "End Program" message using the standard output (Channel 1)
            write(1, "End Program\n",strlen("End Program\n"));

            // Finish this case, it is the same as "break;"
            exit(0);
    }
    return 0;
}
