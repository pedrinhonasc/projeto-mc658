#include <csignal>
#include <cstdlib>
#include <iostream>
#include <vector>
#include<fstream>
#include <algorithm>

using namespace std;

// Flags para controlar a interrupcao por tempo
volatile int pare = 0;       // Indica se foi recebido o sinal de interrupcao
volatile int escrevendo = 0; // Indica se estah atualizando a melhor solucao

// Variaveis para guardar a melhor solucao encontrada
vector<int> melhor_solucao;
int melhor_custo;

struct comparador {
     bool operator()(pair<int, int> i, pair<int, int> j) {
		 return i.second < j.second;
    }
} comparador_menor;

struct comparador_maior {
	bool operator() (pair<int, int> i,pair<int, int> j) {
		return i.second > j.second;
	}
} comparador_maior;
void imprime_saida() {
    // Lembre-se: a primeira linha da saida deve conter n inteiros,
    // tais que o j-esimo inteiro indica o dia de gravacao da cena j!
    for (int j = 0; j < melhor_solucao.size(); j++)
        cout << melhor_solucao[j] << " ";
    // A segunda linha contem o custo (apenas de dias de espera!)
    cout << endl << melhor_custo << endl;
}

void atualiza_solucao(const vector<int> &solucao, int custo) {
    escrevendo = 1;
    melhor_solucao = solucao;
    melhor_custo = custo;
    escrevendo = 0;
    if (pare == 1) {
        // Se estava escrevendo a solucao quando recebeu o sinal,
        // espera terminar a escrita e apenas agora imprime a saida e
        // termina
        imprime_saida();
        exit(0);
    }
}

void interrompe(int signum) {
    pare = 1;
    if (escrevendo == 0) {
        // Se nao estava escrevendo a solucao, pode imprimir e terminar
        imprime_saida();
        exit(0);
    }
}
/* Faz uma busca local com base na solucao gulosa-aleatoria encontrada. A busca
 * local e feita da seguinte forma: pega-se as N/2 cenas com maior numero de
 * dias de espera. Entao, para cada uma dessas N/2 cenas, troca-se com as outras
 * cenas restantes para ver se e possivel encontrar uma solucao ainda melhor.
 * Caso seja possivel atualiza a solucao, se nao continua a heuristica */
void busca_local(vector<int>& solucao_s, vector<int>& espera_cena, vector<vector<int> >& t, vector<int>& custos, vector<int>& s, int atores) {
	int qtd = (espera_cena.size()/2);
	int aux, gravadas, custo;
	vector<pair<int, int> > piores_cenas;
	vector<vector<int> > matriz_solucao;
	vector<int> espera;
	vector<int> solucao;
	solucao.resize(solucao_s.size());

	for(int i = 0; i < espera_cena.size(); i++)
		piores_cenas.push_back(make_pair(i+1, espera_cena[i]));
	/* ordena as piores cenas em ordem decrescente */
	sort(piores_cenas.begin(), piores_cenas.end(), comparador_maior);

	/* troca-se as cenas de posicoes, isto e, pega um cena ruim(com nuemro grande
	 * de dias de espera e troca com uma que nao esta nas N/2 cenas com grande
	 * numero de dias de espera. */
	for(int i = 0; i < qtd; i++) {
		for(int j = 0; j < solucao_s.size(); j++) {
			if(solucao_s[j] != piores_cenas[i].first) {
				aux = solucao_s[j];
				solucao_s[j] = solucao_s[piores_cenas[i].first-1];
				solucao_s[piores_cenas[i].first-1] = aux;
				matriz_solucao.resize(atores);

				for(int i = 0; i < atores; i++)
					matriz_solucao[i].resize(solucao_s.size());
				/* constroi a matriz com base na nova solucao gerada */
				for(int i = 0; i < solucao_s.size(); i++) {
					for(int j = 0; j < atores; j++) {
						matriz_solucao[j][i] = t[j][solucao_s[i] -1];
					}
				}
				/* conta-se o numero de dias de espera de cada ator */
				espera.resize(atores);
				for(int i = 0; i < atores; i++) {
					gravadas = 0;
					espera[i] = 0;
					for(int j = 0; j < solucao_s.size(); j++) {
						if(matriz_solucao[i][j] == 1) {
							gravadas++;
						} else if(gravadas == s[i]) {
							break;
						} else if(gravadas != 0) {
							espera[i]++;
						}
					}
				}
				custo = 0;
				/* calcula o custo da nova solucao */
				for(int i = 0; i < espera.size(); i++) {
					custo += custos[i] * espera[i];
				}
				/* monta a solucao no formato de saida esperado */
				for(int i = 0; i < solucao_s.size(); i++)
					solucao[solucao_s[i]-1] = i+1;
				if (custo <= melhor_custo) {
					atualiza_solucao(solucao, custo);
				}
			}
		}
	}
	return;

}

/* encontra solucao com base em uma escolhe gulosa aleatoria */
void sol_gulosa_aleatoria(vector<vector<int> >& t, int cenas, int atores, vector<int>& custos, vector<int>& s) {
	vector<int> solucao_s, chegada, espera_s, espera_cena, saida, cenas_gravadas;
	/* cena e peso */
	vector<pair<int, int> > L;
	int uns, index, gravadas;
	vector<pair<int, int> >::iterator it;
	int custo_total = 0;
	chegada.resize(atores, 0);
	saida.resize(atores, 0);
	espera_s.resize(atores, 0);
	espera_cena.resize(cenas, 0);
	cenas_gravadas.resize(atores, 0);

	/* para escolher a primiera cena a ser colocada na solucao parcial, leva-se
	 * em consideracao a quantiade de numeros 1 em cada cena, que e o peso de cada
	 * uma, assim quanto menor a quantidade de 1 na cena, menor o peso dela e maior
	 * a chance dela ser selecionada */
	for(int i = 0; i < cenas; i++) {
		uns = 0;
		for(int j = 0; j < atores; j++) {
			if(t[j][i])
				uns++;
		}
		L.push_back(make_pair(i+1,uns));
	}

	/* embaralha o vetor de cenas */
	for(int i = L.size() - 1; i > 0; i--) {
  		int j = rand() % i;
  		swap(L[i], L[j]);
	}
	while(solucao_s.size() != cenas) {
		/* encontrar a cena com menor quantiade de uns */
		if(!solucao_s.size()) {
			it = min_element(L.begin(), L.end(), comparador_menor);
			index = distance(L.begin(), it);

			solucao_s.push_back(L[index].first);
			L.erase(it);
		/* encontrar a cena com menor quantidade de dias de espera */
		/* como a primeira cena ja foi selecionada o importante agora e ter o
		 * smenor numero de dias de espera com relacao a proxima cena a ser adicionada,
		 * assim o peso, agora, de cada cena e a quantidade de dia de esperas que
		 * a cena gera caso ela seja selecionada para a solucao parcial. Assim,
		 * quanto maior a quantidade de dias de espera, menor e a chance dela ser
		 * escolhida */
		} else {
			/* encontrando o primeiro dia de chegada de cada ator com relacao as cenas na solucao_s */
			chegada.clear();
			saida.clear();
			cenas_gravadas.clear();
			chegada.resize(atores, 0);
			saida.resize(atores, 0);
			cenas_gravadas.resize(atores, 0);
			for (int i = 0; i < solucao_s.size(); i++) {
				for (int j = 0; j < atores; j++) {
					if(t[j][solucao_s[i]-1] == 1) {
						if (chegada[j] == 0)
							chegada[j] = i+1;
						cenas_gravadas[j]++;
						if(cenas_gravadas[j] == s[j])
							saida[j] = i+1;
					}
				}
			}

			/* contando quantos dias de espera cada cena em L pode ter caso seja
			 * a proxima cena selecionada para a solucao parcial */
			for(int i = 0; i < L.size(); i++) {
				espera_cena[L[i].first-1] = 0;
				for(int j = 0; j < chegada.size(); j++) {
					if (chegada[j] != 0 && saida[j] == 0) {
						/* se for dia de espera */
						if(t[j][L[i].first-1] == 0) {
							espera_cena[L[i].first-1]++;
						}
					}
				}
				/* atualiza o peso dessa cena */
				L[i].second = espera_cena[L[i].first-1];
			}
			/* encontra a cena que gera menos dias de espera e adiciona na solucao parcial */
			it = min_element(L.begin(), L.end(), comparador_menor);
			index = distance(L.begin(), it);
			solucao_s.push_back(L[index].first);
			L.erase(it);
		}

	}
	/* gerando matriz da solucao que foi encontrada para que seja possivel
	 * calcular a quantidade  de dias de espera de cada ator */
	vector<vector<int> > matriz_solucao;
	matriz_solucao.resize(atores);
	for(int i = 0; i < atores; i++)
		matriz_solucao[i].resize(cenas);
	for(int i = 0; i < cenas; i++) {
		for(int j = 0; j < atores; j++) {
			matriz_solucao[j][i] = t[j][solucao_s[i] -1];
		}
	}
	/* contando a quantidade de dias de espera de cada ator com base na solucao
	 * encontrada */
	espera_cena.clear();
	espera_cena.resize(cenas, 0);
	for(int i = 0; i < atores; i++) {
		gravadas = 0;
		for(int j = 0; j < cenas; j++) {
			if(matriz_solucao[i][j] == 1) {
				gravadas++;
			} else if(gravadas == s[i]) {
				break;
			} else if(gravadas != 0) {
				espera_s[i]++;
				espera_cena[j]++;
			}
		}
	}
	/* calcula o custo total da solucao */
	for(int i = 0; i < espera_s.size(); i++) {
		custo_total += custos[i] * espera_s[i];
	}

	vector<int> solucao;
	solucao.resize(cenas);
	/* transformando a solucao encontrada no formato de saida esperado */
	for(int i = 0; i < solucao_s.size(); i++)
		solucao[solucao_s[i]-1] = i+1;

	/* ainda nao ha uma melhor solucao encontrada */
	if(!melhor_solucao.size())
		atualiza_solucao(solucao, custo_total);
	else {
		if (custo_total <= melhor_custo)
			atualiza_solucao(solucao, custo_total);
	}
	busca_local(solucao_s, espera_cena, t, custos, s, atores);
	return;
}

void grasp(vector<vector<int> >& t, vector<int>& custos, int cenas, int atores, vector<int>& s) {
	vector<int> solucao_s;
	int custo_s;

	while(1) {
		sol_gulosa_aleatoria(t, cenas, atores, custos, s);
	}
	return;
}
int main(int argc, char *argv[]) {
	int cenas, atores, i, j;
	vector<int> custos, s;
	vector<vector<int> > t;
	ifstream entrada(argv[1]);
	string str, tmp;
	int linha = 1;

    // Registra a funcao que trata o sinal
    signal(SIGINT, interrompe);

	/* lendo entrada */
	while(getline(entrada, str)) {
		if(linha == 1) {
			tmp = str;
			cenas = atoi(tmp.c_str());
		} else if(linha == 2) {
			tmp = str;
			atores = atoi(tmp.c_str());
			t.resize(atores);
			for(i = 0; i < atores; i++)
				t[i].resize(cenas);
			custos.resize(atores);
		} else if (linha == atores + 3){
			j = 0;
			for(i = 0; i < atores; i++) {
				tmp.clear();
				while(str[j] != ' ' && str[j] != '\0') {
					tmp = tmp + str[j];
					j++;
				}
				custos[i] = atoi(tmp.c_str());
				j++;
			}
		} else {
			for(i = 0; i < cenas; i++) {

				tmp = str[2*i];
				t[linha-3][i] = atoi(tmp.c_str());
			}
		}
		linha++;
	}
	s.resize(atores);
	/* calculando a quantidade de cenas que cada ator grava */
	for (i = 0; i < atores; i++) {
		s[i] = 0;
		for(j = 0; j < cenas; j++) {
			s[i] += t[i][j];
		}
	}
	grasp(t, custos, cenas, atores, s);

    return 0;
}
