#include "grafo.h"
#include <fstream>

double costBOW(graph& G);
vector<double*> gradBOW(graph& G, ifstream& F);
void trainBOW(graph* G, ifstream& F, double lr, int tmax);