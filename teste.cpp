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

int nos_explorados = 0;
int melhor_limitante_inf = 0;

typedef struct dados_no {
	vector<int> melhor_solucao;
	int nivel;
	int cena;
	vector<bool> cenas_disponiveis;
}dados_no;

struct comparador {
     bool operator()(pair<int, dados_no> i, pair<int, dados_no> j) {
         if (i.second.nivel < j.second.nivel) {
             return true;
         } else if (i.second.nivel > j.second.nivel) {
            return false;
         }
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
	cout << melhor_limitante_inf << endl;
	cout << nos_explorados << endl;
}

void atualiza_solucao(const vector<int> &solucao, int custo) {
    escrevendo = 1;
    melhor_solucao = solucao;
    melhor_custo = custo;
	for (int j = 0; j < melhor_solucao.size(); j++)
        cout << melhor_solucao[j] << " ";
    // A segunda linha contem o custo (apenas de dias de espera!)
    cout << endl << melhor_custo << endl;
	cout << "melhor custo " << melhor_custo << endl;
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
int limitantes(vector<int>& custos, vector<int>& solucao_parcial, vector<int>& s, vector<vector<int> >& t, int atores, int cenas) {
	int e, d, K1 = 0, K2 = 0, i, j, k, e_i = 0, d_i = 0;
	int tamanho_e = 0, qtd_cenas_e, qtd_cenas_d, l_e, l_d;
	vector<int> e_p, d_p, conjunto_ap, nao_ap;

    
	/* se ha apenas um elemento na solucao parcial ou nao ha nenhum, entao nao tem como calcular o limitante com K1 nem K2 */
	if(solucao_parcial.size() == 0 || solucao_parcial.size() == 1) {
		return 0;
	} else {
		tamanho_e = solucao_parcial.size() / 2;
		/* se tiver numero impar de elementos */
		if (solucao_parcial.size() % 2 != 0) {
			l_e = tamanho_e + 1;
			l_d = cenas - tamanho_e + 1;
			/* encontrando conjunto E(P) */
			for (i = 0, j = 0; i <= tamanho_e; i++, j+=2) {
				e_p.push_back(solucao_parcial[j]);
			}
			/* encontrando conjunto D(P) */
			for(i = tamanho_e + 1, j = 1; i < solucao_parcial.size(); i++, j+=2) {
				d_p.push_back(solucao_parcial[j]);
			}

		} else {
			l_e = tamanho_e;
			l_d = cenas - tamanho_e + 1;
			/* encontrando conjunto E(P) */
			for (i = 0, j = 0; i < tamanho_e; i++, j+=2) {
				e_p.push_back(solucao_parcial[j]);
			}
			/* encontrando conjunto D(P) */
			for(i = tamanho_e, j = 1; i < solucao_parcial.size(); i++, j+=2) {
				d_p.push_back(solucao_parcial[j]);
			}
		}
		/* Encontrando o conjunto A(P) e o conjunto de atores que nao pertecem a A(P) */
		for(i = 0; i < atores; i++) {
			qtd_cenas_e = 0;
			qtd_cenas_d = 0;
            
            for(j = 0; j < e_p.size(); j++) {
                if (t[i][e_p[j]-1]) {
                    qtd_cenas_e++;
                }
            }
            
            for(k = 0; k < d_p.size(); k++) {
                if (t[i][d_p[k]-1]) {
                    qtd_cenas_d++;
                }   
            }
            
			/* se tem inicio em E(P) e fim em D(P) */
			if ((qtd_cenas_e > 0) && (qtd_cenas_d > 0)) {
				conjunto_ap.push_back(i+1);
			/* se todas as cenas ja foram gravadas em E(P) ou D(P) */
			} else if (qtd_cenas_e == s[i] || qtd_cenas_d == s[i]) {
				conjunto_ap.push_back(i+1);
			/* se nao esta em A(P) */
			} else {
				nao_ap.push_back(i+1);
			}
		}
		/* encontrando o primeiro dia de gravacao e o ultimo */
		for(i = 0; i < conjunto_ap.size(); i++) {
			e_i = 0;
			d_i = 0;
			for(j = 0; j < e_p.size(); j++) {
				if(t[conjunto_ap[i]-1][e_p[j]-1]) {
					e_i = j + 1;
					break;
				}
			}
			for(j = 0; j < d_p.size(); j++) {
				if(t[conjunto_ap[i]-1][d_p[j]-1]) {
					d_i = cenas - j;
					break;
				}
			}
			/* se o inicio e fim esta em D(P) */
			if (e_i == 0) {
				for(j = d_p.size() - 1; j >= 0; j--) {
					if(t[conjunto_ap[i]-1][d_p[j]-1]) {
						e_i = cenas - j;
                        break;
					}
				}
			/* se inicio e fim esta em E(P) */
			} else if(d_i == 0) {
				for(j = e_p.size() - 1; j >= 0; j--) {
					if(t[conjunto_ap[i]-1][e_p[j]-1]) {
						d_i = j + 1;
                        break;
					}
				}
			}
			
			K1 += custos[conjunto_ap[i]-1] * (d_i - e_i + 1 - s[conjunto_ap[i]-1]);
		}
        
        //cout << "K1 = " << total << endl;

		/******* K2 *******/
		vector<int> b_e, b_d;
		// encontrando atores que podem participar de k2
        for(i = 0; i < nao_ap.size(); i++) {
		 	for (j = 0; j < e_p.size(); j++) {
		 		if(t[nao_ap[i]-1][e_p[j]-1]) {
		 			b_e.push_back(nao_ap[i]);
		 			break;
		 		}
		 	}
		 	for (j = 0; j < d_p.size(); j++) {
		 		if(t[nao_ap[i]-1][d_p[j]-1]) {
		 			b_d.push_back(nao_ap[i]);
		 			break;
		 		}
		 	}
		}
		
		for (i = 0; i < b_e.size(); i++) {
            int flag = 0, esquerda = 0;
            int ator = b_e[i] - 1;
            
            for (j = 0; j < e_p.size(); j++) {
                if (t[ator][e_p[j] - 1]) {
                    flag = j;
                    break;
                }
            }

            for (j = flag; j < e_p.size(); j++) {
                esquerda += 1 - t[ator][e_p[j] - 1];
            }
            K2 += custos[ator] * esquerda;
            
        }
        
        for (i = 0; i < b_d.size(); i++) {
            int flag = 0, direita = 0;
            int ator = b_d[i] - 1;
            
            for (j = 0; j < d_p.size(); j++) {
                if (t[ator][d_p[j] - 1]) {
                    flag = j;
                    break;
                }
            }
            
            for (j = flag; j < d_p.size(); j++) {
                direita += 1 - t[ator][d_p[j] - 1];
            }
            K2 += custos[ator] * direita;
            
        }
        
        if (K2 != 0) {
            cout << endl;
        }
		 		
		cout << "K2 = " << K2 << endl;


	}
	return K1 + K2;

}

void b_n_b(int raiz, int N, vector<int>& custos, vector<int>& s, vector<vector<int> >& t, int atores, int cenas) {
	priority_queue<pair<int, dados_no>, vector<pair<int, dados_no> >, comparador> nos_ativos; /* min_heap */
	int limitante_sup = 1000000, qtd_filhos, total_lim_inf = 0, pos = 0, i, j;
	pair<int, dados_no> no_aux;
	vector<int>::iterator it;

	no_aux.first = 0;
	no_aux.second.nivel = 0;
	no_aux.second.cena = 0;
	no_aux.second.cenas_disponiveis.resize(N, true);

	nos_ativos.push(no_aux);

	while(!nos_ativos.empty()){

		pair<int, dados_no> no = nos_ativos.top();
		nos_ativos.pop();

		/* quantidade de filhos que o no no nivel i pode ter */
		qtd_filhos = N - no.second.nivel;
		//cout << qtd_filhos << endl;

		/* se o limitante superior ainda nao foi definido */
		if(limitante_sup == 1000000) {
			/* se e um no folha da arvore */
			if (qtd_filhos == 0) {
				// cout << "etrei" << endl;
				vector<int> resposta;

				//for (int j = 0; j < no.second.melhor_solucao.size(); j++)
				//	cout << no.second.melhor_solucao[j] << " ";
				//cout << '\n';

				for(i = 0; i < no.second.melhor_solucao.size(); i+= 2) {
					resposta.push_back(no.second.melhor_solucao[i]);
				}
				for(i = no.second.melhor_solucao.size() -1; i > 0; i -= 2) {
					resposta.push_back(no.second.melhor_solucao[i]);
				}

				limitante_sup = no.first;

				cout << " Achei limitante_sup " << limitante_sup << endl;
				atualiza_solucao(resposta, no.first);
			} else {
				/* gera os filhos */
				for(i = 0; i < N; i++) {
					total_lim_inf = 0;

					/* se for possivel gerar o filho */
					if(no.second.cenas_disponiveis[i]) {
						no_aux.second.melhor_solucao = no.second.melhor_solucao;
						no_aux.second.melhor_solucao.push_back(i+1);
						total_lim_inf += limitantes(custos, no_aux.second.melhor_solucao, s, t, atores, cenas);
						no_aux.first = total_lim_inf;
						no_aux.second.nivel = no.second.nivel + 1;
						no_aux.second.cena = i+1;
						no_aux.second.cenas_disponiveis.resize(N);
						for(j = 0; j < no.second.cenas_disponiveis.size(); j++) {
							if (i != j) {
								no_aux.second.cenas_disponiveis[j] = no.second.cenas_disponiveis[j];
							} else {
								no_aux.second.cenas_disponiveis[j] = false;
							}
						}
						nos_ativos.push(no_aux);
						no_aux.second.melhor_solucao.clear();
						no_aux.second.melhor_solucao.resize(0);
					}

				}
			}
		} else {
			if (qtd_filhos == 0) {
				vector<int> resposta;
				for(i = 0; i < no.second.melhor_solucao.size(); i+= 2) {
					resposta.push_back(no.second.melhor_solucao[i]);
				}
				for(i = no.second.melhor_solucao.size() ; i > 0; i -= 2) {
					resposta.push_back(no.second.melhor_solucao[i]);
				}
				cout << "limt inf " << no.first << " sup " << limitante_sup << endl;
					if (no.first < melhor_custo) {
						limitante_sup = no.first;
						atualiza_solucao(resposta, no.first);
					}
			} else {
				for(i = 0; i < N; i++) {
					total_lim_inf = 0;
					if(no.second.cenas_disponiveis[i]) {
						no_aux.second.melhor_solucao = no.second.melhor_solucao;
						no_aux.second.melhor_solucao.push_back(i+1);
						total_lim_inf += limitantes(custos, no_aux.second.melhor_solucao, s, t, atores, cenas);
						if (total_lim_inf < melhor_custo) {
							no_aux.first = total_lim_inf;
							no_aux.second.nivel = no.second.nivel + 1;
							no_aux.second.cena = i+1;
							no_aux.second.cenas_disponiveis.resize(N);
							for(j = 0; j < no.second.cenas_disponiveis.size(); j++) {
								if (i != j) {
									no_aux.second.cenas_disponiveis[j] = no.second.cenas_disponiveis[j];
								} else {
									no_aux.second.cenas_disponiveis[j] = false;
								}
							}
							nos_ativos.push(no_aux);
							no_aux.second.melhor_solucao.clear();
							no_aux.second.melhor_solucao.resize(0);

						}

					}
				}
			}
		}
	}
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
	for (i = 0; i < atores; i++) {
		s[i] = 0;
		for(j = 0; j < cenas; j++) {
			s[i] += t[i][j];
		}
	}
	for (i = 0; i < atores; i++) {
		cout << "s " << s[i] << "c " << custos[i] << endl;
	}
	b_n_b(0, cenas, custos, s, t, atores, cenas);
	imprime_saida();

    return 0;
}
