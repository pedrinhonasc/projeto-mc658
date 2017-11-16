#include <csignal>
#include <cstdlib>
#include <iostream>
#include <vector>
#include<fstream>

using namespace std;

// Flags para controlar a interrupcao por tempo
volatile int pare = 0;       // Indica se foi recebido o sinal de interrupcao
volatile int escrevendo = 0; // Indica se estah atualizando a melhor solucao

// Variaveis para guardar a melhor solucao encontrada
vector<int> melhor_solucao;
int melhor_custo;

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

vector<int> gualosa_aleatoria(int cenas) {
	vector<int> solucao_s;


	while(solucao_s.size() != cenas) {

	}
	return solucao_s;
}

int main(int argc, char *argv[]) {
	int atores, cenas, custo_slinha;
	vector<int> solucao_s, solucao_slinha;

    // Registra a funcao que trata o sinal
    signal(SIGINT, interrompe);

	ifstream myReadFile;
 	myReadFile.open(argv[1].c_str());
	if (myReadFile.is_open()) {
		while(!myReadFile.eof()) {

		}
	}
	myReadFile.close();
	melhor_solucao.resize(cenas);
	for(i= 0; i < cenas; i++)
		melhor_solucao[i] = 0;

	while(1){
		solucao_s = gualosa_aleatoria();
		solucao_slinha =buscalocal(solucao_s);
		if() {
			atualiza_solucao(solucao_slinha, custo_slinha);
		}
	}
	// return melhor_solucao;
    // Continue sua implementacao aqui. Sempre que encontrar uma nova
    // solucao, utilize a funcao atualiza_solucao para atualizar as
    // variaveis globais.

    return 0;
}
