int limitantes(vector<int>& custos, vector<int>& solucao_parcial, vector<int>& s, vector<vector<int> >& t, int atores, int cenas) {
	int e, d, total = 0, i, j, k, e_i = 0, d_i = 0;
	int tamanho_e = 0, ambos, qtd_cenas_e, qtd_cenas_d, l_e, l_d;
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
			ambos = 0;
			qtd_cenas_e = 0;
			qtd_cenas_d = 0;
			for(j = 0; j < e_p.size(); j++) {
				for(k = 0; k < d_p.size(); k++) {
					/* se tem um inicio em E(P) e tem um fim em D(P) */
					if(t[i][e_p[j]-1] && t[i][d_p[k]-1]) {
						ambos++;
					/* se tem inicio e fim em E(P) */
					} else if (t[i][e_p[j]-1]) {
						qtd_cenas_e++;
					/* se tem inicio e fim em D(P) */
					} else if (t[i][d_p[k]-1]) {
						qtd_cenas_d++;
					}
				}
			}
			/* se tem inicio em E(P) e fim em D(P) */
			if (ambos > 0) {
				conjunto_ap.push_back(i+1);
			/* se todas as cenas ja foram gravadas em E(P) ou D(P) */
			} else if (qtd_cenas_e == s[i] || qtd_cenas_d == s[i]) {
				conjunto_ap.push_back(i+1);
			/* se nao esta em A(P) */
			} else {
				nao_ap.push_back(i+1);
			}
		}

		/* encontrando o primeiro dia de gravacao e o ultimo ou seja e_i e d_i */
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
				for(j = 0; j < d_p.size(); j++) {
					if(t[conjunto_ap[i]-1][d_p[j]-1]) {
						e_i = cenas - j;
					}
				}
			/* se inicio e fim esta em E(P) */
			} else if(d_i == 0) {
				for(j = 0; j < e_p.size(); j++) {
					if(t[conjunto_ap[i]-1][e_p[j]-1]) {
						d_i = j + 1;
					}
				}
			}
			total += custos[conjunto_ap[i]-1] * (d_i - e_i + 1 - s[conjunto_ap[i]-1]);
		}
