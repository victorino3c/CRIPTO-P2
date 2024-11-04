#ifndef AFIN_H
#define AFIN_H

#include "../utiles/utils.h"

/**
 * @brief prints the information of the program for its execution
 * 
 */
void printExeInfoAfin();

/**
 * @brief cypher function for the affine method
 * 
 * @param texto pointer to the text to be cyphered (input and output)
 * @param m module for the cypher (GMP Library)
 * @param a key for the cypher (GMP Library)
 * @param b key for the cypher (GMP Library)
 */
void cifrar(char *texto, mpz_t m, mpz_t a, mpz_t b);

/**
 * @brief decypher function for the affine method
 * 
 * @param texto pointer to the text to be decyphered (input and output)
 * @param m module for the cypher (GMP Library)
 * @param a key for the cypher (GMP Library)
 * @param b key for the cypher (GMP Library)
 */
void descifrar(char *texto, mpz_t m, mpz_t a, mpz_t b);

#endif // AFIN_H