#include <csignal>
#include <cstdlib>
#include <iostream>
#include <vector>
#include <fstream>
#include <queue>
#include <utility>
#include <string>

using namespace std;

// Flags para controlar a interrupcao por tempo
volatile int pare = 0;       // Indica se foi recebido o sinal de interrupcao
volatile int escrevendo = 0; // Indica se estah atualizando a melhor solucao

// Variaveis para guardar a melhor solucao encontrada
vector<int> melhor_solucao;
int melhor_custo;

typedef struct dados_no {
	int pai;
	int limite_inferior;
	int nivel;
	int cena;
	bool visitado;
	char status; /*explorado, amadurecido */
}dados_no;

struct comparador {
     bool operator()(pair<int, dados_no> i, pair<int, dados_no> j) {
     return i.first > j.first;
    }
};

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
int k_1(vector<int>& custos, vector<int>& solucao_parcial, vector<int>& s) {
	int e, d, total = 0;
	int total_p = 0;

	for(int i = 0; i < solucao_parcial.size(); i++)
		if(solucao_parcial != -1)
			total_p += 1;

	if(total_p == 0) {
		return 0;
	} else if (total_p == 1) {
		return 0;
	} else {
		// for (int i = 0; i < )
	}

}

void b_n_b(int raiz, int N, vector<int>& custos, vector<int>& s) {
	vector<int> melhor_solucao, limites_inf;
	priority_queue<pair<int, dados_no>, vector<pair<int, dados_no> >, comparador> nos_ativos; /* min_heap */
	int limite_sup = 1000000, qtd_filhos, total_lim_inf = 0, pos = 0;
	pair<int, dados_no> no_aux;

	no_aux.first = 0;
	no_aux.second.pai = -1;
	no_aux.second.visitado = true;
	no_aux.second.status = 'e';
	no_aux.second.nivel = 0;
	no_aux.second.cena = -1;

	melhor_solucao.resize(N);
	for(int i = 0; i < solucao_parcial.size(); i++)
		melhor_solucao[i] = -1;
	// pos = melhor_solucao.size() - 1 - i
	nos_ativos.push(no_aux);
	while(!nos_ativos.empty()){
		pair<int, dados_no> no = nos_ativos.top();
		nos_ativos.pop();
		qtd_filhos = N - no.second.nivel;
		cout << qtd_filhos << endl;
		for(i = 0; i < qtd_filhos; i++) {
			total_lim_inf += k_1(custos, melhor_solucao, s);

		}
	// 	// total_lim_inf += k_2();
	// 	// total_lim_inf += k_3();
	// 	// total_lim_inf += k_4();
		for(j = 0; j < qtd_filhos; j++) {
			if (limite_inf > limite_sup){

			} else {
	// 	// 		if(qtd_filhos == 1) {
	// 	// 			limite_sup = limite inferior desse no;
	// 	// 			atualiza_solucao(melhor_solucao);
	// 	// 		} else {
	// 	// 			nos_ativos.push(esse no);
	// 	// 		}
	// 	// 	}
		}
	}
}
// -pai
// -nivel
// -limitante
// -cena
// -visitado
// -podado
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

	b_n_b(0, cenas, custos, s);

	// return melhor_solucao;
    // Continue sua implementacao aqui. Sempre que encontrar uma nova
    // solucao, utilize a funcao atualiza_solucao para atualizar as
    // variaveis globais.

    return 0;
}
