/*
 * Name: Marc Ciruelos Santos
 * Date: 24/05/2023
 * Subject: Operating systems
 * University: Universitat Oberta de Catalunya (UOC)
 * 
 * Description: This program is based on C language to support to the calc_primes2.c. 
 *              The main goal is to return true if the number provided as a parameter
 *              is prime or not. Otherwise, it returns false.
 */

// INCLUDES
#include <stdbool.h>
#include <stdlib.h>

/**
 * This function receive one element as a number, and it checks if the number is prime or not. 
 *      
 * @argc int     Number of parameters inserted into this program.
 * @argv char*   Array with the content of the arguments inserted into the program. 
*/
int main(int argc, char *argv[]) {

    // Converts the number received as a paramater to integer.
    int number = atoi(argv[1]); 

        // The number 1 never is prime. It returns false.
        if (number==1)
            return false;
                
        // The number 2 never is prime. It returns false.
        if (number==2)
            return true;
        
        // When the remainder of a number divided by 2 is equal than 0, it is a composite number. 
        // Also, it returns false.
        if (number%2==0)
            return false;

        // When the remainder of a number divided by 3 is equal than 0, it is a composite number. 
        // Also, it returns false.
        for (long int i = 3; i * i <= number; i += 2) {
            if (number % i == 0) {
                return false;
            }
        }

    // Finally, for the rest of the cases, it means that the number is prime. It returns true.
    return true; 
}