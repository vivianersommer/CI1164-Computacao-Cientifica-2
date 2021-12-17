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
void cria_jacobiana(bag *b, char* jacobiana){ 
  void *f, *f_dv;
  char *valor_x = malloc(MAX * sizeof(char));
  char *str = calloc(MAX, sizeof(char));
  char *str2 = calloc(MAX, sizeof(char));   

  for(int i=0; i<b->max_eq; i++){

    for (int j = 0; j< MAX; j++){
      str[j] = b->eq[(i*MAX) + j];
    }    

    clean_fgets(str);
    f = evaluator_create(str); //utilizamos as funções de cálculo de funções definidas pela biblioteca MATHEVAL
    assert(f);
    
    for(int j = 0; j < b->max_eq; j++){
      int coef = j+1;
      sprintf(valor_x, "x%i", coef); // incognitas = [x1, x2, x3, ..]
      f_dv = evaluator_derivative(f, valor_x); //também utilizamos essa biblioteca para calcular a derivada
      assert(evaluator_get_string(f_dv));
      str2 = evaluator_get_string(f_dv); //passamos a funcao como string pra jacobiana
      free(f_dv);
      long int tam = strlen(str2);
      for(int z = 0; z < tam; z++){
        jacobiana[(i*(b->max_eq*MAX)) + (j*MAX) + z] = str2[z];
      }
    }
    evaluator_destroy(f);
  }
  free(valor_x);
  free(str);
  free(str2);
}

//substitui nas equações originais os valores de x
void anali_function(bag *b, double *x, double *values, char **incognitas, int cont_bag){ 
  void* f;
  double val = 0;
  char *str = calloc(MAX, sizeof(char));   

  for(int i = 0; i < b->max_eq; i++){
    for (int j = 0; j< MAX; j++){
      str[j] = b->eq[(i*MAX) + j];
    }
    clean_fgets(str);
    f = evaluator_create(str); //utilizamos as funções de cálculo de funções definidas pela biblioteca MATHEVAL
    assert(f);
    val = evaluator_evaluate(f, b->max_eq, incognitas , x); 
    values[i] = val;
    evaluator_destroy(f);
  }
}

//calcula da norma do vetor como no video
double norma_vetor(bag *b, double *x){ 
  double maior = 0;
  int maximo_iteracao = 1;
  int valor_absoluto = 1;
  int j, prox_j = 0;
  for(j=0; ((maximo_iteracao) && (valor_absoluto)); j++){
    prox_j = j + 1;
    maximo_iteracao = (prox_j<b->max_eq) ? 1 : 0;
    valor_absoluto = (fabs(x[j]) > maior) ? 0 : 1;
  }
  if(fabs(x[j-1])> maior){
    maior = fabs(x[j-1]);
  }
  return maior;
}

void analize_jacobiana_x(char* jacobiana, double* x, char **incognitas, int max_eq, double* jacobiana_x){ 
 
  void *f;
  double val = 0;
  char *str = malloc((MAX)* sizeof(char));
  clean_fgets(jacobiana);

  LIKWID_MARKER_START("JACOBIANA");
  
  for(int i = 0; i < max_eq; i++){
    for(int j = 0; j < max_eq; j++){
      for (int k=0; k < MAX; k++){
        str[k] = jacobiana[(i*(max_eq*MAX)) + (j*MAX) + k];
      }
      clean_fgets(str);
      f = evaluator_create(str); //utilizamos as funções de cálculo de funções definidas pela biblioteca MATHEVAL

      assert(f);
      val = evaluator_evaluate(f, max_eq, incognitas, x); 
      jacobiana_x[(i * max_eq) + j] = val; //guardando valores resultantes do evaluate com o uso do x
      for(int z = 0; z < MAX; z++){
        jacobiana[(i*(max_eq*MAX)) + (j*MAX) + z] = str[z];
      }
      evaluator_destroy(f);
    }
  }
  free(str);

  LIKWID_MARKER_STOP("JACOBIANA");
}

//calculos para eliminacao de gaus por pivoteamento parcial na nova matriz jacobiana modificadas pelos valores de x
double *eliminacaoGauss(bag *b, double* jacobiana_x, double *invert_x){

    double *a = malloc(b->max_eq * sizeof(double));
    double *c = malloc(b->max_eq * sizeof(double));
    double *d = malloc(b->max_eq * sizeof(double));
    double *x = malloc(b->max_eq * sizeof(double));
    unsigned int iPivo = 0;
    double aux, m , s;

    LIKWID_MARKER_START("GAUSS");

    for(int i = 0; i < b->max_eq;  i++){
      d[i] = jacobiana_x[(i * b->max_eq) + i];
    }
    for(int i = 0, k=1 ; k < b->max_eq; i++, k++){
      a[i] = jacobiana_x[(k * b->max_eq) + i];
      c[i] = jacobiana_x[(i * b->max_eq) + k];
    }
    // Gauss ------------------------------------------

    for (int i=0; i < (b->max_eq - 1); ++i){
      double m = a[i]/d[i];
      a[i] = 0.0;
      d[i+1] -= (c[i] * m);
      invert_x[i+1] -= (invert_x[i] * m);
    }

    //-------------------------------------------------

    // Cálculo do valor de cada variável --------------

    x[b->max_eq -1] = invert_x[b->max_eq -1] / d[b->max_eq - 1];
    for (int i= b->max_eq-2; i >= 0; --i){
      x[i] = (invert_x[i] - c[i] * x[i+1])/d[i];
    }
    //-------------------------------------------------
    free(a);
    free(c);
    free(d);
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
    double *jacobiana_x = malloc((b->max_eq) * (b->max_eq) * sizeof(double)); //matriz jacobiana apos valores de x  
    int maximo_interacao = 1;
    int norma_vetor_values = 1;
    int norma_vetor_delta = 1;
    int prox_i = 0;
    
    // incognitas não foi otimizado para vetor, pois MATHEVAL - evaluator_evaluate espera um char ** como entrada
    char **incognitas = malloc(b->max_eq * sizeof(char*));  // incognitas = [x1, x2, x3, ..]
    int coef=0;
    for(int j=0; j< b->max_eq; j++){
      incognitas[j] = malloc(MAX_INC * sizeof(char));
    }
    for(int w=0; w<b->max_eq; w++){  
      coef = w+1;
      sprintf(incognitas[w], "x%i", coef); // incognitas = [x1, x2, x3, ..]
    }
    // -------------------------------------------------------------------------------------------------------------
    
    // COL= b->max_eq   LIN= b->max_eq  Z= MAX
    // jacobiana[ (i*(LIN*COL)) + (j*COL) + z ]
    char *jacobiana = calloc(b->max_eq * b->max_eq * MAX, sizeof(char));

    LIKWID_MARKER_START("NEWTON");
    //"falhando graciosamente"
    if(!delta || !x || !x_novo || !values || !invert_x || !jacobiana_x || !incognitas || !jacobiana){
      fprintf(stderr, "Erro ao alocar variáveis\n");
    }

    b->tderivadas = timestamp();
    LIKWID_MARKER_START("DERIVADA");
    cria_jacobiana(b, jacobiana); //crio matriz jacobiana e calculo o tempo
    LIKWID_MARKER_STOP("DERIVADA");
    b->tderivadas = timestamp() - b->tderivadas;

  //LOOP DE ITERACOES --------------------------------------------------------------------------------------------------------
    for(int i=0; (maximo_interacao && norma_vetor_values && norma_vetor_delta); i++){

      //impressao das iteracoes x
      fprintf(arq2, "#\n");
      int inter=1;
      for(int s=0; s< b->max_eq; s++){
        fprintf(arq2, "x%d = %f\n", inter , x[s]);
        inter++;
      }
    
      //utiliza valores de x nas funcoes para calcular os resultados
      anali_function(b, x, values, incognitas, cont_bag);

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

      //atualizo x antigo com seus novos valores
      for(int f=0; f<b->max_eq; f++)
        x[f] = x_novo[f];

      prox_i = i + 1;
      maximo_interacao = (prox_i<b->max_iter) ? 1 : 0;
      norma_vetor_values = norma_vetor(b, values) > b->epsilon;
      norma_vetor_delta = norma_vetor(b, delta) > b->epsilon;
    }

    //FIM DO LOOP DAS ITERACOES---------------------------------------------------------------------------
    LIKWID_MARKER_STOP("NEWTON");

    //confeir se devo seguir com a iteracao
    if(norma_vetor(b, values) < b->epsilon){
      return;
    }

    //conferir se devo seguir com a iteracao
    if(norma_vetor(b, delta)< b->epsilon){
      return;
    }

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
    free(jacobiana_x);
    free(jacobiana);

    for(int j=0; j< b->max_eq; j++){
      free(incognitas[j]);
    }

}




