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
    //  return i.second.nivel < j.second.nivel && i.first > j.first;
    }
} comparador_menor;
// struct comparador var;

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

vector<int> sol_gulosa_aleatoria(vector<vector<int> >& t, int cenas, int atores, vector<int>& custos, vector<int>& s) {
	vector<int> solucao_s, chegada, espera_s, espera_cena, saida, cenas_gravadas;
	/* cena e peso */
	vector<pair<int, int> > L;
	int uns, index;
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
	// if (!melhor_solucao.size()) {
	// 	for(int i = 0; i < cenas; i++) {
	// 		uns = 0;
	// 		for(int j = 0; j < atores; j++) {
	// 			if(t[j][i])
	// 			uns++;
	// 		}
	// 		L.push_back(make_pair(i+1,uns));
	// 	}
	// } else {
	for(int i = 0; i < cenas; i++) {
		uns = 0;
		for(int j = 0; j < atores; j++) {
			if(t[j][i])
				uns++;
			// if(t[j][melhor_solucao[i]-1])
		}
		L.push_back(make_pair(i+1,uns));
		// L.push_back(make_pair(melhor_solucao[i],uns));
	}
	// }

	// shuffle
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
			/* encontrando os dias de espera que cada ator tem na solucao_s */
			for(int i = 0; i < chegada.size(); i++) {
				espera_s[i] = 0;
				if(chegada[i] != 0 && saida[i] != 0) {
					/* espera_s e por ator */
					for(int j = chegada[i]; j < saida[i] - chegada[i] +1; j++) {
						if(t[i][solucao_s[j]-1] == 0)
							espera_s[i]++;
						// if(t[i][solucao_s[chegada[i]+j]-1] == 0)
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
			if (solucao_s.size() == cenas) {
				for(int i = 0; i < espera_s.size(); i++) {
					custo_total += custos[i] * espera_s[i];
				}
			}

			/* testar cada cena em L quantos dias de espera vai resultar */

			// for(int i = 0; i < L.size(); i++) {
			// 	for(int j; j < atores; j++) {
			// 		for(int k = 0; k < solucao_s.size(); k++) {
			// 			if(t[j][L[i].first -1] == 0 && t[j][solucao_s[k]-1])
			//
			// 		}
			// 	}
			// }
		}

	}
	if(!melhor_solucao.size())
		atualiza_solucao(solucao_s, custo_total);
	else {
		if (custo_total <= melhor_custo)
			atualiza_solucao(solucao_s, custo_total);
	}
	return solucao_s;
}

void grasp(vector<vector<int> >& t, vector<int>& custos, int cenas, int atores, vector<int>& s) {
	vector<int> solucao_s;
	int custo_s, nao_atualiza = 0;

	while(nao_atualiza != 10) {
		solucao_s = sol_gulosa_aleatoria(t, cenas, atores, custos, s);
		for(int i = 0; i < solucao_s.size(); i++) {

		}
		// if(!melhor_solucao.size())
		// 	atualiza_solucao(solucao_s, custo_s);
		// else
		// 	if (custo_s <= melhor_custo) {
		// 		nao_atualiza = 0;
		// 		atualiza_solucao(solucao_s, custo_s);
		// 	}
		// 	else {
		// 		nao_atualiza++;
		// 	}
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
				while(str[j] != ' ') {
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
	for (i = 0; i < atores; i++) {
		s[i] = 0;
		for(j = 0; j < cenas; j++) {
			s[i] += t[i][j];
		}
	}
	grasp(t, custos, cenas, atores, s);
	// return melhor_solucao;
    // Continue sua implementacao aqui. Sempre que encontrar uma nova
    // solucao, utilize a funcao atualiza_solucao para atualizar as
    // variaveis globais.

    return 0;
}
