/**
 * @file des.c
 * @author Nicolas Victorino && Ignacio Nunnez
 * @brief 
 * @version 0.1
 * @date 2024-11-22
 * 
 * @copyright Copyright (c) 2024
 * 
 */

#include "des.h"

/******************************************************************************************/
/* Public functions of DES */
/******************************************************************************************/

/* Des cypher function */
uint64_t des_cypher(uint64_t block, uint64_t cbc_block, uint64_t *subkeys)
{

    // Encode the block by the keys (Step 2)
    return encode_block(block, cbc_block, subkeys, CYPHER);
}

/* Des decypher function */
uint64_t des_decypher(uint64_t block, uint64_t cbc_block, uint64_t *subkeys)
{

    // Revert the order of the keys
    uint64_t subkeys_reverted[16];
    for (int i = 0; i < 16; i++)
    {
        subkeys_reverted[i] = subkeys[15 - i];
    }

    return encode_block(block, cbc_block, subkeys_reverted, DECYPHER);
}

/* Auxiliar function for triple_des_cypher */
uint64_t triple_des_cypher(uint64_t block, uint64_t cbc_block, uint64_t *keys1, uint64_t *keys2, uint64_t *keys3)
{
    uint64_t block_cyphered;

    block_cyphered = des_cypher(block, cbc_block, keys1);
    block_cyphered = des_decypher(block_cyphered, cbc_block, keys2);
    block_cyphered = des_cypher(block_cyphered, cbc_block, keys3);

    return block_cyphered;
}

/* Auxiliar function for triple_des_decypher */
uint64_t triple_des_decypher(uint64_t block, uint64_t cbc_block, uint64_t *keys1, uint64_t *keys2, uint64_t *keys3)
{
    uint64_t block_decyphered;

    block_decyphered = des_decypher(block, cbc_block, keys3);
    block_decyphered = des_cypher(block_decyphered, cbc_block, keys2);
    block_decyphered = des_decypher(block_decyphered, cbc_block, keys1);

    return block_decyphered;
}

/* Function calculator of the subkeys */
void calculate_subkeys(uint64_t key, uint64_t *subkeys)
{

    // Generate the subkeys
    uint64_t permuted_key = permute_original_key(key);
    uint32_t left[DES_NUM_ROUNDS], right[DES_NUM_ROUNDS], inital_left, inital_right;
    split_48block(permuted_key, &inital_left, &inital_right);

    left[0] = rotate_key_round(inital_left, 0);
    right[0] = rotate_key_round(inital_right, 0);

    for (int i = 1; i < DES_NUM_ROUNDS; i++)
    {
        left[i] = rotate_key_round(left[i - 1], i);
        right[i] = rotate_key_round(right[i - 1], i);
    }

    for (int i = 0; i < DES_NUM_ROUNDS; i++)
    {
        subkeys[i] = merge_and_permute_key(left[i], right[i]);
    }
}

/* Function to encode a block */
uint64_t encode_block(uint64_t block, uint64_t cbc_vector, uint64_t *subkeys, int mode)
{

    if (mode == CYPHER) {
        block = block ^ cbc_vector;
    }

    uint64_t block_permuted = ip_permutation(block);

    uint32_t left_block, right_block;
    split_64block(block_permuted, &left_block, &right_block);


    for(int i=0; i<DES_NUM_ROUNDS; i++){
        do_des_round(&left_block, &right_block, subkeys[i]);
    }

    uint64_t final_block = merge_32block(right_block, left_block);

    uint64_t block_cyphered = inverse_ip_permutation(final_block);

    if (mode == DECYPHER) {
        block_cyphered = block_cyphered ^ cbc_vector;
    }

    return block_cyphered;
}

void do_des_round(uint32_t *left_block, uint32_t *right_block, uint64_t subkeys)
{

    uint64_t right_extended = extend_and_permute_E(*right_block);
    right_extended ^= subkeys;
    uint32_t right_substituted = s_box_substitution(right_extended);
    uint32_t right_permuted = s_box_permutation(right_substituted);

    uint32_t temp = *right_block;
    *right_block = *left_block ^ right_permuted;
    *left_block = temp;

}

/* Permutes original key following PC1 */
uint64_t permute_original_key(uint64_t key)
{

    uint64_t key_permuted = 0;
    for (int i = 0; i < BITS_IN_PC1; i++)
    {
        key_permuted |= (key >> (64 - PC1[i])) & 1;
        key_permuted <<= 1;
    }

    key_permuted >>= 1;

    return key_permuted;
}

/* Split 48 bits block */
void split_48block(uint64_t block, uint32_t *left, uint32_t *right)
{

    *left = block >> 28;
    *right = block & 0x000000000FFFFFFF;
}

/* Split 64 bits block */
void split_64block(uint64_t block, uint32_t *left, uint32_t *right)
{

    *left = block >> 32;
    *right = block & 0x00000000FFFFFFFF;
}

/* Merge two 32 bits blocks */
uint64_t merge_32block(uint32_t left, uint32_t right)
{

    uint64_t block = ((uint64_t)left << 32) | right;

    return block;
}

/* Rotate 32 bits key following round shifts */
uint32_t rotate_key_round(uint32_t key, int round)
{

    uint32_t new_key = key << ROUND_SHIFTS[round];

    new_key &= 0x0FFFFFFF;

    new_key |= key >> (28 - ROUND_SHIFTS[round]);

    return new_key;
}

/* Merge two 28 bits blocks into a 56 bits block and permute it following PC2 */
uint64_t merge_and_permute_key(uint32_t left, uint32_t right)
{

    uint64_t key = 0;
    uint64_t key_permuted = 0;

    key = left;
    key <<= 28;
    key |= right;

    for (int i = 0; i < BITS_IN_PC2; i++)
    {
        key_permuted |= (key >> (56 - PC2[i])) & 1;
        key_permuted <<= 1;
    }

    key_permuted >>= 1;

    return key_permuted;
}

/* Permutes block following IP-des */
uint64_t ip_permutation(uint64_t text)
{

    uint64_t text_permuted = 0;

    for (int i = 0; i < BITS_IN_IP; i++)
    {
        text_permuted |= (text >> (64 - IP[i])) & 1;

        // Shift the bits to the left, except for the last one
        if (i < BITS_IN_IP - 1)
            text_permuted <<= 1;
    }

    return text_permuted;
}

/* Extend and permute a 32 bits block following E-des */
uint64_t extend_and_permute_E(uint32_t right)
{

    uint64_t right_extended = 0;

    for (int i = 0; i < BITS_IN_E; i++)
    {
        right_extended |= (right >> (32 - E[i])) & 1;
        right_extended <<= 1;
    }

    right_extended >>= 1;

    return right_extended;
}

/* Substitute a 48 bits block following S-boxes */
uint32_t s_box_substitution(uint64_t right_extended)
{

    uint32_t right_substituted = 0;

    for (int i = NUM_S_BOXES - 1; i >= 0; i--)
    {

        int column = (right_extended >> (6 * i)) & 0x1E;
        column >>= 1;

        int row = (right_extended >> (6 * i)) & 0x21;
        if (row % 2 == 0)
        {
            row >>= 4;
        }
        else
        {
            row >>= 4;
            row++;
        }

        right_substituted |= S_BOXES[NUM_S_BOXES - 1 - i][row][column];

        if (i > 0)
            right_substituted <<= 4;
    }

    return right_substituted;
}

/* Permutes a 32 bits block following P-des */
uint32_t s_box_permutation(uint32_t right_substituted)
{

    uint32_t right_permuted = 0;

    for (int i = 0; i < BITS_IN_P; i++)
    {
        right_permuted |= (right_substituted >> (32 - P[i])) & 1;

        // Shift the bits to the left, except for the last one
        if (i < BITS_IN_P - 1)
            right_permuted <<= 1;
    }

    return right_permuted;
}

/* Permutes block following IP^-1-des */
uint64_t inverse_ip_permutation(uint64_t block)
{

    uint64_t block_permuted = 0;

    for (int i = 0; i < BITS_IN_IP; i++)
    {
        block_permuted |= (block >> (64 - IP_INV[i])) & 1;

        // Shift the bits to the left, except for the last one
        if (i < BITS_IN_IP - 1)
            block_permuted <<= 1;
    }

    return block_permuted;
}

/* Checks if a string is an hexadecimal number valid */
int is_hex(const char *str) {
    if (str == NULL || *str == '\0') {
        return -1;
    }

    for (int i = 0; str[i] != '\0'; i++) {
        char c = str[i];
        
        if (!(isdigit(c) || (c >= 'A' && c <= 'F'))) {
            return -1;
        }
    }

    return 0;
}
