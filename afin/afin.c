/**
* @file afin.c 
*
* @brief This files contents function and main function for an affine cipher
* @author Nicolas Victorino && Ignacio Nunnez
* @date 24/09/2024
*
* @copyright 2024
*/

#include "afin.h"


void cifrar(char *texto, mpz_t m, mpz_t a, mpz_t b) {
    int i = 0, len = strlen(texto);
    mpz_t temp_mult, temp_add, temp_mod, temp_char;
    char *cifrado = calloc(len + 1, sizeof(char)), temp[2];

    mpz_init(temp_mult);
    mpz_init(temp_add);
    mpz_init(temp_mod);
    mpz_init(temp_char);

    for (i = 0; i < len; i++)  {

        mpz_set_ui(temp_char, texto[i] - 'a');

        /* Multiplicar a por el valor del caracter*/
        mpz_mul(temp_mult, a, temp_char);

        /*Sumar b al resultado de la multiplicación*/
        mpz_add(temp_add, temp_mult, b);

        /*Calcular el módulo m del resultado*/
        mpz_mod(temp_mod, temp_add, m);

        /*Convertir el resultado a un caracter*/
        sprintf(&temp[0], "%c", (int)mpz_get_ui(temp_mod) + 'a');

        strcat(cifrado, &temp[0]);
    }

    cifrado[i] = '\0';

    strcpy(texto, cifrado);

    mpz_clear(temp_mult);
    mpz_clear(temp_add);
    mpz_clear(temp_mod);
    mpz_clear(temp_char);
    free(cifrado);

}

void descifrar(char *texto, mpz_t m, mpz_t a, mpz_t b) {

    //Dk(y)= (y – b) * a-1 mod m
    int i = 0, len = strlen(texto);
    mpz_t temp_sub, temp_mult, temp_mod, temp_char, *v;
    char *descifrado = calloc(len + 1, sizeof(char)), temp[2];
    int z;

    mpz_init(temp_sub);
    mpz_init(temp_mult);
    mpz_init(temp_mod);
    mpz_init(temp_char);

    v = extended_euclides(a, m, &z);

    for (i = 0; i < len; i++)  {

        mpz_set_ui(temp_char, texto[i] - 'a');

        /* Restar b al valor del caracter*/
        mpz_sub(temp_sub, temp_char, b);

        /* Multiplicar el resultado por el inverso de a*/
        mpz_mul(temp_mult, temp_sub, v[z - 1]);

        /*Calcular el módulo m del resultado*/
        mpz_mod(temp_mod, temp_mult, m);

        /*Convertir el resultado a un caracter*/
        sprintf(&temp[0], "%c", (int)mpz_get_ui(temp_mod) + 'a');

        strcat(descifrado, &temp[0]);
    }

    descifrado[i] = '\0';

    strcpy(texto, descifrado);

    for (int j = 0; j < z; j++) {
        mpz_clear(v[j]);
    }

    free(v);

    mpz_clear(temp_sub);
    mpz_clear(temp_mult);
    mpz_clear(temp_mod);
    mpz_clear(temp_char);
    free(descifrado);
}