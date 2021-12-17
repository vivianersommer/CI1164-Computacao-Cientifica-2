//VIVIANE DE ROSA SOMMER GRR20182564
//VINICIUS OLIVEIRA DOS SANTOS GRR20182592

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
#include <likwid.h>


//algoritmo para tratamento da linha de comando, daqui abdusimos o nome do arquivo de saida passado
char *le_nome(int argc, char **argv){ 
  int option;
  while((option = getopt(argc, argv, "o: ")) != -1){		
    if(option == 'o'){   
      return optarg; //pego o nome do arquivo para escrita
    } else {
      return NULL;
    }
  }
  return NULL;
}

void confere(FILE *arq, FILE *arq2){
    if (!arq){
      perror ("Erro ao abrir arquivo de entrada") ;
      exit (1) ; // encerra o programa com status 1
    }

    if (!arq2){
      perror ("Erro ao criar arquivo de saida") ;
      exit (1) ; // encerra o programa com status 1
    }
}


//calcula tempo de execucao em milisegundos
double timestamp(){ 
  struct timeval tp;
  gettimeofday(&tp, NULL);
  return((double)(tp.tv_sec*1000.0 + tp.tv_usec/1000.0));
}

//função para "limpar" string
void clean_fgets(char *pos) { 
  strtok(pos, "\n");
}

//criacao da jacobiana utilizando as derivadas
void cria_jacobiana(bag *b, char***jacobiana){ 
  void *f, *f_dv;
  char *valor_x = malloc(MAX * sizeof(char));
   
  for(int i=0; i<b->max_eq; i++){

    clean_fgets(b->eq[i]);
    f = evaluator_create(b->eq[i]); //utilizamos as funções de cálculo de funções definidas pela biblioteca MATHEVAL
    assert(f);

    for(int j = 0; j < b->max_eq; j++){
      int coef = j+1;
      sprintf(valor_x, "x%i", coef); // incognitas = [x1, x2, x3, ..]
      f_dv = evaluator_derivative(f, valor_x); //também utilizamos essa biblioteca para calcular a derivada
      assert(evaluator_get_string(f_dv));
      jacobiana[i][j] = evaluator_get_string(f_dv); //passamos a funcao como string pra jacobiana
    }
    evaluator_destroy(f);
  }
  free(f_dv);
  free(valor_x);
}

//substitui nas equações originais os valores de x
void anali_function(bag *b, double *x, double *values, char **names, int cont_bag){ 
  void* f;
  double val = 0;
  
  for(int i = 0; i < b->max_eq; i++){
    clean_fgets(b->eq[i]);
    f = evaluator_create(b->eq[i]); //utilizamos as funções de cálculo de funções definidas pela biblioteca MATHEVAL
    assert(f);
    val = evaluator_evaluate(f, b->max_eq, names , x); 
    values[i] = val;
    evaluator_destroy(f);
  }
}

//calcula da norma do vetor como no video
double norma_vetor(bag *b, double *x){ 
  double maior = 0;
  for(int j=0; j<b->max_eq; j++){
    if(fabs(x[j])> maior){
      maior = fabs(x[j]);
    }
  }
  return maior;
}

void analize_jacobiana_x(char*** jacobiana, double* x, char **names, int max_eq, double** values){ 
  void *f;
  double val = 0;

  LIKWID_MARKER_START("JACOBIANA");

  for(int i = 0; i < max_eq; i++){
    for(int j = 0; j < max_eq; j++){
      clean_fgets(jacobiana[i][j]);
      assert(jacobiana[i][j]);
      f = evaluator_create(jacobiana[i][j]); //utilizamos as funções de cálculo de funções definidas pela biblioteca MATHEVAL
      val = evaluator_evaluate(f, max_eq, names , x); 
      values[i][j] = val; //guardando valores resultantes do evaluate com o uso do x
      evaluator_destroy(f);
    }
  }

  LIKWID_MARKER_STOP("JACOBIANA");

}

//calculos para eliminacao de gaus por pivoteamento parcial na nova matriz jacobiana modificadas pelos valores de x
double *eliminacaoGauss(bag *b, double** jacobiana_x, double *invert_x){

    double *x = malloc(b->max_eq * sizeof(double));
    unsigned int iPivo = 0;
    double aux, m , s;

    LIKWID_MARKER_START("GAUSS");
    //"falhando graciosamente"
    if(!x){
      fprintf(stderr, "Erro ao alocar variáveis\n");
    }

    // pivoteamento parcial---------------------------
    for(int i = 0; i < b->max_eq; ++i ) {
      for(int k = i+1; k < b->max_eq; ++k ) {
        iPivo = 0;
        for(int d=i+1; d < b->max_eq ; d++){
          if (abs(jacobiana_x[d][i]) > abs(jacobiana_x[i][i])){
            iPivo = d;
          }
        }
        if (i < iPivo){
          for(int r = 0; r < b->max_eq; r++){
              aux = jacobiana_x[i][r];
              jacobiana_x[i][r] = jacobiana_x[iPivo][r];
              jacobiana_x[iPivo][r] = aux;
          }

          // Troca os elementos do vetor: b
          aux = invert_x[i];
          invert_x[i] = invert_x[iPivo];
          invert_x[iPivo] = aux;
        }
      }
    }
    //-------------------------------------------------
 
    // Gauss ------------------------------------------

    for(int i=0; i < b->max_eq; ++i ) {
      for(int k=i+1; k < b->max_eq; ++k ) {
        m = jacobiana_x[k][i] / jacobiana_x[i][i];
        jacobiana_x[k][i] = 0.0;

        for( int j=i+1; j < b->max_eq; ++j ){
          jacobiana_x[k][j] =  jacobiana_x[k][j] - jacobiana_x[i][j] * m;
        }

        invert_x[k] -= invert_x[i] * m;
      }
    }

    //-------------------------------------------------

    // Cálculo do valor de cada variável --------------

    for (int i = b->max_eq - 1; i >= 0; --i) {
      s = 0;
      for (int j = i + 1; j < b->max_eq; ++j){
          s = s + jacobiana_x[i][j]*x[j];
      }
      x[i] = (invert_x[i]-s)/jacobiana_x[i][i];
    }

    //-------------------------------------------------

    LIKWID_MARKER_STOP("GAUSS");
    return x;
}

void newton (bag *b, FILE* arq2, int cont_bag){
    
    //alocacoes dinamicas para os vetores e matrizes usados
 
    double *x = b->x0; // valor calculado na iteração anterior x1,x2,x3,... (x0)
    double *delta = malloc((b->max_eq) * sizeof(double)); // valor calculado na iteração atual para x1,x2,x3,...
    double *x_novo = malloc((b->max_eq) * sizeof(double)); // x + delta
    double *values = malloc((b->max_eq) * sizeof(double)); //valores resultado das funcoes
    double *invert_x = malloc((b->max_eq) * sizeof(double)); //x invertido -f(x)

    double **jacobiana_x = malloc((b->max_eq) * sizeof(double*)); //matriz jacobiana apos valores de x  
    for(int s=0; s< b->max_eq; s++){
      jacobiana_x[s] = malloc((b->max_eq) * sizeof(double));
    }

    char **incognitas = malloc(b->max_eq * sizeof(char*));  // incognitas = [x1, x2, x3, ..]
    for(int j=0; j< b->max_eq; j++){
      incognitas[j] = malloc(MAX_INC * sizeof(char));
    }

    int coef=0;
    for(int w=0; w<b->max_eq; w++){  
      coef = w+1;
      sprintf(incognitas[w], "x%i", coef); // incognitas = [x1, x2, x3, ..]
    }

    char ***jacobiana  = (char ***) malloc(sizeof(char**) * b->max_eq); //matriz 3d jacobiana, cada posicao i,j uma string
    for(int i = 0; i < b->max_eq; i++){
      jacobiana[i] = (char **) malloc(sizeof(char*) * b->max_eq);  
      for(int j = 0; j < b->max_eq; j++){
        jacobiana[i][j] = (char *) malloc(sizeof(char) * MAX);
      }
    }

    LIKWID_MARKER_START("NEWTON");
    //"falhando graciosamente"
    if(!delta || !x || !x_novo || !values || !invert_x || !jacobiana_x || !incognitas || !jacobiana){
      fprintf(stderr, "Erro ao alocar variáveis\n");
    }

  
    b->tderivadas = timestamp();
    LIKWID_MARKER_START("DERIVADAS");
    cria_jacobiana(b, jacobiana); //crio matriz jacobiana e calculo o tempo
    LIKWID_MARKER_STOP("DERIVADAS");
    b->tderivadas = timestamp() - b->tderivadas;
 

  //LOOP DE ITERACOES --------------------------------------------------------------------------------------------------------
    for(int i=0; i<b->max_iter; i++){

      //impressao das iteracoes x
      fprintf(arq2, "#\n");
      int inter=1;
      for(int s=0; s< b->max_eq; s++){
        fprintf(arq2, "x%d = %f\n", inter , x[s]);
        inter++;
      }
      //utiliza valores de x nas funcoes para calcular os resultados
      anali_function(b, x, values, incognitas, cont_bag);
      //confeir se devo seguir com a iteracao
      if(norma_vetor(b, values) < b->epsilon){
        LIKWID_MARKER_STOP("NEWTON");
        return;
      }

      // jacobiana(x) e calculo do tempo da mesma
      double tjacobina = timestamp();
      analize_jacobiana_x(jacobiana, x, incognitas, b->max_eq, jacobiana_x);  
      b->tjacobiana = b->tjacobiana + (timestamp() - tjacobina);
      // -f(x)
      for(int m = 0; m< b->max_eq; m++){
        invert_x[m] = ((-1) * values[m]);
      }

      // jacobiana(x) * incognitas = - f(x) => SL
      double tsl = timestamp();
      delta = eliminacaoGauss(b, jacobiana_x, invert_x);
      b->tsl = b->tsl + (timestamp() - tsl);
        
      //novos x que virao de delta + x
      for(int a = 0; a<b->max_eq; a++){
        x_novo[a] = delta[a] + x[a]; //xi+1 = deltai + xi
      }

      //conferir se devo seguir com a iteracao
      if(norma_vetor(b, delta)< b->epsilon){
        LIKWID_MARKER_STOP("NEWTON");
        return;
      }

      //atualizo x antigo com seus novos valores
      for(int f=0; f<b->max_eq; f++)
        x[f] = x_novo[f];

    }
 
    
    //FIM DO LOOP DAS ITERACOES-----------------------------------------------------------------------------------------------------
    //impressao das iteracoes x finais
    int inter=1;
    fprintf(arq2,"#\n");
    for(int s=0; s< b->max_eq; s++){
      fprintf(arq2,"x%d = %f\n", inter , x[s]);
      inter++;
    } 

    //free em todo mundo
    free(b->x0);          
    free(delta);
    free(x_novo);
    free(values);
    free(invert_x);
    
    for(int i=0; i< b->max_eq; i++){
      free(jacobiana_x[i]);
    }
    free(jacobiana_x);

    for(int i=0; i< b->max_eq; i++){
      for(int j=0; j< b->max_eq; j++){
        free(jacobiana[i][j]);
      }
      free(jacobiana[i]);
    }
    free(jacobiana);

    for(int j=0; j< b->max_eq; j++){
      free(incognitas[j]);
    }
    
    LIKWID_MARKER_STOP("NEWTON");
    
}




