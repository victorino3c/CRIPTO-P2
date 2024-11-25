/**
 * @file des_no_lineal.c
 * @author Nicolas Victorino && Ignacio Nunnez
 * @brief 
 * @version 0.1
 * @date 2024-11-24
 * 
 * @copyright Copyright (c) 2024
 * 
 */

#include "des.h"

#define DATA_FILE "data/frequencies.txt"
#define GP_FILE "data/plotHistogram.gp"

#define NUMBER_OF_TESTS 100000

#define SIZE 32

#define BLOCK_NUM 8
#define BLOCK_SIZE 6

#define NUM_ROUNDS 16

/* Mode of the graphic result*/
typedef enum {
    NORMAL,
    BAR
} mode_gr;

/* Mode of the comparation of numbers */
typedef enum {
    RANDOM,
    PARTIALLY_RANDOM,
    INCREMENTAL
} mode_num;

/**
 * @brief Change one random bit per block
 * 
 * @param key Original key
 * @param block_size Size of the block that can be changed. 
 * If it is 6 any bit of the block can be changed, if it is 1 only one bit can be changed (Will return key+1 or key-1).
 * @return uint64_t new key
 */
uint64_t change_one_bit_per_block(uint64_t key, int block_size);

/** 
 * @brief Check the arguments of the program
 * 
 * @param argc Number of arguments
 * @param argv Arguments
 * @param mode_gr Mode of the graphic result
 * @param mode_num Mode of the comparation of numbers
 * @param rep Number of repetitions
 * 
 * @return int 0 if the arguments are correct, -1 otherwise
 */
int check_args(int argc, char *argv[], mode_gr* mode_gr, mode_num* mode_num, int* rep);

int main(int argc, char *argv[]) {
    int num_attempts = NUMBER_OF_TESTS;

    uint64_t key;
    uint32_t result1, result2;
    int frequencies[SIZE];
    
    mode_gr mode_gr;
    mode_num mode_num;

    if(check_args(argc, argv, &mode_gr, &mode_num, &num_attempts) == -1){
        return -1;
    }

    FILE *data_file = fopen(DATA_FILE, "w");
    if (data_file == NULL) {
        printf("Error opening file\n");
        return -1;
    }

    for(int i = 0; i < SIZE; i++) {
        frequencies[i] = 0;
    }

    srand(time(NULL));

    for (int i = 0; i < num_attempts; i++) {

        key = rand64();
        result1 = s_box_substitution(key);

        if(mode_num == PARTIALLY_RANDOM){
            key = change_one_bit_per_block(key, BLOCK_SIZE);
        }
        else if(mode_num == INCREMENTAL){
            key = change_one_bit_per_block(key, 1);
        }
        else if(mode_num == RANDOM){
            key = rand64();
        }

        result2 = s_box_substitution(key);

        if(mode_gr == BAR){
            bit_comparator_position(result1, result2, frequencies, SIZE);
        }
        else{
            frequencies[bit_comparator_counter(result1, result2, SIZE)]++;
        }

    }

    for (int i = 0; i < SIZE; i++) {
        fprintf(data_file, "%d\n", frequencies[i]);
    }

    printf("Data generated in %s\n", DATA_FILE);

    fclose(data_file);

    generate_histogram_with_gnuplot(GP_FILE);

    return 0;
}

/* Function to check arguments */
int check_args(int argc, char *argv[], mode_gr* mode_gr, mode_num* mode_num, int* rep) {
    if(argc != 7){
        printf("Mode not recognized\n");
        printf("Usage: %s [-G normal | bar] [-N random | partially_random | incremental] [-r]\n", argv[0]);
        return -1;
    }

    if(strcmp(argv[1], "-g") == 0){
        if(strcmp(argv[2], "normal") == 0){
            *mode_gr = NORMAL;
        }
        else if(strcmp(argv[2], "bar") == 0){
            *mode_gr = BAR;
        }
        else{
            printf("Mode not recognized\n");
            printf("Usage: %s [-G normal | bar] [-N random | partially_random | incremental] [-r]\n", argv[0]);
            return -1;
        }
    }
    else{
        printf("Mode not recognized\n");
        printf("Usage: %s [-G normal | bar] [-N random | partially_random | incremental] [-r]\n", argv[0]);
        return -1;
    }

    if(strcmp(argv[3], "-n") == 0){
        if(strcmp(argv[4], "random") == 0){
            *mode_num = RANDOM;
        }
        else if(strcmp(argv[4], "incremental") == 0){
            *mode_num = INCREMENTAL;
        }
        else if(strcmp(argv[4], "partially_random") == 0){
            *mode_num = PARTIALLY_RANDOM;
        }
        else{
            printf("Mode not recognized\n");
            printf("Usage: %s [-G normal | bar] [-N random | partially_random | incremental] [-r]\n", argv[0]);
            return -1;
        }
    }
    else{
        printf("Mode not recognized\n");
        printf("Usage: %s [-G normal | bar] [-N random | partially_random | incremental] [-r]\n", argv[0]);
        return -1;
    }

    if(argc == 7){
        if(strcmp(argv[5], "-r") == 0){
            *rep = atoi(argv[6]);
        }
        else{
            printf("Mode not recognized\n");
            printf("Usage: %s [-G normal | bar] [-N random | partially_random | incremental] [-r]\n", argv[0]);
            return -1;
        }
    }

    return 0;
}

/* Function to change blocks per bit */
uint64_t change_one_bit_per_block(uint64_t key, int block_size) {
    uint64_t result = 0;
    uint64_t mask = 0x1;
    
    for(int i=0; i < BLOCK_NUM; i++) {
        result |= (mask << (i*BLOCK_SIZE + (rand() % block_size)));
    }

    return key ^ result;
}
