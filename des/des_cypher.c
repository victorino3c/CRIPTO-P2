#include "des.h"

#define DES_IV_BLOCK 0xD48001C68F2325A1

typedef enum {
    STD_BINARY,
    JPEG,
    PNG
} type_b;

/**
 * @brief cypher using DES fd on fd_out with initial_key 
 * 
 * @param fd file descriptor to read
 * @param fd_out file descriptor to write
 * @param initial_key initial key
 * @param type_binary type of binary
 * @return int 0 if everything is ok
 */
int cifrar(int fd, int fd_out, uint64_t initial_key, type_b type_binary);

/**
 * @brief decypher using DES fd on fd_out with initial_key
 * 
 * @param fd file descriptor to read
 * @param fd_out file descriptor to write
 * @param initial_key initial key
 * @param type_binary type of binary
 * @return int 0 if everything is ok
 */
int descifrar(int fd, int fd_out, uint64_t initial_key, type_b type_binary);

/**
 * @brief Check the entry arguments
 * 
 * @param argc number of arguments
 * @param argv arguments
 * @param mode return the mode (cypher or decypher)
 * @param key return the key
 * @param input_file return the input file
 * @param output_file return the output file
 * 
 * @return int -1 if the arguments are not correct
 */
int check_args(int argc, char *argv[], int* mode, uint64_t* key, char** input_file, char** output_file);

/**
 * @brief Print the help for the arguments
 * 
 */
void print_args_help();

/**
 * @brief If a string is a hexadecimal number
 * 
 * @param str string to check
 * 
 * @return int -1 if the string is not a hexadecimal number
 */
int is_hex(const char *str);

int jpg_size_header(int fd);

int main(int argc, char *argv[])
{

    int mode;
    uint64_t initial_key;
    char* input_file = NULL, *output_file = NULL;

    if (check_args(argc, argv, &mode, &initial_key, &input_file, &output_file) == -1) {
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

    type_b type_binary;

    if(input_file[strlen(input_file) - 1] == 'g' && input_file[strlen(input_file) - 2] == 'p' && input_file[strlen(input_file) - 3] == 'j'){
        type_binary = JPEG;
    }
    else if(input_file[strlen(input_file) - 1] == 'g' && input_file[strlen(input_file) - 2] == 'n' && input_file[strlen(input_file) - 3] == 'p'){
        type_binary = PNG;
    }
    else{
        type_binary = STD_BINARY;
    }

    if(mode == CYPHER){
        return cifrar(fd, fd_out, initial_key, type_binary);
    }
    else{
        return descifrar(fd, fd_out, initial_key, type_binary);
    }

}

int cifrar(int fd, int fd_out, uint64_t initial_key, type_b type) { 

    uint64_t keys[DES_NUM_ROUNDS];
    uint64_t block;

    size_t file_size = lseek(fd, 0, SEEK_END);
    printf("File size: %ld\n", file_size);

    int filling_bytes = 8 - (file_size % 8);

    lseek(fd, 0, SEEK_SET);
    lseek(fd_out, 0, SEEK_SET);

    uint64_t data;
    uint64_t iv_block = DES_IV_BLOCK;

    calculate_subkeys(initial_key, keys);

    while (read(fd, &data, 8) == 8) {
        block = des_cypher(data, iv_block, keys);
        iv_block = block;
        write(fd_out, &block, 8);
        data = 0;
    }

    block = des_cypher(data, iv_block, keys);
    write(fd_out, &block, 8);

    write(fd_out, &filling_bytes, 1);
    
    close(fd);
    close(fd_out);

    return 0;
}

int descifrar(int fd, int fd_out, uint64_t initial_key, type_b type) {

    uint8_t filling_bytes;
    uint64_t keys[DES_NUM_ROUNDS];
    uint64_t block;

    // Read last byte of the file
    lseek(fd, -1, SEEK_END);
    read(fd, &filling_bytes, 8);

    //printf("Filling bytes: %d\n", filling_bytes);

    // Remove last byte with the filling
    ftruncate(fd, lseek(fd, 0, SEEK_END) - 1);

    lseek(fd, type, SEEK_SET);

    calculate_subkeys(initial_key, keys);

    // Revert the order of the keys
    uint64_t keys_reverted[16];
    for (int i = 0; i < 16; i++) {
        keys_reverted[i] = keys[15-i];
    }

    uint64_t data;
    uint64_t iv_block = DES_IV_BLOCK;

    while (read(fd, &data, 8) == 8) {
        block = des_decypher(data, iv_block, keys_reverted);
        iv_block = data;
        write(fd_out, &block, 8);
    }

    printf("%d Last block: %lx\n", filling_bytes, block);

    // Remove filling
    if (filling_bytes != 0) {
        ftruncate(fd_out, lseek(fd_out, 0, SEEK_END) - (filling_bytes));
    }

    close(fd);
    close(fd_out);

    return 0;
}

int check_args(int argc, char *argv[], int* mode, uint64_t* key, char** input_file, char** output_file)
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
    *key = strtoul(argv[3], NULL, 16);

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

void print_args_help()
{
    printf("Usage: des [-C | -D] -K key -I input_file [-O output_file]\n");
    printf("Output file is optional. Will use one name by the default.\n");
}

int is_hex(const char *str) {
    if (str == NULL || *str == '\0') {
        return -1; // Cadena vacía o nula no es válida.
    }

    // Recorre cada carácter de la cadena.
    for (int i = 0; str[i] != '\0'; i++) {
        char c = str[i];
        // Verifica si el carácter no está entre 0-9 ni A-F.
        if (!(isdigit(c) || (c >= 'A' && c <= 'F'))) {
            return -1;
        }
    }

    return 0; // Todos los caracteres son válidos.
}

int jpg_size_header(int fd) {

    FILE *infile = fdopen(fd, "rb");

    struct jpeg_decompress_struct cinfo;
    struct jpeg_error_mgr jerr;
    size_t header_size = 0;

    cinfo.err = jpeg_std_error(&jerr);
    jpeg_create_decompress(&cinfo);
    jpeg_stdio_src(&cinfo, infile);

    // Leer la cabecera del JPEG
    if (jpeg_read_header(&cinfo, TRUE) != JPEG_HEADER_OK) {
        fprintf(stderr, "No se pudo leer el encabezado JPEG\n");
        fclose(infile);
        return -1;
    }

    // Guardar la cabecera en un archivo separado
    header_size = ftell(infile);  // Tamaño hasta el encabezado
    printf("Header size: %ld\n", header_size);

    // Cerrar el archivo
    //fclose(infile);
    return header_size;
}