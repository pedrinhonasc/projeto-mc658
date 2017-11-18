/* declaracoes de conjuntos e parametros */

param n, integer, >=1;  /* quantidade de cenas */
param m, integer, >= 1; /* quantidade de atores */

/* matriz T */
set ATORES := {1..m}; /* conjunto de indices de atores */
set CENAS := {1..n};  /* conjunto de indices de dias de gravacao */
param T{i in ATORES, j in CENAS};

/* custo diário de espera */
param c{i in ATORES};

/* numero de cenas nas quais cada ator participa */
param s{i in ATORES}:=sum{j in CENAS} T[i,j];

/* ===> variaveis: */
set DIAS := {1..n};
/* ator i na cena j esta em espera */
var esperas{i in ATORES,j in CENAS}, binary;

/* cena i vai ser gravada no dia j */
var calendario_cenas{i in CENAS, j in DIAS}, binary;

/* ator i vai gravar no dia j */
var dias_de_gravacao{i in ATORES, j in DIAS}, binary;

/* ator i esta no set de gravacao no dia j */
var dias_no_set{i in ATORES, j in DIAS}, binary;

/* no dia j o ator i ja havia chegado no set de gravacao*/
var chegou_no_set{i in ATORES, j in DIAS}, binary;

/* no dia j o ator i ja havia deixado o set de gravacao */
var deixou_o_set{i in ATORES, j in DIAS}, binary;

/* ===> funcao objetivo */
minimize total_espera: (sum{i in ATORES} c[i] * (sum{j in CENAS} esperas[i,j]));

/* ===> restricoes */
/* R = T * P */
/* matriz resultante contento as cenas na ordem correta */
s.t. calendario_gravacao{i in ATORES, j in DIAS}: sum{k in CENAS} (T[i, k] * calendario_cenas[k, j]) = dias_de_gravacao[i,j];

/* soma dos elementos de cada linha da matriz de permutacao calendario_cenas tem que ser 1 */
s.t. ordem_linhas{i in CENAS}: sum{j in DIAS} calendario_cenas[i,j] = 1;

/* soma dos elementos de cada coluna da matriz de permutacao calendario_cenas tem que ser 1 */
s.t. ordem_colunas{i in DIAS}: sum{j in CENAS} calendario_cenas[j,i] = 1;

/* primeiro dia nao pode ser de espera  */
s.t. primeirodia{i in ATORES}: esperas[i, 1] = 0;

/* ultimo dia nao pode ser de espera */
s.t. ultimodia{i in ATORES}: esperas[i, n] = 0;

/* se ator i gravou pelo menos uma cena ate o dia j (inclusive), entao chegou_no_set[i,j] = 1 */
s.t. chegou1{i in ATORES, j in CENAS}: sum{k in {1..j}} dias_de_gravacao[i,k] <= n * chegou_no_set[i,j];

/* ator nao gravou nenhuma cena ate o dia j(inclusive), entao chegou_no_set[i,j] = 0 */
s.t. chegou2{i in ATORES, j in CENAS}: sum{k in {1..j}} dias_de_gravacao[i,k] >= chegou_no_set[i,j];

/* se ator i possui mais cenas para gravar a partir do dia j (inclusive), entao deixou_set[i,j] = 1(note que 0 significa que ator deixou o set, e 1 que ele esta no set ainda) */
s.t. ja_embora1{i in ATORES, j in CENAS}: sum{k in {j..n}} dias_de_gravacao[i,k] <= n*(deixou_o_set[i,j]);

/* se ator i nao tem mais cenas pra gravar a partir do dia j(inclusive, entao deixou_set[i,j] = 0(note que 0 significa que ator deixou o set, e 1 que ele esta no set ainda)) */
s.t. ja_embora2{i in ATORES, j in CENAS}: (1-chegou_no_set[i,j]) + sum{k in {j..n}} dias_de_gravacao[i,k] >= deixou_o_set[i,j];

/* ator i esta no set de gravacao no dia j */
s.t. em_set{i in ATORES, j in DIAS}: dias_no_set[i,j] = chegou_no_set[i,j] - (1-deixou_o_set[i,j]);

/* o dia j e um dia de espera para o ator i*/
s.t. em_espera{i in ATORES, j in {2..n-1}}: esperas[i,j] = dias_no_set[i,j] - dias_de_gravacao[i,j];


/* restricoes que acho qeu nao vou usar */
/* se ator esta no set de gravacao, entao resultante = 1 */
/*s.t. condicao{i in ATORES, j in CENAS}: dias_no_set[i,j] >= calendario_cenas[i,j]; */

/*s.t. cond{i in ATORES, j in CENAS}: chegou_no_set[i,j] - (1-deixou_o_set[i,j]) >= 0;*/

/* ator nao pode estar em cena e espera ao mesmo tempo */
/*s.t. emcena{i in ATORES, j in CENAS}: calendario_cenas[i, j] + esperas[i, j] <= 1; */


/* resolve problema */
solve;

/* ===> imprime solucao (n valores inteiros separados por espaco, onde
o j-esimo valor corresponde ao dia em que foi gravada a cena j) */

for {a in CENAS} {
	for {b in CENAS} {
		for{{0}: calendario_cenas[a,b] == 1} {
			printf "%d ", b;}
	}
}
printf: "\n";

/* ===> imprime custo da solucao encontrada */
printf '%d\n', total_espera;

end;
