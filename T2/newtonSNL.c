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

    LIKWID_MARKER_INIT;
    //ler dados de sistemas.dat -------------------------------------------------------------------------------------------------------------
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
        bag *b = malloc(sizeof(bag)); //declaracao de ponteiro para a estrutura contendo variaveis de acordo com formato proposto

        b->ttotal = 0;
        b->tjacobiana = 0;
        b->tsl = 0;
        b->tderivadas = 0;

        b->ttotal = timestamp();


        // b->max_eq ------------------------------------------------------------------------------------------------------------------------
        fscanf(arq, "%i", &b->max_eq);
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
        b->x0 = malloc((b->max_eq) * sizeof(double)); 
        for(int i=0; i<b->max_eq; i++){
            fscanf(arq,"%le",&b->x0[i]);
         }
        // -------------------------------------------------------------------------------------------------------------------------------------

        // b->epsilon --------------------------------------------------------------------------------------------------------------------------
        fscanf(arq, "%le", &b->epsilon);
        // -------------------------------------------------------------------------------------------------------------------------------------

        // b->max_iter -------------------------------------------------------------------------------------------------------------------------
        fscanf(arq, "%i", &b->max_iter);
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
            evaluator_destroy(f);
        }
        // -------------------------------------------------------------------------------------------------------------------------------------

        // Método de Newton. -------------------------------------------------------------------------------------------------------------------
        newton(b, arq2, cont_bag);
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

        char aux[10];
        fgets(aux, 10, arq);
        fgetc(arq); // ler \n que sobrou

        //free em todo mundo
        free(b->eq);
        free(b);
        cont_bag++; 
    }
    // -----------------------------------------------------------------------------------------------------------------------------------------

    //fechar arquivos
    LIKWID_MARKER_CLOSE;
    fclose(arq);
    fclose(arq2);
    
}
