#include "../utiles/utils.h"

int64_t calculate_subkeys(int64_t key);

void split_key(int64_t key, int32_t *left, int32_t *right);

int32_t rotate_key_round(int32_t key, int round);

int64_t merge_and_permute_key(int32_t left, int32_t right);

int main(int argc, char *argv[]) {
    
    /*// Test calculate_subkeys
    int64_t key = 0x133457799BBCDFF1;
    int64_t subkey = calculate_subkeys(key);
    printf("Subkey: 0x%lx\n", subkey);*/

    // Test split_key
    /*int64_t key = 0xf0ccaaf556678f;
    int32_t left, right;
    (void) split_key(key, &left, &right);
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

    

    return 0;

}

int64_t calculate_subkeys(int64_t key) {

    int64_t key_permuted = 0;
    for (int i = 0; i < BITS_IN_PC1; i++) {
        key_permuted |= (key >> (64 - PC1[i])) & 1;
        key_permuted <<= 1;
    }

    key_permuted >>= 1;

    return key_permuted;

}

void split_key(int64_t key, int32_t *left, int32_t *right) {

    *left = key >> 28;
    *right = key & 0x0FFFFFFF;
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