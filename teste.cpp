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
    // A segunda linha contem o custo (apenas de dias de espera!)
    vector<int> rFinal;
    rFinal.resize (melhor_solucao.size());
    for (int j = 0; j < melhor_solucao.size(); j++) {
        rFinal[melhor_solucao[j] - 1] = j + 1;
    }
    for (int j = 0; j < rFinal.size(); j++)
        cout << rFinal[j] << " ";
    cout << endl << melhor_custo << endl;
    cout << melhor_limitante_inf << endl;
    cout << nos_explorados << endl;
}

void atualiza_solucao(const vector<int> &solucao, int custo) {
    escrevendo = 1;
    melhor_solucao = solucao;
    melhor_custo = custo;
	//for (int j = 0; j < melhor_solucao.size(); j++)
        //cout << melhor_solucao[j] << " ";
    // A segunda linha contem o custo (apenas de dias de espera!)
    //cout << endl << melhor_custo << endl;
	//cout << "melhor custo " << melhor_custo << endl;
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

void empacotamento (vector<int>& solucao_parcial, vector<vector<int> >& t, int cenas, vector<int>& atores, vector<int>& pacote) {
    vector<int> cena_pendente;
    cena_pendente.resize(cenas);
    vector<int> l_cenas;
    vector<int> c_cenas;
    vector<int> aOK;
    aOK.resize(atores.size());
    
    for(int j = 0; j < aOK.size(); j++) {
       aOK[j] = 0;
    }
    
    for (int i = 0; i < cenas; i++) {
        cena_pendente[i] = 1;
    }
    for (int i = 0; i < solucao_parcial.size(); i++) {
        cena_pendente[solucao_parcial[i] - 1] = 0;
    }
    for (int i = 0; i < cenas; i++) {
        if(cena_pendente[i] == 1)
            l_cenas.push_back(i + 1);
    }
    c_cenas.resize(l_cenas.size());
    
    for (int i = 0; i < l_cenas.size(); i++) {
        c_cenas[i] = 0;
        for(int j = 0; j < atores.size(); j++) {
            c_cenas[i] += t[atores[j] - 1][l_cenas[i] - 1];
        }
    }
    
    // ordenando as cenas por ordem de custo
    for (int k = 0; k < l_cenas.size(); k++) {
        int menor = k;
        for(int i = k+1; i < l_cenas.size(); i++) {
            if (c_cenas[i] < c_cenas[menor]) {
                menor = i;
            }
        }
        
        if (menor != k) {
            int aux = c_cenas[k];
            c_cenas[k] = c_cenas[menor];
            c_cenas[menor] = aux;
            aux = l_cenas[k];
            l_cenas[k] = l_cenas[menor];
            l_cenas[menor] = aux;
        }
    }
    
    for(int i = 0; i < l_cenas.size(); i++) {
        bool flag = true;
        // checa se a cena pode ser escolhida
        for (int j = 0; j < atores.size(); j++) {
            if (((aOK[j] == 1) && (t[atores[j] - 1][l_cenas[i] - 1] == 1)) || (c_cenas[i] == 0))    
                flag = false;
        }
        // se pode escolha-a
        if (flag) {
            pacote.push_back(l_cenas[i]);
            for (int j = 0; j < atores.size(); j++) {
                if ((aOK[j] == 0) && (t[atores[j] - 1][l_cenas[i] - 1] == 1))
                    aOK[j] = 1;
            }
        }
    }  
}

int limitantes(vector<int>& custos, vector<int>& solucao_parcial, vector<int>& s, vector<vector<int> >& t, int atores, int cenas) {
	int e, d, K1 = 0, K2 = 0, K3 = 0, K4 = 0, i, j, k, e_i = 0, d_i = 0;
	int tamanho_e = 0, qtd_cenas_e, qtd_cenas_d, l_e, l_d;
	vector<int> e_p, d_p, conjunto_ap, nao_ap;
    nos_explorados++;
    
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
            } else if ( ( (qtd_cenas_e == s[i]) && (qtd_cenas_e > 0) ) || ( (qtd_cenas_d == s[i]) && (qtd_cenas_d > 0) ) ) {
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
        
        /******* K3 *******/
        if (b_e.size() > 1) {
            vector<int> pacote1;
            empacotamento (solucao_parcial, t, cenas, b_e, pacote1);
           
            if (pacote1.size() > 1) {
                vector<int> custoCenas;
                custoCenas.resize(pacote1.size());
                
                // calculando o custo das cenas
                for (i = 0; i < custoCenas.size(); i++) {
                    custoCenas[i] = 0;
                    for(j = 0; j < b_e.size(); j++) {
                        custoCenas[i] += custos[b_e[j] - 1] * t[b_e[j] - 1][pacote1[i] - 1];
                    }
                }
                
                for (int k = 0; k < custoCenas.size(); k++) {
                    int menor = k;
                    for(int i = k+1; i < custoCenas.size(); i++) {
                        if (custoCenas[i] < custoCenas[menor]) {
                            menor = i;
                        }
                    }
                    
                    if (menor != k) {
                        int aux = custoCenas[k];
                        custoCenas[k] = custoCenas[menor];
                        custoCenas[menor] = aux;
                        aux = pacote1[k];
                        pacote1[k] = pacote1[menor];
                        pacote1[menor] = aux;
                    }
                }
                
                for (int k = custoCenas.size() - 1; k >= 0; k--) {
                    K3 += custoCenas[k] * (custoCenas.size() - 1 - k);
                }
                
            }
            
        }
        
        /******* K4 *******/
        if (b_d.size() > 1) {
            vector<int> pacote;
            empacotamento (solucao_parcial, t, cenas, b_d, pacote);
            
            if (pacote.size() > 1) {
                vector<int> custoCenas;
                custoCenas.resize(pacote.size());
                
                // calculando o custo das cenas
                for (i = 0; i < custoCenas.size(); i++) {
                    custoCenas[i] = 0;
                    for(j = 0; j < b_d.size(); j++) {
                        custoCenas[i] += custos[b_d[j] - 1] * t[b_d[j] - 1][pacote[i] - 1];
                    }
                }
                
                for (int k = 0; k < custoCenas.size(); k++) {
                    int menor = k;
                    for(int i = k+1; i < custoCenas.size(); i++) {
                        if (custoCenas[i] < custoCenas[menor]) {
                            menor = i;
                        }
                    }
                    
                    if (menor != k) {
                        int aux = custoCenas[k];
                        custoCenas[k] = custoCenas[menor];
                        custoCenas[menor] = aux;
                        aux = pacote[k];
                        pacote[k] = pacote[menor];
                        pacote[menor] = aux;
                    }
                    
                    for (int k = custoCenas.size() - 1; k >= 0; k--) {
                        K4 += custoCenas[k] * (custoCenas.size() - 1 - k);
                    }
                }
            }
        }
        

	}
	return K1 + K2 + K3 + K4;

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
        
        while ((!nos_ativos.empty()) && (no.first >= limitante_sup)) {
            no = nos_ativos.top();
            nos_ativos.pop();
        }
        
		/* quantidade de filhos que o no no nivel i pode ter */
		qtd_filhos = N - no.second.nivel;

		/* se o limitante superior ainda nao foi definido */
		if(limitante_sup == 1000000) {
			/* se e um no folha da arvore */
			if (qtd_filhos == 0) {

				vector<int> resposta;

				for(i = 0; i < no.second.melhor_solucao.size(); i+= 2) {
					resposta.push_back(no.second.melhor_solucao[i]);
				}
				for(i = no.second.melhor_solucao.size() -1; i > 0; i -= 2) {
					resposta.push_back(no.second.melhor_solucao[i]);
				}
                
                melhor_limitante_inf = no.first;
                limitante_sup = no.first;

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
				for(i = no.second.melhor_solucao.size() - 1; i > 0; i -= 2) {
					resposta.push_back(no.second.melhor_solucao[i]);
				}
				
					if (no.first < melhor_custo) {
                       melhor_limitante_inf = no.first;
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
// 	for (i = 0; i < atores; i++) {
// 		cout << "s " << s[i] << "c " << custos[i] << endl;
// 	}
	b_n_b(0, cenas, custos, s, t, atores, cenas);
	imprime_saida();

    return 0;
}
