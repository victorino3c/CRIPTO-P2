/**
* @file utils.c 
*
* @file utils.h 
* @brief 
* @author Nicolas Victorino && Ignacio Nunnez
* @date 24/09/2024
*/

#include "utils.h"

#define DEFALUT_TEXT_SIZE 1000

/* Function to preprocess the text to encode*/
void filtrarTexto(char *texto) {
    int i = 0;
    while (texto[i] != '\0') {
        if (texto[i] >= 'A' && texto[i] <= 'Z') {
            texto[i] = texto[i] + 32;
        } else if (texto[i] < 'a' || texto[i] > 'z') {
            /* Elimino el caracter*/
            int j = i;
            while (texto[j] != '\0') {
                texto[j] = texto[j + 1];
                j++;
            }
            /* Como he eliminado un caracter, tengo que mover el indice para atras*/
            i--;
        }
        i++;
    }
}

/* Function to get the input text*/
char * handleInputText(char *i) {

    FILE *filein;
    char *texto;

    if (i != NULL) {

        filein = fopen(i, "r");
        if (filein == NULL) {
            printf("Error al abrir el archivo de entrada\n");
            return NULL;
        }

        fseek(filein, 0, SEEK_END);
        long fsize = ftell(filein);
        fseek(filein, 0, SEEK_SET);

        texto = malloc(fsize + 1);
        fread(texto, fsize, 1, filein);
        fclose(filein);
        texto[fsize] = '\0';

    } else {
        texto = malloc(DEFALUT_TEXT_SIZE + 1);
        printf("Introduce el texto a cifrar (máximo %d caracteres): ", DEFALUT_TEXT_SIZE);
        fgets(texto, DEFALUT_TEXT_SIZE , stdin);
        texto[strlen(texto) - 1] = '\0';
    }

    return texto;
}

/* Function to handle the output of text*/
int handleOutputText(char *o, char *texto) {

    FILE *fileout;

    if (o != NULL) {
        fileout = fopen(o, "w");
        if (fileout == NULL) {
            printf("Error al abrir el archivo de salida\n");
            return -1;
        }

        fprintf(fileout, "%s", texto);
        fclose(fileout);
    } else {
        printf("%s\n", texto);
    }

    return 0;
}

/* Function to add filling to a text until its length is a multiple of n*/
char* addFilling(int n, char *texto) {
    int len = strlen(texto);
    int new_len = len + (n - (len % n));
    texto = realloc(texto, new_len + 1); // +1 para el carácter nulo
    if (texto == NULL) {
        printf("Error en la asignacion de memoria\n");
        exit(1);
    }

    for (int x = len; x < new_len; x++) {
        texto[x] = 'x';
    }
    texto[new_len] = '\0';

    return texto;
}

/*Euclides algorithm with gmp*/
mpz_t *euclides(mpz_t a, mpz_t b, int * z) {

    /* Check that a > b */
    if (mpz_cmp(a, b) < 0){
        return euclides(b, a, z);
    }

    int i = 0; 
    mpz_t temp, tempa, tempb;
    mpz_t *result = NULL;

    mpz_init(temp);
    mpz_init(tempb);
    mpz_init(tempa);

    mpz_set(tempa, a);
    mpz_set(tempb, b);

    /* Euclides loop */
    while (mpz_cmpabs_ui(b, 0) != 0) {
        
        /* We will always save memory for one more element */
        result = (mpz_t *)realloc(result, (i+2) * sizeof(mpz_t));

        if (result == NULL) {
            printf("Error en la asignacion de memoria\n");
            mpz_clear(temp);
            mpz_clear(tempa);
            mpz_clear(tempb);
            exit(1);
        }
        mpz_init(result[i]);

        mpz_tdiv_q(result[i], a, b);
        mpz_mod(temp, a, b);
        mpz_set(a, b);
        mpz_set(b, temp);

        i++;
    }

    /* Save the last element */
    mpz_init(result[i]);
    mpz_set(result[i], a);

    *z = i + 1;

    mpz_set(a, tempa);
    mpz_set(b, tempb);

    mpz_clear(temp);
    mpz_clear(tempa);
    mpz_clear(tempb);

    return result;

}

/*Euclides extended algorithm with gmp*/
mpz_t *extended_euclides(mpz_t a, mpz_t mod, int *tam) {
    int i, z;
    
    mpz_t aux;
    mpz_t *u,*v;

    mpz_t *lista = euclides(a, mod, &z);
    if (mpz_cmp_ui(lista[z - 1], 1) != 0) {
        printf("No existe inverso\n");

        for (i=0; i<z; i++) {
            mpz_clear(lista[i]);
        }
        free(lista);
        return NULL;
    }

    u = (mpz_t *)malloc(z * sizeof(mpz_t));
    if (u == NULL) {
        printf("Error en la asignacion de memoria\n");
        exit(1);
    }
    v = (mpz_t *)malloc(z * sizeof(mpz_t));
    if (v == NULL) {
        printf("Error en la asignacion de memoria\n");
        exit(1);
    }

    for (i=0; i<z; i++) {
        mpz_init(u[i]);
        mpz_init(v[i]);
    }

    mpz_init(aux);

    /*Caso base*/
    mpz_set_str(u[0], "1", 10);
    mpz_set_str(u[1], "0", 10);
    mpz_set_str(v[0], "0", 10);
    mpz_set_str(v[1], "1", 10);

    /*Caso general*/
    for (i=2; i<z; i++) {

        /* u[i] = u[i-2] - lista[i-2]*u[i-1] */
        mpz_mul(aux, lista[i-2], u[i-1]);
        mpz_sub(u[i], u[i-2], aux);

        /*v[i] = v[i-2] - lista[i-2]*v[i-1];*/
        mpz_mul(aux, lista[i-2], v[i-1]);
        mpz_sub(v[i], v[i-2], aux);
    }

    *tam = z;
    
    mpz_clear(aux);
    for (i=0; i<z; i++) {
        mpz_clear(lista[i]);
    }
    free(lista);
    
    // Que se hace con u ??? PREGUNTAR AL PROFE
    for(i=0; i<z; i++) {
        mpz_clear(u[i]);
    }
    free(u);

    return v;
}

/*Euclides algorithm with ints*/
int * euclides2(int det, int m, int n, int *z) {
    int i = 0, temp;
    int *result = NULL;

    result = (int *)malloc(1 * sizeof(int));
    if (result == NULL) {
        printf("Error en la asignacion de memoria\n");
        exit(1);
    }

    while (det != 0)   {

        result = (int *)realloc(result, (i+2) * sizeof(int));
        if (result == NULL) {
            printf("Error en la asignacion de memoria\n");
            exit(1);
        }

        result[i] = m / det;

        temp = m;
        m = det;
        det = temp % det;

        i++;
    }  

    result[i] = m;

    *z = i + 1;

    return result;
}

/*Extended euclides algorithm with ints*/
int *extended_euclides2(int det, int m, int n, int *tam) {
    int i, z;
    
    int *u,*v;

    int *lista = euclides2(det, m, n, &z);

    if (lista[z - 1] != 1) {
        printf("No existe inverso\n");
        free(lista);
        return NULL;
    }

    u = (int *)malloc(z * sizeof(int));
    if (u == NULL) {
        printf("Error en la asignacion de memoria\n");
        free(lista);
        exit(1);
    }

    v = (int *)malloc(z * sizeof(int));
    if (v == NULL) {
        printf("Error en la asignacion de memoria\n");
        free(lista);
        free(u);
        exit(1);
    }

    // Inicializar u y v
    for (i=0; i<z; i++) {
        u[i] = 0;
        v[i] = 0;
    }

    // Caso base
    u[0] = 1;
    u[1] = 0;
    v[0] = 0;
    v[1] = 1;

    // Caso general
    for (i=2; i<z; i++) {
        u[i] = u[i-2] - lista[i-2]*u[i-1];
        v[i] = v[i-2] - lista[i-2]*v[i-1];
    }

    *tam = z;

    free(lista);
    free(u);

    return v;

}

/*Function to get the greatest common divisor, only returning that value and not a whole list*/ 
int simpleMCD(int a, int b) {

    if (a < b) {
        return simpleMCD(b, a);
    }
    while (b != 0) {
        int t = b;
        b = a % b;
        a = t;
    }
    return a;
}

/*Function to find the gcd of multiple numbers*/
int find_gcd_of_list(int* numbers, int size) {
    int result = numbers[0];
    for (int i = 1; i < size; i++) {
        result = simpleMCD(result, numbers[i]);
        if (result == 1) {
            return 1;
        }
    }
    return result;
}

int random_num(int inf, int sup)
{

    int res = inf;

    /*Error control*/
    if (sup < inf)
    {
      return -1;
    }

    res += rand() % (sup - inf + 1); 
    return res;

}

void generatePermutation(int n , int *permutation) {
    int i, sust = 0, ran = 0;

    /*Seed for the random numbers*/
    srand(time(NULL));

    /*  Introducing values to the array*/
    for (i = 0; i < n; i++)
    {
      permutation[i] = i;
    }

    /*Permutations*/
    for (i = 0; i < n; i++) 
    {
      sust = permutation[i];
      ran = random_num(i, n - 1);
      if (ran == -1)
      {
        printf("Error en la generación de números aleatorios\n");
        exit(1);
      }
      permutation[i] = permutation[ran];
      permutation[ran] = sust;
    }   
}

void invertPermutation(int *k, int size) {
    int i;
    int *aux = (int *)calloc(size, sizeof(int));
    if (aux == NULL) {
        return;
    }

    for(i = 0; i < size; i++) {
        aux[k[i]] = i;
    }

    for(i = 0; i < size; i++) {
        k[i] = aux[i];
    }

    free(aux);
}

void print_histogram(int *frequencies) {
    printf("Histograma de frecuencias de bits diferentes:\n");
    for (int i = HISTOGRAM_LOWER_BOUND; i < HISTOGRAM_UPPER_BOUND; i++) {
        printf("%d | ", i);
        for (int j = 0; j < frequencies[i]; j++) {
            printf("*");
        }
        printf("\n");
    }
    printf("-|----------------------------------------------------------------------------------------------------------\n");
}

void generate_histogram_with_gnuplot(const char *script_filename) {
    char command[256];
    snprintf(command, sizeof(command), "gnuplot %s", script_filename);
    system(command);
}