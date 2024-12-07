/**
 * @file SBOX_AES.c
 * @author Nicolas Victorino && Ignacio Nunnez
 * @brief 
 * @version 0.1
 * @date 2024-11-24
 * 
 * @copyright Copyright (c) 2024
 * 
 */

#include "../utiles/utils.h"

#define OUTPUT_BUFFER 4096

/**
 * @brief Function to apply xtime to a value
 * 
 * @param x value to which we are going to apply xtime
 * 
 * @return xtime of x
 */
uint8_t xtime(uint8_t x);

/**
 * @brief Fast way to apply the affine transformation
 * 
 * @param q value to which we are going to apply the affine transformation
 * 
 * @return affine transformation of q
 */
uint8_t affine_transformation(uint8_t q);

/**
 * @brief Function to initialize the sbox
 * 
 * @param sbox array where the sbox is going to be stored
 * 
 * @return void
 */
void initialize_aes_sbox(uint8_t sbox[256]);

/**
 * @brief Function to divide two polynomials in GF(2^8)
 * 
 * @param dividend dividend
 * @param divisor divisor
 * @param quotient pointer to the quotient
 * @param remainder pointer to the remainder
 * 
 * @return void
 */
void gf256_div(uint16_t dividend, uint16_t divisor, uint16_t *quotient, uint16_t *remainder);

/**
 * @brief Function to get the multiplicative inverse of a polinomial with base 7 and module 11B.
 *        It is not a direct implementation of the algorithm, but a simplified version for the AES.
 * 
 * @param byte byte from which we are going to calculate the multiplicative inverse
 * 
 * @return inverse of byte
 */
void extended_euclides_GF(uint8_t sbox[256]);

/**
 * @brief Function to initialize the inverse sbox
 * 
 * @param sbox sbox to invert
 * @param inv_sbox array where the inverse sbox is going to be stored
 * 
 * @return void
 */
void initialize_aes_inv_sbox(uint8_t sbox[256], uint8_t inv_sbox[256]);

int main(int argc, char *argv[]) {
    uint8_t sbox[256], inv_sbox[256];
    char modo = 'c';
    char *fileout = NULL;
    char *output = (char *) malloc(OUTPUT_BUFFER * sizeof(char));

    if(argc > 1) {
        if (strcmp(argv[1], "-C") == 0) {
            modo = 'c';
        } else if (strcmp(argv[1], "-D") == 0) {
            modo = 'd';
        } else {
            printf("Modo no valido\n");
            return -1;
        }
    } else {
        printf("Error en los argumentos\n");
        printf("Uso: %s [-c|-d] -o [fichero]\n", argv[0]);
        return -1;
    }

    if (argc > 3) {
        fileout = argv[3];
        if (fileout == NULL) {
            printf("Error al abrir el archivo\n");
            return -1;
        }
    } 

    /* Get the values of the direct sbox*/
    initialize_aes_sbox(sbox);

    if(modo == 'd') {
        initialize_aes_inv_sbox(sbox, inv_sbox);
    }

    /*Print the sboxes*/
    if(modo == 'c') {
        for (int i = 1; i < 257; i++) {
            snprintf(output + strlen(output), OUTPUT_BUFFER-strlen(output), "%02x ", sbox[i-1]);
            if(i % 16 == 0 && i > 15) {
                snprintf(output + strlen(output), OUTPUT_BUFFER-strlen(output), "\n");
            }
        }
    /*Print the inverse s boxes*/
    } else {
        for (int i = 1; i < 257; i++) {
            snprintf(output + strlen(output), OUTPUT_BUFFER-strlen(output), "%02x ", inv_sbox[i-1]);
            if(i % 16 == 0 && i > 15) {
                snprintf(output + strlen(output), OUTPUT_BUFFER-strlen(output), "\n");
            }
        }
    }

    if (handleOutputText(fileout, output) == -1) {
        printf("Error al mostrar la salida\n");
        free(output);
        return 1;
    }

    free(output);

    return 0;
}

uint8_t xtime(uint8_t x) {
    return (x << 1) ^ ((x & 0x80) ? 0x1B : 0x00);
}

uint8_t affine_transformation(uint8_t q) {
    //printf("Initial: %x\n",q);
    uint8_t p[4] = {0,0, 0, 0};
    for (int i = 1; i <= 4; i ++) {
        p[i-1] = q << i | q >> (8 - i);
    }

    q = q^p[0]^p[1]^p[2]^p[3];
    
    return q;
}

void initialize_aes_sbox(uint8_t sbox[256]) {
    extended_euclides_GF(sbox);
    for(int i = 0; i < 256; i++) {
        sbox[i] = affine_transformation(sbox[i]) ^ 0x63;
    }
}

void gf256_div(uint16_t dividend, uint16_t divisor, uint16_t *quotient, uint16_t *remainder) {
    *quotient = 0;
    *remainder = dividend;

    // Find the degree (highest bit position) of the dividend and divisor
    int dividend_degree = 15;
    int divisor_degree = 15;

    // Find the degree of the dividend
    while (dividend_degree >= 0 && ((dividend & (1 << dividend_degree)) == 0)) {
        dividend_degree--;
    }
    // Find the degree of the divisor
    while (divisor_degree >= 0 && ((divisor & (1 << divisor_degree)) == 0)) {
        divisor_degree--;
    }

    // Perform the division algorithm
    while (dividend_degree >= divisor_degree) {
        int shift = dividend_degree - divisor_degree;
        *quotient |= (1 << shift);
        *remainder ^= (divisor << shift);

        // Update the degree of the remainder
        while (dividend_degree >= 0 && ((*remainder & (1 << dividend_degree)) == 0)) {
            dividend_degree--;
        }
    }
}

void extended_euclides_GF(uint8_t sbox[256]) {
	uint8_t p = 1, q = 1;
	
	/* loop invariant: p * q == 1 in the Galois field */
	do {
		/* multiply p by 3, we use the xtime function */
		p = p ^ xtime(p);

		/* divide q by 3 (equals multiplication by 0xf6) */
		q ^= q << 1;
		q ^= q << 2;
		q ^= q << 4;
		q ^= q & 0x80 ? 0x09 : 0;

        sbox[p] = q;

	} while (p != 1);

    /* 0 is a special case since it has no inverse */
    sbox[0] = 0x63;
}

void initialize_aes_inv_sbox(uint8_t sbox[256], uint8_t inv_sbox[256]) {
    for (int i = 0; i < 256; i++) {
        inv_sbox[sbox[i]] = i;
    }
}