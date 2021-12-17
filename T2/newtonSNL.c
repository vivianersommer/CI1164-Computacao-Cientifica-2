//VIVIANE DE ROSA SOMMER GRR20182564
//VINICIUS OLIVEIRA DOS SANTOS GRR20182592

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "utils.h"
#include <inttypes.h>
#include <assert.h>
#include <matheval.h>
#include <likwid.h>

int main (int argc, char **argv){

    //ler dados de sistemas.dat -------------------------------------------------------------------------------------------------------------
    LIKWID_MARKER_INIT;
    FILE *arq, *arq2;
    arq = stdin;

    char* output = malloc(MAX_NOME * sizeof(char)); // reservo espaço para um nome de ate 100 letras
    output = le_nome(argc, argv);

    if (output == NULL){
		arq2 = stdout; //caso nao tenha sido passado um nome, pegue da saida padrao
    } else{
        arq2 = fopen(output, "w"); //Crio arquivo
    }
    confere(arq, arq2);

    int cont_bag = 0;
    while(!feof(arq)){
        printf("ENTREI PARA BAG %i\n", cont_bag);
        bag *b = malloc(sizeof(bag)); //declaracao de ponteiro para a estrutura contendo variaveis de acordo com formato proposto

        b->ttotal = 0;
        b->tjacobiana = 0;
        b->tsl = 0;
        b->tderivadas = 0;

        b->ttotal = timestamp();


        // b->max_eq ------------------------------------------------------------------------------------------------------------------------
        /*char *max_eq = malloc(MAX_NOME * sizeof(char));
        fgets(max_eq, MAX_NOME, arq); 
        clean_fgets(max_eq);
        b->max_eq = atoi(max_eq);
        free(max_eq);*/
        fscanf(arq, "%i\n", &b->max_eq);
        printf("MAXIMO: %i\n", b->max_eq);
        // ----------------------------------------------------------------------------------------------------------------------------------

        // b->eq ----------------------------------------------------------------------------------------------------------------------------
        b->eq = malloc ((b->max_eq+1) * MAX * sizeof(char)); 
        for(int i=0; i<b->max_eq; i++){      
            for(int j=0; j<MAX; j++){
                b->eq[(i*MAX) + j] = ' ';
            }
        }
        char *str2 = malloc(MAX * sizeof(char));
        for(int i=0; i<=b->max_eq -1; i++){      
            char ch;      

            fgets(str2, MAX, arq);
            for(int j=0; j<MAX; j++){
                b->eq[(i*MAX) + j] = str2[j];
            }

            //para garantir que vai ser puxada uma linha nao vazia garantindo q nao venha lixo
            if(strlen(b->eq) > 0){
                ch = b->eq[(i*MAX)];
            }

            // analiza se foi feita a leitura de string inválida, confere se nao eh "\r" ou "\n"
            if(ch == 13 || ch == 10){ 
                fgets(str2, MAX, arq);
                for(int j=0; j<MAX; j++){
                    b->eq[(i*MAX) + j] = str2[j];
                }
            }
        }
        free(str2);
        clean_fgets(b->eq);

        // -------------------------------------------------------------------------------------------------------------------------------------


        // b->x0 -------------------------------------------------------------------------------------------------------------------------------
        b->x0 = malloc((b->max_eq+1) * sizeof(double)); 
        char* x0 = malloc((b->max_eq*7) * sizeof(char));

        fgets(x0, (b->max_eq*7), arq);

        if(x0 == NULL){
            fgets(x0, (b->max_eq*7), arq);
        }

        char **tokens;
        int count, i;
        const char *str = x0;

        count = split(str, ' ', &tokens);
        for (i = 0; i < count; i++){
            b->x0[i] = atof(tokens[i]);
            free(tokens[i]);
        }
        free(x0);
        // -------------------------------------------------------------------------------------------------------------------------------------

        // b->epsilon --------------------------------------------------------------------------------------------------------------------------
        /*char *ep = malloc(30 * sizeof(char));
        fgets(ep, 30, arq);
        b->epsilon = atof(ep);*/
        fscanf(arq, "%le\n", &b->epsilon);
        printf("EPSILON = %f\n", b->epsilon);
        // -------------------------------------------------------------------------------------------------------------------------------------

        // b->max_iter -------------------------------------------------------------------------------------------------------------------------
        char *max_iter = malloc(MAX_NOME * sizeof(char));
        fgets(max_iter, MAX_NOME, arq); //ler do arquivo dat maximo de equacoes possiveis
        clean_fgets(max_iter);
        b->max_iter = atoi(max_iter);
        free(max_iter);
        printf("ITERACAO = %i\n", b->max_iter);
        // -------------------------------------------------------------------------------------------------------------------------------------

         //"falhando graciosamente"
        if(!b->x0 || !b->eq){
            fprintf(stderr, "Erro ao alocar variáveis\n");
        }

        // Prints ------------------------------------------------------------------------------------------------------------------------------
        fprintf(arq2,"%d\n", b->max_eq);
        void *f;
        for(int i = 0; i < b->max_eq; i++){
            char* str = malloc(MAX * sizeof(char));
            for (int j = 0; j< MAX; j++){
                str[j] = b->eq[(i*MAX) + j];
            }
            clean_fgets(str);
            f = evaluator_create(str);
            assert(f);
            fprintf(arq2,"%s = 0\n", evaluator_get_string(f));
        }

        evaluator_destroy(f);
        // -------------------------------------------------------------------------------------------------------------------------------------

        // Método de Newton. -------------------------------------------------------------------------------------------------------------------
        printf("ENTREI NO NEWTON\n");
        newton(b, arq2, cont_bag);
        printf("SAI DO NEWTON\n");
        b->ttotal = timestamp() - b->ttotal;

        // -------------------------------------------------------------------------------------------------------------------------------------
        // Print tempos ------------------------------------------------------------------------------------------------------------------------
        fprintf(arq2, "###########\n");
        fprintf(arq2, "# Tempo Total: %f\n", b->ttotal);
        fprintf(arq2, "# Tempo Derivadas: %f\n", b->tderivadas);
        fprintf(arq2, "# Tempo Jacobiana: %f\n", b->tjacobiana);
        fprintf(arq2, "# Tempo SL: %f\n", b->tsl);   
        fprintf(arq2, "###########\n");   
        fprintf(arq2, "\n");     
        // -------------------------------------------------------------------------------------------------------------------------------------

        fgetc(arq); // ler \n que sobrou

        //free em todo mundo
        free(b->eq);
        free(b);
        printf("SAI PARA BAG %i\n", cont_bag);
        cont_bag++; 
    }
    // -----------------------------------------------------------------------------------------------------------------------------------------

    //fechar arquivos
    fclose(arq);
    fclose(arq2);
    LIKWID_MARKER_CLOSE;
    return 0;
}
