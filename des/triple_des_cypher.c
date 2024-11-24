/**
 * @file triple_des_cypher.c
 * @author Nicolas Victorino && Ignacio Nunnez
 * @brief 
 * @version 0.1
 * @date 2024-11-23
 * 
 * @copyright Copyright (c) 2024
 * 
 */

#include "des.h"

/**
 * @brief check the entry arguments
 * 
 * @param argc argc
 * @param argv argv
 * @param mode CYPHER or DECYPHER
 * @param key 192 bits key
 * @param input_file file to cypher
 * @param output_file file to write the cyphered data
 * @return int -1 if the arguments are not correct
 */
int check_args(int argc, char *argv[], int* mode, char* key, char** input_file, char** output_file);

/**
 * @brief Print the help for the arguments
 * 
 */
void print_args_help();

/**
 * @brief Cypher the data
 * 
 * @param fd file descriptor to read
 * @param fd_out file descriptor to write
 * @param initial_key initial key
 * @return int 0 if everything is ok
 */
int cifrar(int fd, int fd_out, char* initial_key);

/**
 * @brief Decypher the data
 * 
 * @param fd file descriptor to read
 * @param fd_out file descriptor to write
 * @param initial_key initial key
 * @return int 0 if everything is ok
 */
int descifrar(int fd, int fd_out, char* initial_key);

int main(int argc, char *argv[])
{

    int mode;
    char key[49];
    char* input_file = NULL, *output_file = NULL;

    if (check_args(argc, argv, &mode, key, &input_file, &output_file) == -1) {
        print_args_help();
        exit(-1);
    }

    int fd = open(input_file, O_RDONLY);
    if (fd < 0) {
        printf("Error opening file\n");
        exit(-1);
    }

    int fd_out = open(output_file, O_WRONLY | O_CREAT, 0666);
    if (fd_out < 0) {
        printf("Error opening file\n");
        exit(-1);
    }

    if(mode == CYPHER){
        return cifrar(fd, fd_out, key);
    }
    else{
        return descifrar(fd, fd_out, key);
    }

}

/* Cyphring file function */
int cifrar(int fd, int fd_out, char* initial_key) { 

    // Obtain the keys
    char key1[17], key2[17], key3[17];

    strncpy(key1, initial_key, 16); key1[16] = '\0';
    strncpy(key2, initial_key+16, 16); key2[16] = '\0';
    strncpy(key3, initial_key+32, 16); key3[16] = '\0';

    uint64_t keys1[DES_NUM_ROUNDS], keys2[DES_NUM_ROUNDS], keys3[DES_NUM_ROUNDS];

    calculate_subkeys(strtoul(key1, NULL, 16), keys1);
    calculate_subkeys(strtoul(key2, NULL, 16), keys2);
    calculate_subkeys(strtoul(key3, NULL, 16), keys3);

    uint64_t block;

    size_t file_size = lseek(fd, 0, SEEK_END);

    lseek(fd, 0, SEEK_SET);
    lseek(fd_out, 0, SEEK_SET);

    int filling_bytes = (file_size) % 8;

    uint64_t data;

    while (read(fd, &data, 8) == 8) {
        block = triple_des_cypher(data, 0, keys1, keys2, keys3);
        write(fd_out, &block, 8);
        data = 0;
    }

    block = triple_des_cypher(data, 0, keys1, keys2, keys3);
    write(fd_out, &block, 8);

    write(fd_out, &filling_bytes, 1);
    
    close(fd);
    close(fd_out);

    return 0;
}

/* Decyphring file function */
int descifrar(int fd, int fd_out, char* initial_key) {

    // Obtain the keys
    char key1[17], key2[17], key3[17];

    strncpy(key1, initial_key, 16); key1[16] = '\0';
    strncpy(key2, initial_key+16, 16); key2[16] = '\0';
    strncpy(key3, initial_key+32, 16); key3[16] = '\0';

    uint64_t keys1[DES_NUM_ROUNDS], keys2[DES_NUM_ROUNDS], keys3[DES_NUM_ROUNDS];

    calculate_subkeys(strtoul(key1, NULL, 16), keys1);
    calculate_subkeys(strtoul(key2, NULL, 16), keys2);
    calculate_subkeys(strtoul(key3, NULL, 16), keys3);

    uint8_t filling_bytes;
    uint64_t block;
    int tt = 0;

    // Read last byte of the file
    lseek(fd, -1 -tt, SEEK_END);
    read(fd, &filling_bytes, 1);

    // Remove last byte with the filling
    ftruncate(fd, lseek(fd, 0, SEEK_END) - 1);

    uint64_t data;

    lseek(fd, 0, SEEK_SET);
    lseek(fd_out, 0, SEEK_SET);

    while (read(fd, &data, 8) == 8) {
        block = triple_des_decypher(data, 0, keys1, keys2, keys3);
        write(fd_out, &block, 8);
    }

    // Remove filling
    if (filling_bytes != 0) {
        ftruncate(fd_out, lseek(fd_out, 0, SEEK_END) - (8 - filling_bytes));
    }

    close(fd);
    close(fd_out);

    return 0;
}

/* Check the arguments */
int check_args(int argc, char *argv[], int* mode, char* key, char** input_file, char** output_file)
{
    if(argc != 8 && argc != 6){
        return -1;
    }

    if(strcmp(argv[1], "-C") == 0){
        *mode = CYPHER;
    }
    else if (strcmp(argv[1], "-D") == 0){
        *mode = DECYPHER;
    }
    else{
        printf("Mode not recognized\n");
        return -1;
    }

    if(strcmp(argv[2], "-k") != 0){
        printf("Key not recognized\n");
        return -1;
    }

    if(is_hex(argv[3]) == -1){
        printf("Key is not a hexadecimal number\n");
        return -1;
    }

    if(strlen(argv[3]) != 48){
        /* Rearranges the string with 0s at the beggining */
        if(strlen(argv[3]) < 48){
            int i = 0;
            for(i = 0; i < 48 - strlen(argv[3]); i++){
                key[i] = '0';
            }
            strncpy(key + i, argv[3], strlen(argv[3]));
            key[48] = '\0';
        }
        else{
            printf("Key is too long\n");
            return -1;
        }

    }
    else{
        strcpy(key, argv[3]);
    }

    printf("Key: %s\n", key);

    if(strcmp(argv[4], "-i") != 0){
        printf("Input file not recognized\n");
        return -1;
    }

    *input_file = argv[5];

    if(argc == 8){
        if(strcmp(argv[6], "-o") != 0){
            printf("Output file not recognized\n");
            return -1;
        }

        *output_file = argv[7];
    }
    else{
        *output_file = malloc(sizeof(char)*(strlen(*input_file) + 11));
        strcpy(*output_file, "cyphered_");
        strcat(*output_file, *input_file);
    }

    return 0;
}

/* Print the help */
void print_args_help()
{
    printf("Usage: ./triple_des_cypher [-C | -D] -k key -i input_file [-o output_file]\n");
}
