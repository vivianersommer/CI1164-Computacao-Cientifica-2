//VIVIANE DE ROSA SOMMER GRR20182564
//VINICIUS OLIVEIRA DOS SANTOS GRR20182592

#ifndef __UTILS_H__
#define __UTILS_H__

#include "utils.h"
#include <string.h>
#include <math.h>
#include <matheval.h>
#include <stdio.h>
#include <stdint.h>
#include <inttypes.h>
#include <assert.h>
#include <sys/time.h>
#include <stdlib.h>
#include <unistd.h>

#define MAX_NOME 50 //maximo de caracteres para um nome de arquivo de saida
#define MAX 300 //maximo de caracteres permitidas na linha de uma funcao passada no .dat
#define MAX_INC 6 //maximo de espaco para alocacao de incognitas deve ser 6 pois sera no maximo x4096

//cada bag carrega todos os valores de entrada e todos que devem estar na saida referentes a um bloco por vez
typedef struct{
    int max_eq; //numero maximo de equacoes
    char** eq; //matriz de strings vinda com a entrada
    double *x0; //iteracao inicial 
    double epsilon; //o epsilon para o criterios
    int max_iter; //numero maximo de iteracoes 
    double ttotal; //tempo total
    double tderivadas; //tempo das derivadas
    double tjacobiana; //tempo da jacobiana
    double tsl; //tempo da SL
    
}bag;

char *le_nome(int argc, char **argv);
void confere(FILE *arq, FILE *arq2);
double timestamp();
void clean_fgets(char *pos);
char* gera_incognitas(int max_eq, int w);
void cria_jacobiana(bag *b, char***jacobiana);
void newton(bag *b, FILE*arq2, int cont_bag);
void anali_function(bag *b, double *x, double *values, char **names, int cont_bag);
double norma_vetor(bag *b, double *x);
void analize_jacobiana_x(char*** jacobiana, double* x, char **names, int max_eq, double** values);
double *eliminacaoGauss(bag *b, double** jacobiana_x, double *invert_x);

#endif // __UTILS_H__
