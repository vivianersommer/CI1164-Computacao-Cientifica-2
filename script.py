import os
import subprocess
import pandas as pd


N = [10 , 32, 50, 64] 
#100, 128, 200, 250, 256, 300] 
#400, 512, 600, 1000, 1024, 2000, 2048, 3000, 4096]

print("Compilando T1 ...")
if not os.path.exists("../vods18vrs18/T1/newtonSNL"):
    os.chdir("../vods18vrs18/T1/")
    os.system("make avx")

os.chdir("../")

print("Compilando T2 ...")
if not os.path.exists("../vods18vrs18/T2/newtonSNL"):
    os.chdir("../vods18vrs18/T2")
    os.system("make avx")

if os.path.isdir("./sl"):
    os.system("rm sl/*.dat")
    os.system("rmdir sl")

if os.path.isdir("./csvs"):
    os.system("rm csvs/*.csv")
    os.system("rmdir csvs")

os.system("mkdir csvs")
os.chdir("../")

time_normal1 = []
time_normal2 = []
time_normal3 = []
time_normal4 = []

time_otimizado1 = []
time_otimizado2 = []
time_otimizado3 = []
time_otimizado4 = []

banda_memoria_normal1 = []
banda_memoria_normal2 = []
banda_memoria_normal3 = []
banda_memoria_normal4 = []

banda_memoria_otimizado1 = []
banda_memoria_otimizado2 = []
banda_memoria_otimizado3 = []
banda_memoria_otimizado4 = []

cache_miss_normal1 = []
cache_miss_normal2 = []
cache_miss_normal3 = []
cache_miss_normal4 = []

cache_miss_otimizado1 = []
cache_miss_otimizado2 = []
cache_miss_otimizado3 = []
cache_miss_otimizado4 = []

dp_normal1 = []
dp_normal2 = []
dp_normal3 = []
dp_normal4 = []

dp_otimizado1 = []
dp_otimizado2 = []
dp_otimizado3 = []
dp_otimizado4 = []

avx_normal1 = []
avx_normal2 = []
avx_normal3 = []
avx_normal4 = []

avx_otimizado1 = []
avx_otimizado2 = []
avx_otimizado3 = []
avx_otimizado4 = []

t1 = []
t2 = []

# gera  todos os SL  -----------------------------------------------------------------------------
print("Executando broyden para gerar todos os SL ...")
os.system("chmod +x broyden.sh")
command = "./broyden.sh"
os.system(command)
#-------------------------------------------------------------------------------------------------

for value in N:

    sample_in = "../sl/sl_" + str(value) + ".dat"

    # gráfico 1 TESTE DE TEMPO------------------------------------------------------------------------
    print("Executando likwid-perfctr para N " + str(value) + " e -g L3 ...")
    
    ## COLETANDO NORMAL
    os.chdir("../vods18vrs18/T1")
    sample_out = "../sl/sample_out_l3_" + str(value)+".dat"
    os.system("likwid-perfctr -C 3 -g L3 -m ./newtonSNL -o saida_"+ str(value) +".txt < " + sample_in + " > " + sample_out)
    result = subprocess.Popen(["fgrep", " RDTSC Runtime ", sample_out], stdout=subprocess.PIPE)
    stdout,stderr = result.communicate()
    stdout = str(stdout)
    t1 = stdout.split('|')
    
    #Regiao Newton
    time_normal1.append(float(t1[2].replace(" ","")))
    #Regiao Derivada
    time_normal2.append(float(t1[5].replace(" ","")))
    #Regiao Jacobiana
    time_normal3.append(float(t1[8].replace(" ","")))
    #Regiao Gauss
    time_normal4.append(float(t1[11].replace(" ","")))

    ## COLETANDO OTIMIZADO
    os.chdir("../T2/")
    sample_out2 = "../sl/sample_out_l3_opt" + str(value)+".dat"
    os.system("likwid-perfctr -C 3 -g L3 -m ./newtonSNL -o saida_"+ str(value) +"_opt.txt <" + sample_in + " > " + sample_out2)
    result = subprocess.Popen(["fgrep", " RDTSC Runtime ", sample_out2], stdout=subprocess.PIPE)
    stdout,stderr = result.communicate()
    stdout = str(stdout)
    t1 = stdout.split('|')
    
    #Regiao Newton
    time_otimizado1.append(float(t1[2].replace(" ","")))
    #Regiao Derivada
    time_otimizado2.append(float(t1[5].replace(" ","")))
    #Regiao Jacobiana
    time_otimizado3.append(float(t1[8].replace(" ","")))
    #Regiao Gauss
    time_otimizado4.append(float(t1[11].replace(" ","")))

    #------------------------------------------------------------------------------------------------------

    # gráfico 2 Banda de Memoria---------------------------------------------------------------------------
    
    ## COLETANDO NORMAL
    result = subprocess.Popen(["fgrep", " L3 bandwidth ", sample_out], stdout=subprocess.PIPE)
    stdout,stderr = result.communicate()
    stdout = str(stdout)
    t1 = stdout.split('|')

    #Regiao Newton
    banda_memoria_normal1.append(float(t1[2].replace(" ","")))
    #Regiao Derivada
    banda_memoria_normal2.append(float(t1[5].replace(" ","")))
    #Regiao Jacobiana
    banda_memoria_normal3.append(float(t1[8].replace(" ","")))
    #Regiao Gauss
    banda_memoria_normal4.append(float(t1[11].replace(" ","")))

    ## COLETANDO OTIMIZADO
    result = subprocess.Popen(["fgrep", " L3 bandwidth ", sample_out2], stdout=subprocess.PIPE)
    stdout,stderr = result.communicate()
    stdout = str(stdout)
    t1 = stdout.split('|')

    #Regiao Newton
    banda_memoria_otimizado1.append(float(t1[2].replace(" ","")))
    #Regiao Derivada
    banda_memoria_otimizado2.append(float(t1[5].replace(" ","")))
    #Regiao Jacobiana
    banda_memoria_otimizado3.append(float(t1[8].replace(" ","")))
    #Regiao Gauss
    banda_memoria_otimizado4.append(float(t1[11].replace(" ","")))

    #------------------------------------------------------------------------------------------------------

    # gráfico 3 CACHE MISS L2---------------------------------------------------------------------------
    
    ## COLETANDO NORMAL
    os.chdir("../T1/")
    sample_out = "../sl/sample_out_l2cache_" + str(value)+".dat"
    print("Executando likwid-perfctr para N " + str(value) + " e -g L2CACHE ...")
    os.system("likwid-perfctr -C 3 -g L2CACHE -m ./newtonSNL -o saida_" + str(value) + ".txt <" + sample_in + " > " + sample_out)

    result = subprocess.Popen(["fgrep", " L2 miss ratio ", sample_out], stdout=subprocess.PIPE)
    stdout,stderr = result.communicate()
    stdout = str(stdout)
    t1 = stdout.split('|')

    #Regiao Newton
    cache_miss_normal1.append(float(t1[2].replace(" ","")))
    #Regiao Derivada
    cache_miss_normal2.append(float(t1[5].replace(" ","")))
    #Regiao Jacobiana
    cache_miss_normal3.append(float(t1[8].replace(" ","")))
    #Regiao Gauss
    cache_miss_normal4.append(float(t1[11].replace(" ","")))
    
    ## COLETANDO OTIMIZADO
    os.chdir("../T2/")
    sample_out2 = "../sl/sample_out_l2cache_opt" + str(value)+".dat"
    os.system("likwid-perfctr -C 3 -g L2CACHE -m ./newtonSNL -o saida_"+ str(value) +"_opt.txt <" + sample_in + " > " + sample_out2)
    result = subprocess.Popen(["fgrep", " L2 miss ratio ", sample_out2], stdout=subprocess.PIPE)
    stdout,stderr = result.communicate()
    stdout = str(stdout)
    t1 = stdout.split('|')  

    #Regiao Newton  
    cache_miss_otimizado1.append(float(t1[2].replace(" ","")))
    #Regiao Derivada
    cache_miss_otimizado2.append(float(t1[5].replace(" ","")))
    #Regiao Jacobiana
    cache_miss_otimizado3.append(float(t1[8].replace(" ","")))
    #Regiao Gauss
    cache_miss_otimizado4.append(float(t1[11].replace(" ","")))

    #------------------------------------------------------------------------------------------------------

    # gráfico 4 OPERAÇÕES ARITMÉTICAS----------------------------------------------------------------------
    
    ## COLETANDO NORMAL
    os.chdir("../T1")
    sample_out = "../sl/sample_out_flops_" + str(value)+".dat"
    print("Executando likwid-perfctr para N " + str(value) + " e -g FLOPS_DP ...")
    os.system("likwid-perfctr -C 3 -g FLOPS_DP -m ./newtonSNL -o saida_"+ str(value) +".txt <" + sample_in + " > " + sample_out)
        
    result = subprocess.Popen(["fgrep", "DP [MFLOP/s]", sample_out], stdout=subprocess.PIPE)
    stdout,stderr = result.communicate()
    stdout = str(stdout)
    t2 = stdout.split('|')

    #Regiao Newton
    dp_normal1.append(float(t2[2].replace(" ","")))
    avx_normal1.append(float(t2[5].replace(" ",""))) 
    #Regiao Derivada
    dp_normal2.append(float(t2[8].replace(" ","")))
    avx_normal2.append(float(t2[11].replace(" ",""))) 
    #Regiao Jacobiana
    dp_normal3.append(float(t2[14].replace(" ","")))
    avx_normal3.append(float(t2[17].replace(" ",""))) 
    #Regiao Gauss
    dp_normal4.append(float(t2[20].replace(" ","")))
    avx_normal4.append(float(t2[23].replace(" ",""))) 

    ## COLETANDO OTIMIZADO
    os.chdir("../T2")
    sample_out2 = "../sl/sample_out_flops_opt" + str(value)+".dat"
    os.system("likwid-perfctr -C 3 -g FLOPS_DP -m ./newtonSNL -o saida_"+ str(value) +"_opt.txt <" + sample_in + " > " + sample_out2)
    result = subprocess.Popen(["fgrep", "DP [MFLOP/s]", sample_out2], stdout=subprocess.PIPE)
    stdout,stderr = result.communicate()
    stdout = str(stdout)
    t2 = stdout.split('|')
    
    #Regiao Newton
    dp_otimizado1.append(float(t2[2].replace(" ","")))
    avx_otimizado1.append(float(t2[5].replace(" ",""))) 
    #Regiao Derivada
    dp_otimizado2.append(float(t2[8].replace(" ","")))
    avx_otimizado2.append(float(t2[11].replace(" ",""))) 
    #Regiao Jacobiana
    dp_otimizado3.append(float(t2[14].replace(" ","")))
    avx_otimizado3.append(float(t2[17].replace(" ",""))) 
    #Regiao Gauss
    dp_otimizado4.append(float(t2[20].replace(" ","")))
    avx_otimizado4.append(float(t2[23].replace(" ",""))) 
    os.chdir("../")
    #------------------------------------------------------------------------------------------------------

#PLOT GRAFICOS TEMPO---------------------------------------------------------------------------------------

df = pd.DataFrame()
df.insert(loc= 0, column ='A' ,value = N)
df.insert(loc= 1, column ='B' ,value = time_normal1)
df.insert(loc= 2, column ='C' ,value = time_otimizado1)
df.to_csv('csvs/tempo1.csv', index=False, header=False) 

os.system("gnuplot-qt -persist -s -e 'reset; \
                            set logscale y; \
                            set terminal qt 1 size 900,600; \
                            set title \"Teste de tempo - Newton\"; \
                            set xlabel \"N\"; \
                            set ylabel \"Tempo\"; \
                            set datafile separator comma; \
                            plot \"csvs/tempo1.csv\" using 1:2 with lines t\"Normal\", \
                            \"csvs/tempo1.csv\" using 1:3 with lines t\"Otimizado\"'")

df = pd.DataFrame()
df.insert(loc= 0, column ='A' ,value = N)
df.insert(loc= 1, column ='B' ,value = time_normal2)
df.insert(loc= 2, column ='C' ,value = time_otimizado2)
df.to_csv('csvs/tempo2.csv', index=False, header=False) 

os.system("gnuplot-qt -persist -s -e 'reset; \
                            set logscale y; \
                            set terminal qt 1 size 900,600; \
                            set title \"Teste de tempo - Derivadas\"; \
                            set xlabel \"N\"; \
                            set ylabel \"Tempo\"; \
                            set datafile separator comma; \
                            plot \"csvs/tempo2.csv\" using 1:2 with lines t\"Normal\", \
                            \"csvs/tempo2.csv\" using 1:3 with lines t\"Otimizado\"'")

df = pd.DataFrame()
df.insert(loc= 0, column ='A' ,value = N)
df.insert(loc= 1, column ='B' ,value = time_normal3)
df.insert(loc= 2, column ='C' ,value = time_otimizado3)
df.to_csv('csvs/tempo3.csv', index=False, header=False) 

os.system("gnuplot-qt -persist -s -e 'reset; \
                            set logscale y; \
                            set terminal qt 1 size 900,600; \
                            set title \"Teste de tempo - Jacobiana\"; \
                            set xlabel \"N\"; \
                            set ylabel \"Tempo\"; \
                            set datafile separator comma; \
                            plot \"csvs/tempo3.csv\" using 1:2 with lines t\"Normal\", \
                            \"csvs/tempo3.csv\" using 1:3 with lines t\"Otimizado\"'")

df = pd.DataFrame()
df.insert(loc= 0, column ='A' ,value = N)
df.insert(loc= 1, column ='B' ,value = time_normal4)
df.insert(loc= 2, column ='C' ,value = time_otimizado4)
df.to_csv('csvs/tempo4.csv', index=False, header=False) 

os.system("gnuplot-qt -persist -s -e 'reset; \
                            set logscale y; \
                            set terminal qt 1 size 900,600; \
                            set title \"Teste de tempo - Gauss\"; \
                            set xlabel \"N\"; \
                            set ylabel \"Tempo\"; \
                            set datafile separator comma; \
                            plot \"csvs/tempo4.csv\" using 1:2 with lines t\"Normal\", \
                            \"csvs/tempo4.csv\" using 1:3 with lines t\"Otimizado\"'")

#---------------------------------------------------------------------------------------------------------

#PLOT GRAFICO BANDA DE MEMORIA---------------------------------------------------------------------------------------
df = pd.DataFrame()
df.insert(loc= 0, column ='A' ,value = N)
df.insert(loc= 1, column ='B' ,value = banda_memoria_normal1)
df.insert(loc= 2, column ='C' ,value = banda_memoria_otimizado1)
df.to_csv('csvs/banda_memoria1.csv', index=False, header=False) 

os.system("gnuplot-qt -persist -s -e 'reset; \
                            set terminal qt 1 size 900,600; \
                            set title \"Banda de Memória - Newton\"; \
                            set xlabel \"N\"; \
                            set ylabel \"Memory Bandwidth\"; \
                            set datafile separator comma; \
                            plot \"csvs/banda_memoria1.csv\" using 1:2 with lines t\"Normal\", \
                            \"csvs/banda_memoria1.csv\" using 1:3 with lines t\"Otimizado\"'")

df = pd.DataFrame()
df.insert(loc= 0, column ='A' ,value = N)
df.insert(loc= 1, column ='B' ,value = banda_memoria_normal2)
df.insert(loc= 2, column ='C' ,value = banda_memoria_otimizado2)
df.to_csv('csvs/banda_memoria2.csv', index=False, header=False) 

os.system("gnuplot-qt -persist -s -e 'reset; \
                            set terminal qt 1 size 900,600; \
                            set title \"Banda de Memória - Derivadas\"; \
                            set xlabel \"N\"; \
                            set ylabel \"Memory Bandwidth\"; \
                            set datafile separator comma; \
                            plot \"csvs/banda_memoria2.csv\" using 1:2 with lines t\"Normal\", \
                            \"csvs/banda_memoria2.csv\" using 1:3 with lines t\"Otimizado\"'")

df = pd.DataFrame()
df.insert(loc= 0, column ='A' ,value = N)
df.insert(loc= 1, column ='B' ,value = banda_memoria_normal3)
df.insert(loc= 2, column ='C' ,value = banda_memoria_otimizado3)
df.to_csv('csvs/banda_memoria3.csv', index=False, header=False) 

os.system("gnuplot-qt -persist -s -e 'reset; \
                            set terminal qt 1 size 900,600; \
                            set title \"Banda de Memória - Jacobiana\"; \
                            set xlabel \"N\"; \
                            set ylabel \"Memory Bandwidth\"; \
                            set datafile separator comma; \
                            plot \"csvs/banda_memoria3.csv\" using 1:2 with lines t\"Normal\", \
                            \"csvs/banda_memoria3.csv\" using 1:3 with lines t\"Otimizado\"'")

df = pd.DataFrame()
df.insert(loc= 0, column ='A' ,value = N)
df.insert(loc= 1, column ='B' ,value = banda_memoria_normal4)
df.insert(loc= 2, column ='C' ,value = banda_memoria_otimizado4)
df.to_csv('csvs/banda_memoria4.csv', index=False, header=False) 

os.system("gnuplot-qt -persist -s -e 'reset; \
                            set terminal qt 1 size 900,600; \
                            set title \"Banda de Memória - Gauss\"; \
                            set xlabel \"N\"; \
                            set ylabel \"Memory Bandwidth\"; \
                            set datafile separator comma; \
                            plot \"csvs/banda_memoria4.csv\" using 1:2 with lines t\"Normal\", \
                            \"csvs/banda_memoria4.csv\" using 1:3 with lines t\"Otimizado\"'")

#PLOT CACHE MISS L2---------------------------------------------------------------------------------------------------------

df = pd.DataFrame()
df.insert(loc= 0, column ='A' ,value = N)
df.insert(loc= 1, column ='B' ,value = cache_miss_normal1)
df.insert(loc= 2, column ='C' ,value = cache_miss_otimizado1)
df.to_csv('csvs/cache_miss1.csv', index=False, header=False) 

os.system("gnuplot-qt -persist -s -e 'reset; \
                            set terminal qt 1 size 900,600; \
                            set title \"Cache miss L2 - Newton\"; \
                            set xlabel \"N\"; \
                            set ylabel \"Miss Ratio\"; \
                            set datafile separator comma; \
                            plot \"csvs/cache_miss1.csv\" using 1:2 with lines t\"Normal\", \
                            \"csvs/cache_miss1.csv\" using 1:3 with lines t\"Otimizado\"'")

df = pd.DataFrame()
df.insert(loc= 0, column ='A' ,value = N)
df.insert(loc= 1, column ='B' ,value = cache_miss_normal2)
df.insert(loc= 2, column ='C' ,value = cache_miss_otimizado2)
df.to_csv('csvs/cache_miss2.csv', index=False, header=False) 

os.system("gnuplot-qt -persist -s -e 'reset; \
                            set terminal qt 1 size 900,600; \
                            set title \"Cache miss L2 - Derivadas\"; \
                            set xlabel \"N\"; \
                            set ylabel \"Miss Ratio\"; \
                            set datafile separator comma; \
                            plot \"csvs/cache_miss2.csv\" using 1:2 with lines t\"Normal\", \
                            \"csvs/cache_miss2.csv\" using 1:3 with lines t\"Otimizado\"'")

df = pd.DataFrame()
df.insert(loc= 0, column ='A' ,value = N)
df.insert(loc= 1, column ='B' ,value = cache_miss_normal3)
df.insert(loc= 2, column ='C' ,value = cache_miss_otimizado3)
df.to_csv('csvs/cache_miss3.csv', index=False, header=False) 

os.system("gnuplot-qt -persist -s -e 'reset; \
                            set terminal qt 1 size 900,600; \
                            set title \"Cache miss L2 - Jacobiana\"; \
                            set xlabel \"N\"; \
                            set ylabel \"Miss Ratio\"; \
                            set datafile separator comma; \
                            plot \"csvs/cache_miss3.csv\" using 1:2 with lines t\"Normal\", \
                            \"csvs/cache_miss3.csv\" using 1:3 with lines t\"Otimizado\"'")

df = pd.DataFrame()
df.insert(loc= 0, column ='A' ,value = N)
df.insert(loc= 1, column ='B' ,value = cache_miss_normal4)
df.insert(loc= 2, column ='C' ,value = cache_miss_otimizado4)
df.to_csv('csvs/cache_miss4.csv', index=False, header=False) 

os.system("gnuplot-qt -persist -s -e 'reset; \
                            set terminal qt 1 size 900,600; \
                            set title \"Cache miss L2 - Gauss\"; \
                            set xlabel \"N\"; \
                            set ylabel \"Miss Ratio\"; \
                            set datafile separator comma; \
                            plot \"csvs/cache_miss4.csv\" using 1:2 with lines t\"Normal\", \
                            \"csvs/cache_miss4.csv\" using 1:3 with lines t\"Otimizado\"'")

#---------------------------------------------------------------------------------------------------------

#PLOT OPERACAO ARITMETICA---------------------------------------------------------------------------------------------------------

df = pd.DataFrame()
df.insert(loc= 0, column ='A' ,value = N)
df.insert(loc= 1, column ='B' ,value = dp_normal1)
df.insert(loc= 2, column ='C' ,value = avx_normal1)
df.insert(loc= 3, column ='D' ,value = dp_otimizado1)
df.insert(loc= 4, column ='E' ,value = avx_otimizado1)
df.to_csv('csvs/flops1.csv', index=False, header=False) 
os.system("gnuplot-qt -persist -s -e 'reset; \
                            set terminal qt 1 size 900,600; \
                            set title \"Operações aritméticas - Newton\"; \
                            set xlabel \"N\"; \
                            set ylabel \"MFLOP\"; \
                            set datafile separator comma; \
                            plot \"csvs/flops1.csv\" using 1:2 with lines t\"Normal - DP\", \
                            \"csvs/flops1.csv\" using 1:3 with lines t\"Normal - AVX\", \
                            \"csvs/flops1.csv\" using 1:4 with lines t\"Otimizado - DP\", \
                            \"csvs/flops1.csv\" using 1:5 with lines t\"Otimizado - AVX\"'")

df = pd.DataFrame()
df.insert(loc= 0, column ='A' ,value = N)
df.insert(loc= 1, column ='B' ,value = dp_normal2)
df.insert(loc= 2, column ='C' ,value = avx_normal2)
df.insert(loc= 3, column ='D' ,value = dp_otimizado2)
df.insert(loc= 4, column ='E' ,value = avx_otimizado2)
df.to_csv('csvs/flops2.csv', index=False, header=False) 
os.system("gnuplot-qt -persist -s -e 'reset; \
                            set terminal qt 1 size 900,600; \
                            set title \"Operações aritméticas - Derivadas\"; \
                            set xlabel \"N\"; \
                            set ylabel \"MFLOP\"; \
                            set datafile separator comma; \
                            plot \"csvs/flops2.csv\" using 1:2 with lines t\"Normal - DP\", \
                            \"csvs/flops2.csv\" using 1:3 with lines t\"Normal - AVX\", \
                            \"csvs/flops2.csv\" using 1:4 with lines t\"Otimizado - DP\", \
                            \"csvs/flops2.csv\" using 1:5 with lines t\"Otimizado - AVX\"'")

df = pd.DataFrame()
df.insert(loc= 0, column ='A' ,value = N)
df.insert(loc= 1, column ='B' ,value = dp_normal3)
df.insert(loc= 2, column ='C' ,value = avx_normal3)
df.insert(loc= 3, column ='D' ,value = dp_otimizado3)
df.insert(loc= 4, column ='E' ,value = avx_otimizado3)
df.to_csv('csvs/flops3.csv', index=False, header=False) 
os.system("gnuplot-qt -persist -s -e 'reset; \
                            set terminal qt 1 size 900,600; \
                            set title \"Operações aritméticas - Jacobiana\"; \
                            set xlabel \"N\"; \
                            set ylabel \"MFLOP\"; \
                            set datafile separator comma; \
                            plot \"csvs/flops3.csv\" using 1:2 with lines t\"Normal - DP\", \
                            \"csvs/flops3.csv\" using 1:3 with lines t\"Normal - AVX\", \
                            \"csvs/flops3.csv\" using 1:4 with lines t\"Otimizado - DP\", \
                            \"csvs/flops3.csv\" using 1:5 with lines t\"Otimizado - AVX\"'")

df = pd.DataFrame()
df.insert(loc= 0, column ='A' ,value = N)
df.insert(loc= 1, column ='B' ,value = dp_normal4)
df.insert(loc= 2, column ='C' ,value = avx_normal4)
df.insert(loc= 3, column ='D' ,value = dp_otimizado4)
df.insert(loc= 4, column ='E' ,value = avx_otimizado4)
df.to_csv('csvs/flops4.csv', index=False, header=False) 
os.system("gnuplot-qt -persist -s -e 'reset; \
                            set terminal qt 1 size 900,600; \
                            set title \"Operações aritméticas - Gauss\"; \
                            set xlabel \"N\"; \
                            set ylabel \"MFLOP\"; \
                            set datafile separator comma; \
                            plot \"csvs/flops4.csv\" using 1:2 with lines t\"Normal - DP\", \
                            \"csvs/flops4.csv\" using 1:3 with lines t\"Normal - AVX\", \
                            \"csvs/flops4.csv\" using 1:4 with lines t\"Otimizado - DP\", \
                            \"csvs/flops4.csv\" using 1:5 with lines t\"Otimizado - AVX\"'")

#---------------------------------------------------------------------------------------------------------

                    
os.chdir("../vods18vrs18/T1")
os.system("make purge")

os.chdir("../")

os.chdir("../vods18vrs18/T2")
os.system("make purge")                          