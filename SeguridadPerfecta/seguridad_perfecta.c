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

typedef struct {
    int *count;
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
 * @param count array of the count of each character
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
void generateEquiprobableKey(int *a, int *b, int m);

/** 
 * @brief Generates a non equiprobable key for the affine method
 * 
 * @param a pointer to the key a
 * @param b pointer to the key b
 * @param m size of the language
 * 
 * @return void
 **/
void generateNonEquiprobableKey(int *a, int *b, int m);

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
 * @param prob_normal probability of the normal text
 * @param prob_encrypted probability of the encrypted text
 * @param key_prob key used to encrypt the text
 * 
 * @return Probabilty pointer to the conditional probability
 * 
 */
ConditionalProbabilty* conditionalProbability(Probabilty *prob_normal, Probabilty *prob_encrypted, float key_prob);

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

    // Obtain text to encrypt
    texto = handleInputText(filein);
    if (texto == NULL) {
        return 1;
    }

    // TODO: BUG DE QUE NO LIMPIA ESPACIO DESPUES DE TEXTO TRAS DESPLAZARLO
    filtrarTexto(texto);

    texto_cifrado = (char *) malloc(strlen(texto) * sizeof(char));

    mpz_t m, a, b;

    mpz_init(m);
    mpz_init(a);
    mpz_init(b);

    mpz_set_si(m, LANGUAGE_SIZE);

    char *temp = (char *) malloc(2 * sizeof(char));
    temp[1] = '\0';
    //ITERO CADA CARACTER DEL TEXTO, CIFRANDOLO CON UNA CLAVE DISTINTA Y GUARDANDOLO EN EL TEXTO CIFRADO
    for (int i = 0; i < strlen(texto); i++) {

        temp[0] = texto[i];

        if(strcmp(modo, "-P") == 0) {
            generateEquiprobableKey(&a_i, &b_i, LANGUAGE_SIZE);
        } else {
            generateNonEquiprobableKey(&a_i, &b_i, LANGUAGE_SIZE);
        }

        mpz_set_si(a, (long)a_i);
        mpz_set_si(b, (long)b_i);

        cifrar(temp, m, a, b);

        texto_cifrado[i] = temp[0];

    }

    texto_cifrado[strlen(texto)] = '\0';

    //LOGICA DE CIFRADO PERFECTO
    Probabilty *prob_original = probabilityText(texto);
    Probabilty *prob_encrypted = probabilityText(texto_cifrado);

    for (int i = 0; i < LANGUAGE_SIZE; i++) {
        if (prob_original->count[i] == 0) {
            snprintf(output + strlen(output), OUTPUT_BUFFER-strlen(output), "Pp(%c)= %.2lf\n", i + 'a', 0.00);
            continue;
        }
        snprintf(output + strlen(output), OUTPUT_BUFFER-strlen(output), "Pp(%c)= %.2lf (%d / %d)\n", i + 'a', (float)prob_original->count[i] / prob_original->total, prob_original->count[i], prob_original->total);
        //snprintf(output + strlen(output), OUTPUT_BUFFER-strlen(output), "Pp(%c)= %.2lf (%d / %d)\n", i + 'a', (float)prob_encrypted->count[i] / prob_encrypted->total, prob_encrypted->count[i], prob_encrypted->total);
    }

    ConditionalProbabilty *cond_prob = conditionalProbability(prob_original, prob_encrypted, (float)1/26);

    for (int i = 0; i < LANGUAGE_SIZE; i++) {
        for (int j = 0; j < LANGUAGE_SIZE; j++) {
            snprintf(output + strlen(output), OUTPUT_BUFFER-strlen(output), "Pp(%c|%c)=%.2lf  ", i + 'a', j + 'a', cond_prob->prob[i][j]);
        }
        snprintf(output + strlen(output), OUTPUT_BUFFER-strlen(output), "\n");
    }

    if (handleOutputText(fileout, output) == -1) {
        return 1;
    }

    free(texto);
    free(texto_cifrado);
    free(output);

    freeProbabilty(prob_original);
    freeProbabilty(prob_encrypted);
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

void generateEquiprobableKey(int *a, int *b, int m) {

    srand(time(NULL));

    int *z = (int *) malloc(sizeof(int));
    int *tam;

    do {
        *a = random_num(1, m);
        tam = euclides2(*a, m, 0, z);
    } while (tam[*z - 1] != 1);

    *b = random_num(1, m);

    free(z);
    free(tam);
}

// PREGUNTAR COMO GENERAR UNA CLAVE NO EQUIVALENTE
void generateNonEquiprobableKey(int *a, int *b, int m) {

    *a = 3;

    *b = 5;

}

Probabilty *probabilityText(char *texto) {

    Probabilty *prob = createProbabilty(LANGUAGE_SIZE, 0);
    int i = 0;

    for (i = 0; i < strlen(texto); i++) {
        prob->count[texto[i] - 'a']++;
        prob->total++;
    }

    return prob;
}

void freeProbabilty(Probabilty *prob) {
    free(prob->count);
    free(prob);
}

Probabilty *createProbabilty(int size, int total) {
    Probabilty *prob = (Probabilty *) malloc(sizeof(Probabilty));

    prob->count = (int *) calloc(sizeof(int), size);
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
        cond_prob->prob[i] = (float *) malloc(lang_size * sizeof(float));
    }

    return cond_prob;
}

ConditionalProbabilty *conditionalProbability(Probabilty *prob_normal, Probabilty *prob_encrypted, float key_prob) {

    ConditionalProbabilty *cond_prob = createConditionalProbabilty(LANGUAGE_SIZE);

    float pn = 0;
    float pe = 0;

    for (int i = 0; i < LANGUAGE_SIZE; i++) {
        for (int j = 0; j < LANGUAGE_SIZE; j++) {
            if (prob_encrypted->count[j] == 0) {
                cond_prob->prob[i][j] = 0;
                continue;
            }

            pn = (float)prob_normal->count[i]/prob_normal->total;
            pe = (float)prob_encrypted->count[j]/prob_encrypted->total;

            cond_prob->prob[i][j] = (pn * key_prob) / pe;

        }
    }

    return cond_prob;
}
