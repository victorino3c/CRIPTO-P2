#include "../utiles/utils.h"

#define OUTPUT_BUFFER 4096

// xtime operation for multiplication by x in GF(2^8)
uint8_t xtime(uint8_t x) {
    return (x << 1) ^ ((x & 0x80) ? 0x1B : 0x00);
}

/**
 * @brief Fast way to apply the affine transformation
 * 
 * @param q value to which we are going to apply the affine transformation
 * 
 * @return affine transformation of q
 */
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
	uint8_t p = 1, q = 1;
	
	/* loop invariant: p * q == 1 in the Galois field */
	do {
		/* multiply p by 3 */
		p = p ^ xtime(p);

		/* divide q by 3 (equals multiplication by 0xf6) */
		q ^= q << 1;
		q ^= q << 2;
		q ^= q << 4;
		q ^= q & 0x80 ? 0x09 : 0;
        
		/* Apply the affine transformation */
        uint8_t xformed = affine_transformation((uint8_t)q);

		sbox[p] = xformed ^ 0x63;
	} while (p != 1);

	/* 0 is a special case since it has no inverse */
	sbox[0] = 0x63;
}

void initialize_aes_inv_sbox(uint8_t sbox[256], uint8_t inv_sbox[256]) {
    for (int i = 0; i < 256; i++) {
        inv_sbox[sbox[i]] = i;
    }
}

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
    }
    if (argc > 3) {
        fileout = argv[3];
        if (fileout == NULL) {
            printf("Error al abrir el archivo\n");
            return -1;
        }
    } else {
        printf("Error en los argumentos\n");
        printf("Uso: %s [-c|-d] -o [fichero]\n", argv[0]);
        return -1;
    }

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