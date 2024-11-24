/**
 * @file des_avalancha.c
 * @author Nicolas Victorino && Ignacio Nunnez
 * @brief 
 * @version 0.1
 * @date 2024-11-24
 * 
 * @copyright Copyright (c) 2024
 * 
 */

#include "des.h"

/* File where to wirte the frequences of the changes */
#define DATA_FILE "data/changes.txt"

/* File where to write the gnuplot script */
#define GP_FILE "data/plotLineGraph.gp"

#define NUMBER_OF_TESTS 100000

/**
 * @brief Struct to store the mode of the program.
 *        Block mode will change one bit of the block
 *        Key mode will change one bit of the key
 * 
 */
typedef enum {
    BLOCK,
    KEY
} mode;

/**
 * @brief Modify one bit of a block
 * 
 * @param block block to modify
 * @param bit bit to modify
 * @return uint64_t modified block
 */
uint64_t modify_one_bit(uint64_t block, int bit);

/**
 * @brief Check the arguments of the program
 * 
 * @param argc number of arguments
 * @param argv arguments
 * @param block return the block to cypher
 * @param initial_key return the initial key 
 * @param num_tests return the number of tests
 * @param mode return the mode of the program
 */
void check_args(int argc, char *argv[], uint64_t *block, uint64_t *initial_key, int *num_tests, mode *mode);

int main(int argc, char *argv[]) {

    srand(time(NULL));
    
    uint64_t keys[16], keys2[16];
    uint64_t block;
    uint64_t initial_key;
    uint32_t left1 = 0, right1 = 0, left2 = 0, right2 = 0;
    double counter[DES_NUM_ROUNDS];
    int number_of_tests = NUMBER_OF_TESTS;
    mode mode;

    check_args(argc, argv, &block, &initial_key, &number_of_tests, &mode);

    FILE *data_file = fopen(DATA_FILE, "w");
    if (data_file == NULL) {
        printf("Error opening file\n");
        return -1;
    }

    for(int i = 0; i < DES_NUM_ROUNDS; i++) {
        counter[i] = 0;
    }

    for(int k=0; k < number_of_tests; k++){


        left1 = 0; left1 |= block >> 32;
        right1 = 0; right1 |= block;
        left2 = 0; left2 |= block >> 32;
        right2 = 0; right2 |= block;

        if(mode == BLOCK){
            block = modify_one_bit(block, rand() % 64);
            left2 = 0; left2 |= block >> 32;
            right2 = 0; right2 |= block;

            for(int i = 0; i < DES_NUM_ROUNDS; i++){

                do_des_round(&left1, &right1, keys[i]);
                do_des_round(&left2, &right2, keys[i]);

                counter[i] += bit_comparator_counter(left1, left2, 32) + bit_comparator_counter(right1, right2, 32);
            }

            left1 = 0; left1 |= block >> 32;
            right1 = 0; right1 |= block;
        }
        else{
            initial_key = modify_one_bit(initial_key, rand() % 64);
            calculate_subkeys(initial_key, keys2);

            for(int i = 0; i < DES_NUM_ROUNDS; i++){

                do_des_round(&left1, &right1, keys[i]);
                do_des_round(&left2, &right2, keys2[i]);

                counter[i] += bit_comparator_counter(left1, left2, 32) + bit_comparator_counter(right1, right2, 32);
            }

            for(int i = 0; i < DES_NUM_ROUNDS; i++){
                keys[i] = keys2[i];
            }
        }

    }

    for(int i = 0; i < DES_NUM_ROUNDS; i++){
        fprintf(data_file, "%d\n", 64 - (int) round(counter[i] / number_of_tests));
    }

    printf("Data generated in %s\n", DATA_FILE);

    fclose(data_file);

    generate_histogram_with_gnuplot(GP_FILE);
}

/* Function that checks the arguments of the programm */
void check_args(int argc, char *argv[], uint64_t *block, uint64_t *initial_key, int *num_tests, mode *mode) {
    if(argc != 9){
        printf("Mode not recognizeda\n");
        printf("Usage: %s -b block -k key -n num_tests -m [block | key]\n", argv[0]);
        exit(1);
    }

    if(strcmp(argv[1], "-b") == 0){
        if(strcmp(argv[2], "random") == 0){
            *block = rand64();
        }
        else if(is_hex(argv[2]) == -1){
            printf("Block is not a hexadecimal number\n");
            exit(1);
        }
 
        *block = strtoull(argv[2], NULL, 16);
    }
    else{
        printf("Mode not recognized\n");
        printf("Usage: %s -b block -k key -n num_tests -m [block | key]\n", argv[0]);
        exit(1);
    }

    if(strcmp(argv[3], "-k") == 0){
        if(strcmp(argv[4], "random") == 0){
            *initial_key = rand64();
        }
        else if(is_hex(argv[4]) == -1){
            printf("Key is not a hexadecimal number\n");
            exit(1);
        }
 
        *initial_key = strtoull(argv[4], NULL, 16);
    }
    else{
        printf("Mode not recognized\n");
        printf("Usage: %s -b block -k key -n num_tests -m [block | key]\n", argv[0]);
        exit(1);
    }

    if(strcmp(argv[5], "-n") == 0){
        *num_tests = atoi(argv[6]);
        if(*num_tests <= 0){
            printf("Number of tests must be greater than 0\n");
            exit(1);
        }
    }
    else{
        printf("Mode not recognized\n");
        printf("Usage: %s -b block -k key -n num_tests -m [block | key]\n", argv[0]);
        exit(1);
    }

    if(strcmp(argv[7], "-m") != 0){
        printf("Mode not recognized\n");
        printf("Usage: %s -b block -k key -n num_tests -m [block | key]\n", argv[0]);
        exit(1);
    }

    if(strcmp(argv[8], "key") == 0){
        *mode = KEY;
    }
    else {
        *mode = BLOCK;
    }

}

/* Function that modifies a bit of a 64 bit block */
uint64_t modify_one_bit(uint64_t block, int bit) {

    if(bit < 0 || bit > 63){
        printf("Bit out of range\n");
        exit(1);
    }

    uint64_t mask = 1;
    mask <<= bit;
    return block ^ mask;
}
