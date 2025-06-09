#include "grafo.h"
#include <fstream>

double costBOW(graph& G);
vector<double*> gradBOW(graph& G, ifstream& F);
void trainBOW(graph* G, ifstream& F, double lr, int tmax);

// Para a topologia S -> V -> T
vector<double*> grad_S_V_T(graph& G, ifstream& F);
void train_S_V_T(graph* G, ifstream& F, double lr, int tmax);

// Para a topologia S <- V <- T
vector<double*> grad_V_ST(graph& G, ifstream& F);
void train_V_ST(graph* G, ifstream& F, double lr, int tmax);

// S -> V <- T (duas relacoes: P(S|V) e P(T|V))
vector<double*> grad_Sto_V_toT(graph& G, ifstream& F);
void train_Sto_V_toT(graph* G, ifstream& F, double lr, int tmax);