/*
 * Name: Marc Ciruelos Santos
 * Date: 06/04/2023
 * Subject: Operating systems
 * University: Universitat Oberta de Catalunya (UOC)
 * 
 * Description: This program based on C and system calls. It uses open(), read(), 
 *              write() and close() system calls to manage a file content. Finally, 
 *              the information is shown using an standard output. 
 */

// SYSCALL INCLUDES
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <unistd.h>

// ADDITIONAL INCLUDES
#include <string.h>
#include <stdlib.h>


// ERRORS MESSAGES
#define ERROR_INVALID_ARGUMENTS "Error: Invalid number of arguments."
#define ERROR_OPENING "Error: Opening file."
#define ERROR_READING "Error: Reading file."
#define ERROR_WRITING "Error: Writing output."
#define ERROR_WRITING_ENOUGH "Error: Incorrect writing of total bytes."
#define ERROR_CLOSING "Error: Closing file"


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
 * This function opens and reads a file, and shows the content in the standard outuput. 
 * In addition, this function manages when any system call fails or not works as expected.
 * 
 * @argc int     Number of parameters inserted into this program.
 * @argv char*   Array with the content of the arguments inserted into the program. 
*/
int main(int argc, char *argv[]) {

    // Variables declaration
    int fileDescriptor;
    ssize_t bytesReaded, bytesWritten;
    char buffer; 

    // Checks if the arguments are different of 2 --> "/.argument1 argument2"
    // When it fails, writes an error message with errorMessage method and exits.
    if (argc != 2) {
        errorMessage(ERROR_INVALID_ARGUMENTS);
        exit(EXIT_FAILURE);
    } 

    // Opens the file of the second argument in read only using the open() syscall. In additon, 
    // it checks at the same time if it works fine. When it fails (-1), writes an error message with 
    // errorSystem method and exits. Therefore, open returns a "free" file descriptor number.
    if ((fileDescriptor = open(argv[1], O_RDONLY)) < 0)
        errorSystem(ERROR_OPENING); 
    
    // Reads the file descriptor, saves into the memory pointer and counts the total of number of bytes.
    // It checks at the same time if it works fine. This step is done byte by byte (while function).
    while ((bytesReaded = read(fileDescriptor, &buffer, 1)) > 0) {
       
        // It writes the information of the buffer and it shows the content in a standard output. 
        // In addition, it assigns the value returned in the bytesWritten variable.
        bytesWritten = write(1, &buffer, bytesReaded);

        // The bytesWritten variable is used to check if the write system call is working fine. 
        // When the bytesWritten is (-1), writes an error message using the errorSystem method and exits. 
        // Otherwise, when the bytes written are diferent that readed, the error (message) changes. 
        if (bytesWritten < 0) 
            errorMessage(ERROR_WRITING);
        else if (bytesWritten != bytesReaded) 
            errorMessage(ERROR_WRITING_ENOUGH);
    } 

    // It checks if the read system call worked fine assinging a value into bytesReaded variable. 
    // When the bytesReaded is (-1), writes an error message using the errorSystem method and exits.
    if(bytesReaded < 0) 
        errorSystem(ERROR_READING);  
    
    // It closes the file descriptor assigned using the close system call. At the same time, it checks 
    // when the file descriptor is not closed fine (-1), and writes an error message using the errorSystem 
    // method and exits.
    if (close(fileDescriptor) < 0)
        errorSystem(ERROR_CLOSING); 

    return 0; 
}