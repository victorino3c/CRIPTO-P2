/**
* @file seguridad_perfecta.c 
*
* @brief This files contents function and main function for an perfect security cypher check
* @author Nicolas Victorino && Ignacio Nunnez
* @date 3/11/2024
*
* @copyright 2024
*/

//IMPORTANTE VER COMO CALCULAR LAS LLAVES NO EQUIPROBABLES Y COMO CALCULAR LAS PROBABILIDAD DE CADA LLAVE (SE PUEDE PONER UNA PROBABILIDAD FIJA?)

#include "../afin/afin.h"

#define LANGUAGE_SIZE 26
#define OUTPUT_BUFFER 4096 * 4
#define DEFAULT_KEYS_SIZE 32

typedef struct {
    int *keys;
    int size;
} Keys;

typedef struct {
    int *absoluta;
    float *relativa;
    int total;
} Probabilty;

typedef struct {
    float **prob;
} ConditionalProbabilty;

/** 
 * @brief Prints the information of the program for its execution
 * 
 * @return void
 **/
void printExeInfo();

/** 
 * @brief Function to create a probabilty struct given the lenguage size and the intial total of characters
 * 
 * @param size size of the language
 * @param total total of characters
 * 
 * @return Probability pointer to the probabilty struct
 **/
Probabilty *createProbabilty(int size, int total);

/** 
 * @brief Frees the memory of the probabilty struct
 * 
 * @param prob pointer to the probabilty struct
 * 
 * @return void
 **/
void freeProbabilty(Probabilty *prob);

/** 
 * @brief Generates a equiprobable key for the affine method
 * 
 * @param a pointer to the key a
 * @param b pointer to the key b
 * @param m size of the language
 * 
 * @return void
 **/
void generateEquiprobableKey(Keys *keys, int *a, int *b);

/** 
 * @brief Generates a non equiprobable key for the affine method
 * 
 * @param a pointer to the key a
 * @param b pointer to the key b
 * @param m size of the language
 * 
 * @return void
 **/
void generateNonEquiprobableKey(Keys *keys, int *a, int *b);

/**
 * @brief Calculates the probability of each character in the language in the encrypted text
 */
Probabilty* probabilityText(char *texto);

/**
 * @brief Frees the memory of the conditional probability struct
 * 
 * @param cond_prob pointer to the conditional probability struct
 * 
 * @return void
 */
void freeConditionalProbabilty(ConditionalProbabilty *cond_prob);

/**
 * @brief Creates a conditional probability struct
 * 
 * @param lang_size size of the language
 * 
 * @return ConditionalProbabilty pointer to the conditional probability struct
 */
ConditionalProbabilty *createConditionalProbabilty(int lang_size);

/**
 * @brief Calculates the conditional probability 
 * 
 * @param texto pointer to the original text
 * @param texto_cifrado pointer to the encrypted text
 * 
 * @return Probabilty pointer to the conditional probability
 * 
 */
ConditionalProbabilty* conditionalProbability(char *texto, char *texto_cifrado);


/**
 * @brief Generates the keys for the affine method
 * 
 * @param m size of the language
 * 
 * @return Keys pointer to the keys struct
 */
Keys *generateKeys(int m);

/**
 * @brief Generates a key for the affine method
 * 
 * @param keys pointer to the keys struct
 * @param a pointer to the key a
 * @param b pointer to the key b
 * @param mode mode of the key generation
 * 
 * @return void
 */
void generateKey(Keys *keys, int *a, int *b, char* mode);


int main(int argc, char *argv[]) {

    char *modo = NULL;
    char *filein = NULL;
    char *fileout = NULL;
    char *texto, *texto_cifrado;
    int a_i, b_i;
    char *output = (char *) malloc(OUTPUT_BUFFER * sizeof(char));
    output[0] = '\0';

    if (argc < 2 || argc > 6) {
        printExeInfo();
        return 1;
    }

    if (strcmp(argv[1], "-P") != 0 && strcmp(argv[1], "-I") != 0) {
        printExeInfo();
        return 1;
    } else {
        modo = argv[1];
    }

    if (argc == 4) {
        if (strcmp(argv[2], "-i") == 0) {
            filein = argv[3];
        } else if (strcmp(argv[2], "-o") == 0) {
            fileout = argv[3];
        } else {
            printExeInfo();
            return 1;
        }
    }

    if (argc == 6) {
        if (strcmp(argv[2], "-i") == 0) {
            filein = argv[3];
        } else {
            printExeInfo();
            return 1;
        }

        if (strcmp(argv[4], "-o") == 0) {
            fileout = argv[5];
        } else {
            printExeInfo();
            return 1;
        }
    }

    /*Generate possibles keys for the afin method*/
    Keys *keys = generateKeys(LANGUAGE_SIZE);

    /* Obtain text to encrypt*/
    texto = handleInputText(filein);
    if (texto == NULL) {
        return 1;
    }

    filtrarTexto(texto);

    texto_cifrado = (char *) malloc(strlen(texto) * sizeof(char));

    mpz_t m, a, b;

    mpz_init(m);
    mpz_init(a);
    mpz_init(b);

    mpz_set_si(m, LANGUAGE_SIZE);

    char *temp = (char *) malloc(2 * sizeof(char));
    temp[1] = '\0';
    
    /*Init the rand seed so the equiprobable keys are obtained correctly */
    srand(time(NULL));

    /* Encrypt each character with a different key, depending on the mode it will equiprobable keys or not*/
    for (int i = 0; i < strlen(texto); i++) {

        temp[0] = texto[i];

        generateKey(keys, &a_i, &b_i, modo);

        mpz_set_si(a, (long)a_i);
        mpz_set_si(b, (long)b_i);

        cifrar(temp, m, a, b);

        texto_cifrado[i] = temp[0];

    }

    texto_cifrado[strlen(texto)] = '\0';

    /*Get the probability of each character in the text*/
    Probabilty *prob_original = probabilityText(texto);

    /*Print the probabilities in the given output*/
    for (int i = 0; i < LANGUAGE_SIZE; i++) {
        if (prob_original->relativa[i] == 0) {
            snprintf(output + strlen(output), OUTPUT_BUFFER-strlen(output), "Pp(%c)= %.2lf\n", i + 'a', 0.00);
            continue;
        }
        snprintf(output + strlen(output), OUTPUT_BUFFER-strlen(output), "Pp(%c)= %.2lf (%d / %d)\n", i + 'a', prob_original->relativa[i], prob_original->absoluta[i], prob_original->total);
    }

    /*Get the conditional probability of each character in the text given its pair in the cipher text*/
    ConditionalProbabilty *cond_prob = conditionalProbability(texto, texto_cifrado);

    /*Print the conditional probabilities in the given output*/
    for (int i = 0; i < LANGUAGE_SIZE; i++) {
        for (int j = 0; j < LANGUAGE_SIZE; j++) {
            snprintf(output + strlen(output), OUTPUT_BUFFER-strlen(output), "Pp(%c|%c)=%.2lf  ", i + 'a', j + 'a', cond_prob->prob[i][j]);
        }
        snprintf(output + strlen(output), OUTPUT_BUFFER-strlen(output), "\n");
    }

    /*Output the result of the program*/
    if (handleOutputText(fileout, output) == -1) {
        return 1;
    }

    /*Free the memory*/
    free(texto);
    free(texto_cifrado);
    free(output);
    free(keys->keys);
    free(keys);

    freeProbabilty(prob_original);
    freeConditionalProbabilty(cond_prob);

    mpz_clear(m);
    mpz_clear(a);
    mpz_clear(b);


    return 0;
}

void printExeInfo() {
    printf("./seguridad_perfecta {-P|-I} [-i filein] [-o fileout]\n");
    printf("-P|-I: -P para equiprobable, -D para no equiprobable\n");
    printf("-i filein: archivo de entrada\n");
    printf("-o fileout: archivo de salida\n");
}

void generateEquiprobableKey(Keys *keys, int *a, int *b) {

    *a = keys->keys[random_num(0, keys->size-1)];
    *b = random_num(0, LANGUAGE_SIZE-1);
}

void generateNonEquiprobableKey(Keys *keys, int *a, int *b) {

    /*Asocio a cada key un peso que sea el peso anterior *2 */
    int *pesos = (int *) malloc(keys->size * sizeof(int));

    pesos[0] = 1;
    int total = 1;

    for (int i = 1; i < keys->size; i++) {
        pesos[i] = pesos[i-1] * 2;
        total += pesos[i];
    }
    
    /* Ahora uso estos pesos para elegir una clave aleatoriamente*/
    int random = random_num(0, total);

    int i = 0;
    while (random > pesos[i]) {
        random -= pesos[i];
        i++;
    }

    *a = keys->keys[i];
    *b = i;

}

Probabilty *probabilityText(char *texto) {

    Probabilty *prob = createProbabilty(LANGUAGE_SIZE, 0);
    int i = 0;

    for (i = 0; i < strlen(texto); i++) {
        prob->absoluta[texto[i] - 'a']++;
        prob->total++;
    }

    for (i = 0; i < LANGUAGE_SIZE; i++) {
        prob->relativa[i] = (float)prob->absoluta[i]/prob->total;
    }

    return prob;
}

void freeProbabilty(Probabilty *prob) {
    free(prob->absoluta);
    free(prob->relativa);
    free(prob);
}

Probabilty *createProbabilty(int size, int total) {
    Probabilty *prob = (Probabilty *) malloc(sizeof(Probabilty));

    prob->absoluta = (int *) calloc(sizeof(int), size);
    prob->relativa = (float *) calloc(sizeof(float), size);
    prob->total = total;

    return prob;
}

void freeConditionalProbabilty(ConditionalProbabilty *cond_prob) {
    for (int i = 0; i < LANGUAGE_SIZE; i++) {
        free(cond_prob->prob[i]);
    }
    free(cond_prob->prob);
    free(cond_prob);
}

ConditionalProbabilty *createConditionalProbabilty(int lang_size) {
    ConditionalProbabilty *cond_prob = (ConditionalProbabilty *) malloc(sizeof(ConditionalProbabilty));

    cond_prob->prob = (float **) malloc(lang_size * sizeof(float *));

    for (int i = 0; i < lang_size; i++) {
        cond_prob->prob[i] = (float *) calloc(lang_size, sizeof(float));
    }

    return cond_prob;
}

ConditionalProbabilty *conditionalProbability(char *texto, char *texto_cifrado) {

    ConditionalProbabilty *cond_prob = createConditionalProbabilty(LANGUAGE_SIZE);

    for (int i = 0; i < strlen(texto); i++) {
        cond_prob->prob[texto[i] - 'a'][texto_cifrado[i] - 'a']++;
    }

    Probabilty * prob_cifrado = probabilityText(texto_cifrado);

    for (int i = 0; i < LANGUAGE_SIZE; i++) {
        for (int j = 0; j < LANGUAGE_SIZE; j++) {


            cond_prob->prob[i][j] = cond_prob->prob[i][j] / prob_cifrado->absoluta[j];
        }
    }

    free(prob_cifrado);

    return cond_prob;
}

Keys *generateKeys(int m) {
    Keys *keys = (Keys *)malloc(sizeof(Keys));

    keys->keys = (int *)malloc(DEFAULT_KEYS_SIZE * sizeof(int));
    int j = 0; 

    int *z = (int *) malloc(sizeof(int)), *tam;

    for (int i = 0; i < m; i++) {
        tam = euclides2(i, m, 0, z);

        if (tam[*z - 1] == 1) {
            keys->keys[j] = i;
            j++;
        }
    }

    keys->keys = (int *) realloc(keys->keys, j * sizeof(int));
    keys->size = j;

    free(z);

    return keys;
}

void generateKey(Keys *keys, int*a, int*b, char *mode) {
    
    if (strcmp(mode, "-P") == 0) {
        generateEquiprobableKey(keys, a, b);
    } else {
        generateNonEquiprobableKey(keys, a, b);
    }
}
