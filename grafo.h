#include "nodes.h"

#ifndef GRAFO_H
#define GRAFO_H

#define V_CENTRAL 0 // S <- V -> T (V é a resposta, S e T são o contexto)
#define Sto_Vto_T 1 // // S -> V -> T (duas relações: P(S|V) e P(V|T))
#define Sto_V_toT 2 // S -> V <- T (duas relacoes: P(S|V) e P(T|V))
#define S_toV_toT 3 // S <- V <- T (duas relacoes P(V|S) e P(T|V))

class graph{
public:
    int dim;
    vector<node*> nd;

    graph(int dim){
        this->dim = dim;
    }

    int size(){
        return this->nd.size();
    }

    bool isIn(string S); //Checa se string S já foi armazenada em algum nó
    void append(string S, string type); //Adiciona S no final de nd
    int where(string S);


    void load(ifstream& F); //Carrega base de dados
    void load_S_V_T(ifstream& F);     // Para S -> V -> T
    void load_S_V_T_rev(ifstream& F); // Para S <- V <- T
    void load_Sto_V_toT(ifstream& F); // S -> V <- T (duas relacoes: P(S|V) e P(T|V))
    void printNodes(); // Imprime nós
    void printRelations(string S);
    void connect(string S, string T); //Conecta duas strings

    void writeVecs(ofstream& F);

    double* probGivenContext(string S, string T);
    double* vecAverage(double* p);
};

#endif