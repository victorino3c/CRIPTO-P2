/**
 * @file des_probador.c
 * @author Nicolas Victorino && Ignacio Nunnez
 * @brief This program only is for testing the des.h functions
 * @version 0.1
 * @date 2024-11-24
 * 
 * @copyright Copyright (c) 2024
 * 
 */

#include "des.h"

int main(int argc, char *argv[]) {

    uint64_t keys[16];

    calculate_subkeys(0xd543d6ea8c64d3b9, keys);

    printf("%lx\n", des_cypher(0xc2bd324f9e0a7706, 0x0232f2f10b980837, keys));
}