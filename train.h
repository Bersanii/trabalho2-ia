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