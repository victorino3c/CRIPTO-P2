
#include "../utiles/utils.h"

#define INITIAL_ELEM_HIGH 0x123456789ABCDEF0 // upper 64 bits
#define INITIAL_ELEM_LOW 0x123456789ABCDEF0 // lower 64 bits

#define NUMBER_OF_ROUNDS 1
#define NUMBER_OF_TESTS 1024

#define ELEM_SIZE_BYTE 16 // size in bytes of the 128 bits elem

#define OUTPUT_FILE "data/frequencies.txt"
#define GP_FILE "data/plotHistogram.gp"
#define DEFAULT_OUTPUT_MODE "o"

/**
 * @brief xor between both entries. if 2 bits are different -> 1, if they are the same -> 0
 * 
 * @param a 128 bits variable
 * @param b 128 bits variable
 * 
 * @return the xor between both entries (128 bits with differences) 
 */
__int128_t get_128elem_difference(__int128_t a, __int128_t b);

/**
 * @brief get the number of different bits between two elems
 * 
 * @param a 128 bits variable
 * @param b 128 bits variable
 * 
 * @return the number of different bits between both elems
 */
int get_128elem_difference_number(__int128_t a, __int128_t b);

/**
 * @brief Transform a 128 bits elem to an array of 16 bytes
 * 
 * @param elem 128 bits elem
 * 
 * @return an array of 16 bytes
 */
uint8_t *from128to8(__int128_t elem);

/**
 * @brief Transform a 128 bits elem using the S-boxes of AES
 * 
 * @param elem_bytes 16 bytes array
 * 
 * @return the transformed 128 bits elem
 */
__int128_t direct_box_transformation(uint8_t *elem_bytes);

/**
 * @brief Randomly change a 128 bits elem
 * 
 * @param elem 128 bits elem
 * 
 * @return the changed 128 bits elem
 */
__int128_t random_change_128(__int128_t elem);

/**
 * @brief Change one bit of a 128 bits elem
 * 
 * @param elem 128 bits elem
 * 
 * @return the changed 128 bits elem
 */
__int128_t one_bit_change_128(__int128_t elem);

/**
 * @brief Apply a given number of rounds (NUMBER_OF_ROUNDS macro) of S-boxes to a 128 bits elem
 * 
 * @param elem 128 bits elem
 * 
 * @return the transformed 128 bits elem
 */
__int128_t sboxesrounds(__int128_t elem);

/**
 * @brief Generate the frequencies of the different bits
 * 
 * @param frequencies array of frequencies
 * @param different_bits array of number of different bits in each test
 */
void generate_difference_frequencies(int *frequencies, int *different_bits);

/**
 * @brief Print the frequencies in the output file
 * 
 * @param frequencies array of frequencies
 */
void print_frequencies_in_output(int *frequencies, FILE *output);

int main(int argc, char *argv[]) {

    char *mode = NULL;
    FILE *output = NULL;

    /*Check entry parameters*/
    if (argc > 1) {
        if (strcmp(argv[1], "-t") == 0) {
            mode = "t";
        } else if (strcmp(argv[1], "-o") == 0) {
            mode = "o";
            output = fopen(OUTPUT_FILE, "w");
        } else if (strcmp(argv[1], "-h") == 0) {
            printf("Usage: %s [-t | -o]\n", argv[0]);
            printf("Options:\n");
            printf("\t-t\tPrint the histogram in the console\n");
            printf("\t-o\tGenerate the histogram in aes/histogram.png\n");
            return 0;
        } else {
            printf("Modo no reconocido\n");
            return -1;
        }
    } else { //Default mode
        mode = DEFAULT_OUTPUT_MODE;
        output = fopen(OUTPUT_FILE, "w");
    }

    /*Set rand seed*/
    srand(time(NULL));
    
    __int128_t base_entry = ((__int128_t) INITIAL_ELEM_HIGH << 64) | INITIAL_ELEM_LOW;
    __int128_t base_exit = 0, exit = 0, entry = base_entry;

    int different_bits[NUMBER_OF_TESTS];
    int frequencies[ELEM_SIZE_BYTE * 8];

    /*Transform base_entry using S-boxes*/
    base_exit = sboxesrounds(base_entry);

    for (int i = 0; i < NUMBER_OF_TESTS; i++) {
        entry = random_change_128(entry);
        exit = sboxesrounds(entry);
        different_bits[i] = get_128elem_difference_number(base_exit, exit);
    }

    /*Calculate the frequencies of the different bits*/
    generate_difference_frequencies(frequencies, different_bits);

    if (mode != NULL) {
        if (strcmp(mode, "t") == 0) {
            print_histogram(frequencies);
        } else if (strcmp(mode, "o") == 0) {
            print_frequencies_in_output(frequencies, output);
            fclose(output);
            generate_histogram_with_gnuplot(GP_FILE);
        }
    }

    return 0;

}

__int128_t get_128elem_difference(__int128_t a, __int128_t b) {
    return a ^ b;
}

int get_128elem_difference_number(__int128_t a, __int128_t b) {
    __int128_t difference = (__int128_t)get_128elem_difference(a, b);
    int count = 0; 
    
    for(int i = 0; i < ELEM_SIZE_BYTE * 8; i++) {
        count += (difference & 1);
        difference = difference >> 1;
    }

    return count;

}

uint8_t *from128to8(__int128_t elem) {
    uint8_t *elem_bytes = (uint8_t *)malloc(ELEM_SIZE_BYTE * sizeof(uint8_t));

    for (int i = 0; i < ELEM_SIZE_BYTE; i++) {
        elem_bytes[ELEM_SIZE_BYTE - 1 - i] = elem & 0xFF; //Get the last byte
        elem = elem >> 8; //Shift to the right
    }

    return elem_bytes;
}

__int128_t direct_box_transformation(uint8_t *elem_bytes) {
    __int128_t base_exit = 0;

    /*Use the S-box to transform the entry*/
    for (int i = 0; i < ELEM_SIZE_BYTE; i++) {
        int row = elem_bytes[i] >> 4;
        int column = elem_bytes[i] & 0x0F;

        //printf("Row: %d, Column: %d, Value: %s\n", row, column, DIRECT_SBOX[row][column]);

        uint8_t sbox_value = (uint8_t)strtol(DIRECT_SBOX[row][column], NULL, 16);

        base_exit = (base_exit << 8) | sbox_value;
    }  

    return base_exit;
}

__int128_t random_change_128(__int128_t elem) {
    __int128_t change = 0;

    for (int i = 0; i < 128; i++) {
        change = change << 1;
        change = change | (rand() % 2);
    }

    return elem ^ change;
}

__int128_t one_bit_change_128(__int128_t elem) {
    __int128_t change = 1;
    change = change << rand() % 128;

    return elem ^ change;
}

__int128_t sboxesrounds(__int128_t elem) {
    __int128_t elem_exit = elem;

    for (int i = 0; i < NUMBER_OF_ROUNDS; i++) {
        uint8_t *elem_bytes = from128to8(elem_exit);
        elem_exit = direct_box_transformation(elem_bytes);
        free(elem_bytes);
    }

    return elem_exit;
}

void generate_difference_frequencies(int *frequencies, int *different_bits) {

    for (int i = 0; i < ELEM_SIZE_BYTE * 8; i++) {
        frequencies[i] = 0;
    }

    for (int i = 0; i < NUMBER_OF_TESTS; i++) {
        frequencies[different_bits[i]]++;
    }
}

void print_frequencies_in_output(int *frequencies, FILE *output) {
    for (int i = 0; i < ELEM_SIZE_BYTE * 8; i++) {
        //fprintf(output, "%d %d\n", i, frequencies[i]);
        fprintf(output, "%d\n", frequencies[i]);
    }
}