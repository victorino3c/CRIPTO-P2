/**
* @file utils.h 
* @brief Define the functions used in the utils.c file
* @author Nicolas Victorino && Ignacio Nunnez
* @date 24/09/2024
*/

#ifndef UTILS_H
#define UTILS_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <gmp.h>
#include <time.h>
#include <math.h>

/**
 * @brief Filters the text to get it ready to get encoded. Transforms all the upper case letters to lower case and removes all the other symbols
 * 
 * @param texto the text that is going to get encoded
 */
void filtrarTexto(char *texto);

/**
 * @brief Function to handle the input of text
 * 
 * @param i name of the file to open. If it is null we read the text from standard input
 * 
 * @return return the input text. NULL if there is a problem
 */
char * handleInputText(char * i);

/**
 * @brief Function to handle the output of text
 * 
 * @param o name of the file to output the text. If it is null we write the text to the standard output
 * 
 * @return 0 if everything is okay. -1 if there is a problem
 */
int handleOutputText(char *o, char *texto);

/**
 * @brief Function to add filling to the text that it is going to be encoded. It adds 'x' to make the length of the text a multiple of n (size of the array)
 * 
 * @param n size of the array with the key
 * @param text contains the text to be encoded
 */
char* addFilling(int n, char *texto);

/**
 * @brief Calculates the MCD of a and b and all the quotients of the division following the Euclides algorithm.
 * 
 * @param a 
 * @param b 
 * @param z return the size of the array (quotients)
 * @return mpz_t* array with the quotients of the division. If last element array[z-1] is 1, then a and b are coprimes.
 */
mpz_t *euclides(mpz_t a, mpz_t b, int * z);

/**
 * @brief Calculates the inverse of a in modul mod.
 * 
 * @param a 
 * @param mod 
 * @param tam return the size of the array (quotients)
 * @return mpz_t* array with the quotients of the algorithm. Last element array[tam-1] is the inverse of a in modul mod.
 *         returns null if a and mod are not coprimes.
 */
mpz_t *extended_euclides(mpz_t a, mpz_t mod, int * tam);

/**
 * @brief Checks if 2 given numbers are coprimes. It also saves the quotients of the division.
 * 
 * @param det determinant of the matrix
 * @param m size of the language
 * @param n size of the key
 * @param keys matrix with the key
 * @param z return the size of the array (quotients + mcd(det, m))
 * 
 * @return int* array with the quotients of the division. If last element array[z-1] is 1, then det and m are coprimes.
 */
int * euclides2(int det, int m, int n, int *z);

/**
 * @brief Calculates the multiplicative inverse of det in modul m.
 * 
 * @param det determinant of the matrix
 * @param m size of the language
 * @param z return the size of the array (quotients + mcd(det, m))
 * 
 * @return int* array with the quotients of the division. Last element array[z-1] is the inverse of det in modul m.
 *        returns null if det and m are not coprimes.
 */
int * extended_euclides2(int det, int m, int n, int *tam);

/**
 * @brief Simplified Euclides algorithm , it just returns the value of the greatest commin divisor, and not a whole list with the quotients
 * 
 * @param a first value
 * @param b secund value
 * 
 * @return gcd of both values
 */
int simpleMCD(int a, int b);

/**
 * @brief finds the gcd of multiple values
 * 
 * @param numbers array with all the numbers
 * @param size size of the array
 * 
 * @return 1 if two of the values are coprimes, gcd in other cases
 */
int find_gcd_of_list(int* numbers, int size);

/**
 * @brief function to generate a random number between inf and sup
 * 
 * @param inf inferior limit
 * @param sup superior limit
 * 
 * @return random number
 */
int random_num(int inf, int sup);

/**
 * @brief Generate a random permutation with size n, that has the values from 1 to n. 
 * 
 * @param n size of the permutation
 * @param permutation array where the permutation is going to be stored
 *
 */
void generatePermutation(int n , int *permutation);

/**
 * @brief Invert a permutation
 * 
 * @param permutation permutation to be inverted
 * @param n size of the permutation
 */
void invertPermutation(int *permutation, int n);

#endif