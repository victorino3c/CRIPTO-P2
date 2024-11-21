#include "../utiles/utils.h"

int64_t des_cypher(int64_t block, int64_t key, int64_t *subkeys);

int64_t des_decypher(int64_t block, int64_t *subkeys);

void calculate_subkeys(int64_t key, int64_t *subkeys);

int64_t encode_block(int64_t block, int64_t *subkeys);

int64_t permute_original_key(int64_t key);

void split_48block(int64_t block, int32_t *left, int32_t *right);

void split_64block(int64_t key, int32_t *left, int32_t *right);

int64_t merge_32block(int32_t left, int32_t right);

int32_t rotate_key_round(int32_t key, int round);

int64_t merge_and_permute_key(int32_t left, int32_t right);

int64_t ip_permutation(int64_t text);

int64_t extend_and_permute_E(int32_t right);

int32_t s_box_substitution(int64_t right_extended);

int32_t s_box_permutation(int32_t right_substituted);

int64_t inverse_ip_permutation(int64_t block);


int main(int argc, char *argv[]) {
    
    int64_t keys[DES_NUM_ROUNDS];
    int64_t block;

    block = des_cypher(0xFEDCBA9876543210, 0x133457799BBCDFF1, keys);
    //block = des_cypher(0xFEDCBA9876543210, 0x0F1571C947D9E859, keys);

    printf("Block cyphered: 0x%lx\n", block);

    for(int i = 0; i < DES_NUM_ROUNDS; i++) {
        //printf("Subkey %d: 0x%lx\n", i+1, keys[i]);
    }

    /*Revert order keys*/
    int64_t keys2[DES_NUM_ROUNDS];
    for(int i = 0; i < DES_NUM_ROUNDS; i++) {
        keys2[i] = keys[DES_NUM_ROUNDS - i - 1];
        //printf("Subkey2 %d: 0x%lx\n", i+1, keys2[i]);
    }

    block = des_decypher(block, keys2);
    //printf("Block decyphered: 0x%lx\n", block);

}

int64_t des_cypher(int64_t block, int64_t key, int64_t *subkeys) {

    // Generate the subkeys (Step 1)
    calculate_subkeys(key, subkeys);

    // Encode the block by the keys (Step 2)
    return encode_block(block, subkeys);
}

int64_t des_decypher(int64_t block, int64_t *subkeys) {

    return encode_block(block, subkeys);
}

void calculate_subkeys(int64_t key, int64_t *subkeys) {

    // Generate the subkeys
    int64_t permuted_key = permute_original_key(key);
    int32_t left[DES_NUM_ROUNDS], right[DES_NUM_ROUNDS], inital_left, inital_right;
    split_48block(permuted_key, &inital_left, &inital_right);

    //printf("0 -> Left: 0x%x\tRight: 0x%x\n", inital_left, inital_right);

    left[0] = rotate_key_round(inital_left, 0);
    right[0] = rotate_key_round(inital_right, 0);

    //printf("1 -> Left: 0x%x\tRight: 0x%x\n", left[0], right[0]);

    for (int i = 1; i < DES_NUM_ROUNDS; i++) {
        left[i] = rotate_key_round(left[i-1], i);
        right[i] = rotate_key_round(right[i-1], i);

        //printf("%d -> Left: 0x%x\tRight: 0x%x\n", i+1, left[i], right[i]);
    }

    for (int i = 0; i < DES_NUM_ROUNDS; i++) {
        subkeys[i] = merge_and_permute_key(left[i], right[i]);
        //printf("Subkey %d: 0x%lx\n", i+1, subkeys[i]);
    }
}

int64_t encode_block(int64_t block, int64_t *subkeys) {
    int64_t block_permuted = ip_permutation(block);

    //printf("Block permuted: 0x%lx\n", block_permuted);

    int32_t left_block, right_block;
    split_64block(block_permuted, &left_block, &right_block);

    //printf("Initial block -> Left: 0x%x\tRight: 0x%x\n", left_block, right_block);

    for (int i = 0; i < DES_NUM_ROUNDS; i++) {
        //printf("Right: 0x%x\n", right_block);
        int64_t right_extended = extend_and_permute_E(right_block);
        //printf("Right extended: 0x%lx\n", right_extended);
        //printf("Subkey: 0x%lx\n", subkeys[i]);
        right_extended ^= subkeys[i];
        //printf("Right extended XOR: 0x%lx\n", right_extended);
        int32_t right_substituted = s_box_substitution(right_extended);
        int32_t right_permuted = s_box_permutation(right_substituted);

        int32_t temp = right_block;
        right_block = left_block ^ right_permuted;
        left_block = temp;

        //printf("Round %d -> Left: 0x%x\tRight: 0x%x\n", i+1, left_block, right_block);
    }

    int64_t final_block = merge_32block(right_block, left_block);

    int64_t block_cyphered = inverse_ip_permutation(final_block);

    //printf("Block cyphered: 0x%lx\n", block_cyphered);

    return block_cyphered;
}

int64_t permute_original_key(int64_t key) {

    int64_t key_permuted = 0;
    for (int i = 0; i < BITS_IN_PC1; i++) {
        key_permuted |= (key >> (64 - PC1[i])) & 1;
        key_permuted <<= 1;
    }

    key_permuted >>= 1;

    return key_permuted;

}

void split_48block(int64_t block, int32_t *left, int32_t *right) {

    *left = block >> 28;
    *right = block & 0x000000000FFFFFFF;
}

void split_64block(int64_t block, int32_t *left, int32_t *right) {

    *left = block >> 32;
    *right = block & 0x00000000FFFFFFFF;
}

int64_t merge_32block(int32_t left, int32_t right) {

    int64_t block = 0;
    block = left;
    block <<= 32;
    block |= right;

    return block;
}

int32_t rotate_key_round(int32_t key, int round) {

    int32_t new_key = key << ROUND_SHIFTS[round];

    new_key &= 0x0FFFFFFF;

    new_key |= key >> (28 - ROUND_SHIFTS[round]);

    return new_key;
}

int64_t merge_and_permute_key(int32_t left, int32_t right) {

    int64_t key = 0;
    int64_t key_permuted = 0;

    key = left;
    key <<= 28;
    key |= right;

    for (int i = 0; i < BITS_IN_PC2; i++) {
        key_permuted |= (key >> (56 - PC2[i])) & 1;
        key_permuted <<= 1;
    }

    key_permuted >>= 1;

    return key_permuted;
}

int64_t ip_permutation(int64_t text) {

    int64_t text_permuted = 0;

    for (int i = 0; i < BITS_IN_IP; i++) {
        text_permuted |= (text >> (64 - IP[i])) & 1;
        
        // Shift the bits to the left, except for the last one
        if(i < BITS_IN_IP - 1)
            text_permuted <<= 1;
    }

    return text_permuted;
}

int64_t extend_and_permute_E(int32_t right) {

    int64_t right_extended = 0;

    for (int i = 0; i < BITS_IN_E; i++) {
        right_extended |= (right >> (32 - E[i])) & 1;
        right_extended <<= 1;
    }

    right_extended >>= 1;

    return right_extended;
}

int32_t s_box_substitution(int64_t right_extended) {

    int32_t right_substituted = 0;

    for (int i = NUM_S_BOXES-1; i >= 0; i--) {

        int column = (right_extended >> (6 * i)) & 0x1E;
        column >>= 1;

        int row = (right_extended >> (6 * i)) & 0x21;
        if(row % 2 == 0) {
            row >>= 4;
        }
        else {
            row >>= 4;
            row++;
        }

        //printf("Row: %d\n", row);
        //printf("Column: %d\n", column);
        //printf("S_BOX: %d\n\n", S_BOXES[NUM_S_BOXES-1-i][row][column]);

        right_substituted |= S_BOXES[NUM_S_BOXES-1-i][row][column];
        
        if(i > 0)
            right_substituted <<= 4;
    }

    return right_substituted;
}

int32_t s_box_permutation(int32_t right_substituted) {

    int32_t right_permuted = 0;

    for (int i = 0; i < BITS_IN_P; i++) {
        right_permuted |= (right_substituted >> (32 - P[i])) & 1;
        
        // Shift the bits to the left, except for the last one
        if (i < BITS_IN_P - 1)
            right_permuted <<= 1;
    }

    return right_permuted;
}

int64_t inverse_ip_permutation(int64_t block) {

    int64_t block_permuted = 0;

    for (int i = 0; i < BITS_IN_IP; i++) {
        block_permuted |= (block >> (64 - IP_INV[i])) & 1;
        
        // Shift the bits to the left, except for the last one
        if(i < BITS_IN_IP - 1)
            block_permuted <<= 1;
    }

    return block_permuted;
}

/*int test(){*/
    // Step 1

    // Test calculate_subkeys
    /*int64_t key = 0x133457799BBCDFF1;
    int64_t subkey = permute_original_key(key);
    printf("Subkey: 0x%lx\n", subkey);*/

    // Test split_key
    /*int64_t key = 0xf0ccaaf556678f;
    int32_t left, right;
    (void) split_64block(key, &left, &right);
    printf("Left: 0x%x\n", left);
    printf("Right: 0x%x\n", right);*/

    // Test rotate_key_round
    /*int32_t key = 0xF0CCAAF;
    int32_t new_key = rotate_key_round(key, 0);
    printf("New key: 0x%x\n", new_key);*/

    // Test merge_and_permute_key
    /*int32_t left = 0xE19955F;
    int32_t right = 0xAACCF1E;
    int64_t key = merge_and_permute_key(left, right);
    printf("Key: 0x%lx\n", key);*/

    // Step 2
    
    // Test ip_permutation
    /*int64_t text = 0x123456789ABCDEF;
    int64_t text_permuted = ip_permutation(text);
    printf("Text permuted: 0x%lx\n", text_permuted);*/

    // Test e_permutation
    /*int32_t right = 0xF0AAF0AA;
    int64_t right_extended = extend_and_permute_E(right);
    printf("Right extended and permuted: 0x%lx\n", right_extended);*/

    // Test s_box_substitution
    /*int64_t right_extended = 0x6117BA866527;
    int32_t right_substituted = s_box_substitution(right_extended);
    printf("Right substituted: 0x%x\n", right_substituted);*/

    // Test s_box_permutation
    /*int32_t right_substituted = 0x5C82B597;
    int32_t right_permuted = 0;
    right_permuted = s_box_permutation(right_substituted);
    printf("Right permuted: 0x%x\n", right_permuted);*/

    // Test inverse_ip_permutation
    /*int64_t block = 0xA4CD99543423234;
    int64_t block_permuted = inverse_ip_permutation(block);
    printf("Block permuted: 0x%lx\n", block_permuted);*/
/*}*/
