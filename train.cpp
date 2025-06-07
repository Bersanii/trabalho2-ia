#include "grafo.h"
#include <cmath>
#include "mymath.h"
#include <fstream>
#include <iostream>

double costBOW(graph& G){
    double out = 0;
    //Itera sobre os nós
    for (int i=0; i<G.size(); i++){
        //Vetor para armazenar a soma dos vizinhos
        double* neighborSum = zero(G.dim);
        //Itera sobre vizinhos
        for (int j=0; j<G.nd[i]->neighbors.size(); j++){
            out += inner(G.nd[i]->vec,G.nd[i]->neighbors[j]->vec,G.dim);
            for (int s=0; s<G.dim; s++){
                neighborSum[s] += G.nd[i]->neighbors[j]->vec[s];
            }
        }
        //Itera sobre todos os nós para calcular fator de normalização
        double Z = 0;
        for (int p=0; p<G.nd.size(); p++){
            Z += exp(inner(G.nd[p]->vec,neighborSum,G.dim));
        }
        delete[] neighborSum;
        out -= log(Z);
    }
    return -out;
}

vector<double*> gradBOW(graph& G, ifstream& F){
    vector<double*> out;
    for (int i=0; i<G.size(); i++){
        double* aux = zero(G.dim);
        out.push_back(aux);
    }
    while(!F.eof()){
        string S;
        getline(F,S,' ');
        string V;
        getline(F,V,' ');
        string T;
        getline(F,T);

        //Indices
        int pV = G.where(V);
        int pS = G.where(S);
        int pT = G.where(T);

        //Gradiente para a resposta (verbo)
        double* prob = G.probGivenContext(S,T);
        double *grad_V = zero(G.dim);
        for (int s=0; s<G.dim; s++){
            grad_V[s] = (1-prob[pV])*G.nd[pS]->vec[s]+G.nd[pT]->vec[s];
        }
        iadd(out[pV],grad_V,G.dim);
        delete[] grad_V;

        //Gradiente para o primeiro substantivo
        double* avgS = G.vecAverage(prob);
        imul(avgS,-1,G.dim);
        iadd(out[pS],G.nd[pV]->vec,G.dim);
        iadd(out[pS],avgS,G.dim);
        delete[] avgS;
        double *auxS = zero(G.dim);
        copy(G.nd[pS]->vec,G.nd[pS]->vec+(G.dim-1),auxS);
        imul(auxS,-prob[pS],G.dim);
        iadd(out[pS],auxS,G.dim);
        delete[] auxS;

        //Gradiente para o segundo substantivo
        double* avgT = G.vecAverage(prob);
        imul(avgT,-1,G.dim);
        iadd(out[pT],G.nd[pV]->vec,G.dim);
        iadd(out[pT],avgT,G.dim);
        delete[] avgT;
        double *auxT = zero(G.dim);
        copy(G.nd[pT]->vec,G.nd[pT]->vec+(G.dim-1),auxT);
        imul(auxT,-prob[pT],G.dim);
        iadd(out[pT],auxT,G.dim);
        delete[] auxT;
    }

    return out;
}

void trainBOW(graph* G, ifstream& F, double lr, int tmax){
    int t = 0;
    while (t<tmax){
        vector<double*> grad = gradBOW(*G,F);
        for (int i=0; i<G->size(); i++){
            for (int s=0; s<G->dim; s++){
                G->nd[i]->vec[s] += lr*grad[i][s];
            }
            // cout << costBOW(*G) << endl;
        }
        t++;
    }
}