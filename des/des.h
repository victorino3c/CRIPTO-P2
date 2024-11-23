/**
 * @file des.h
 * @author Nicolas Victorino && Ignacio Nunnez
 * @brief 
 * @version 0.1
 * @date 2024-11-22
 * 
 * @copyright Copyright (c) 2024
 * 
 */

#ifndef DES_H
#define DES_H

#include "../utiles/utils.h"

/**
 * @brief Cypher a block with the DES algorithm mode CBC
 * 
 * @param block Block to be cyphered
 * @param cbc_block Initial vector for the CBC mode (IV)
 * @param subkeys Subkeys of the algorithm
 * @return uint64_t Block cyphered
 */
uint64_t des_cypher(uint64_t block, uint64_t cbc_block, uint64_t *subkeys);

/**
 * @brief Decypher a block with the DES algorithm mode CBC
 * 
 * @param block Block to be decyphered
 * @param cbc_block Initial vector for the CBC mode (IV)
 * @param subkeys Subkeys generated in the cyphering
 * @return uint64_t Block decyphered
 */
uint64_t des_decypher(uint64_t block, uint64_t cbc_block, uint64_t *subkeys);

/**
 * @brief Triple DES cypher
 * 
 * @param block Block to cypher
 * @param cbc_block Initial vector for the CBC mode (IV)
 * @param keys1 First set of keys
 * @param keys2 Second set of keys
 * @param keys3 Third set of keys
 * 
 * @return uint64_t Cyphered block
 */
uint64_t triple_des_cypher(uint64_t block, uint64_t cbc_block, uint64_t *keys1, uint64_t *keys2, uint64_t *keys3);

/**
 * @brief Triple DES decypher
 * 
 * @param block Block to decypher
 * @param cbc_block Initial vector for the CBC mode (IV)
 * @param keys1 First set of keys
 * @param keys2 Second set of keys
 * @param keys3 Third set of keys
 * @return uint64_t Decyphered block
 */
uint64_t triple_des_decypher(uint64_t block, uint64_t cbc_block, uint64_t *keys1, uint64_t *keys2, uint64_t *keys3);

/**
 * @brief Calculate the subkeys for the DES algorithm of a given key
 * 
 * @param key Initial key
 * @param subkeys Return the subkeys generated
 */
void calculate_subkeys(uint64_t key, uint64_t *subkeys);

/**
 * @brief Encodes block cypher encryting of DES
 * 
 * @param block Block to encode
 * @param cbc_vector IV for CBC mode
 * @param subkeys Subkeys generated for the encoding
 * @param mode Whether to cypher (0) or decypher (1)
 * @return uint64_t Encoded block
 */
uint64_t encode_block(uint64_t block, uint64_t cbc_vector, uint64_t *subkeys, int mode);

/**
 * @brief Permutes the original key following PC1-des-table
 * 
 * @param key original key
 * @return uint64_t permuted key
 */
uint64_t permute_original_key(uint64_t key);

/**
 * @brief Returns the left and right parts of a 48 bits block
 * 
 * @param block block to split
 * @param left return left part (most sifnificant bits)
 * @param right return right part (less significant bits)
 */
void split_48block(uint64_t block, uint32_t *left, uint32_t *right);

/**
 * @brief Returns the left and right parts of a 64 bits block
 * 
 * @param key block to split
 * @param left return left part (most sifnificant bits)
 * @param right return right part (less significant bits)
 */
void split_64block(uint64_t block, uint32_t *left, uint32_t *right);

/**
 * @brief Merges into a 64 bits block two 32 bits blocks
 * 
 * @param left left block (most significant bits)
 * @param right right block (less significant bits)
 * @return uint64_t merged block
 */
uint64_t merge_32block(uint32_t left, uint32_t right);

/**
 * @brief Rotates a 32 bits key following the round shifts
 * 
 * @param key key to rotate
 * @param round des round 
 * @return uint32_t rotated key
 */
uint32_t rotate_key_round(uint32_t key, int round);

/**
 * @brief Merges two 28 bits blocks into a 56 bits block and permutes it following PC2-des-table
 * 
 * @param left left block (most significant bits)
 * @param right right block (less significant bits)
 * @return uint64_t merged and permuted key
 */
uint64_t merge_and_permute_key(uint32_t left, uint32_t right);

/**
 * @brief Permutes a 64 bits block following IP-des-table
 * 
 * @param text block to permute
 * @return uint64_t permuted block
 */
uint64_t ip_permutation(uint64_t text);

/**
 * @brief Permutes and extend a 32 bits block following E-des-table
 * 
 * @param right block to permute and extend
 * @return uint64_t permuted and extended block (48 bits) 
 */
uint64_t extend_and_permute_E(uint32_t right);

/**
 * @brief Substitutes a 48 bits block following S-boxes
 * 
 * @param right_extended block to substitute
 * @return uint32_t substituted block (32 bits)
 */
uint32_t s_box_substitution(uint64_t right_extended);

/**
 * @brief Permutes a 32 bits block following P-des-table
 * 
 * @param right_substituted 
 * @return uint32_t 
 */
uint32_t s_box_permutation(uint32_t right_substituted);

/**
 * @brief Permutes a 64 bits block following IP^-1-des-table
 * 
 * @param block block to permute
 * @return uint64_t permuted block
 */
uint64_t inverse_ip_permutation(uint64_t block);

/**
 * @brief If a string is a hexadecimal number
 * 
 * @param str string to check
 * 
 * @return int -1 if the string is not a hexadecimal number
 */
int is_hex(const char *str);

#endif