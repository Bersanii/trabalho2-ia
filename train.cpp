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

// =================================================================
// Implementação para a topologia S -> V -> T
// =================================================================
vector<double*> grad_S_V_T(graph& G, ifstream& F){
    vector<double*> out;
    for (int i=0; i<G.size(); i++){
        out.push_back(zero(G.dim));
    }

    while(!F.eof()){
        string S, V, T;
        getline(F,S,' ');
        getline(F,V,' ');
        getline(F,T);
        int pS = G.where(S);
        int pV = G.where(V);
        int pT = G.where(T);

        // --- Relação S -> V ---
        // S é contexto, V é resposta
        double* prob_V = G.probGivenContext(S, S); // Contexto é apenas S
        double* grad_V = zero(G.dim);
        for(int s=0; s<G.dim; s++){
            grad_V[s] = (1 - prob_V[pV]) * G.nd[pS]->vec[s];
        }
        iadd(out[pV], grad_V, G.dim);
        delete[] grad_V;

        double* avg_S = G.vecAverage(prob_V);
        imul(avg_S, -1, G.dim);
        iadd(out[pS], G.nd[pV]->vec, G.dim);
        iadd(out[pS], avg_S, G.dim);
        delete[] avg_S;
        double* auxS = zero(G.dim);
        copy(G.nd[pS]->vec, G.nd[pS]->vec + (G.dim - 1), auxS);
        imul(auxS, -prob_V[pS], G.dim);
        iadd(out[pS], auxS, G.dim);
        delete[] auxS;
        delete[] prob_V;

        // --- Relação V -> T ---
        // V é contexto, T é resposta
        double* prob_T = G.probGivenContext(V, V); // Contexto é apenas V
        double* grad_T = zero(G.dim);
        for (int s=0; s<G.dim; s++){
            grad_T[s] = (1 - prob_T[pT]) * G.nd[pV]->vec[s];
        }
        iadd(out[pT], grad_T, G.dim);
        delete[] grad_T;

        double* avg_V = G.vecAverage(prob_T);
        imul(avg_V, -1, G.dim);
        // Note que o gradiente para V é acumulado
        iadd(out[pV], G.nd[pT]->vec, G.dim);
        iadd(out[pV], avg_V, G.dim);
        delete[] avg_V;
        double* auxV = zero(G.dim);
        copy(G.nd[pV]->vec, G.nd[pV]->vec + (G.dim - 1), auxV);
        imul(auxV, -prob_T[pV], G.dim);
        iadd(out[pV], auxV, G.dim);
        delete[] auxV;
        delete[] prob_T;
    }

    return out;
}

void train_S_V_T(graph* G, ifstream& F, double lr, int tmax){
    int t = 0;
    while (t < tmax){
        F.clear();
        F.seekg(0, ios::beg);
        vector<double*> grad = grad_S_V_T(*G, F);
        for (int i=0; i<G->size(); i++){
            for (int s=0; s<G->dim; s++){
                G->nd[i]->vec[s] += lr * grad[i][s];
            }
            delete[] grad[i];
        }
        t++;
    }
}

// =================================================================
// Implementação para a topologia S <- V <- T
// =================================================================
vector<double*> grad_V_ST(graph& G, ifstream& F){
    vector<double*> out;
    for (int i=0; i<G.size(); i++){
        out.push_back(zero(G.dim));
    }

    while(!F.eof()){
        string S, V, T;
        getline(F,S,' ');
        getline(F,V,' ');
        getline(F,T);
        int pS = G.where(S);
        int pV = G.where(V);
        int pT = G.where(T);

        // V é o contexto para S e T
        double* prob = G.probGivenContext(V, V);

        // --- Gradiente para S (resposta) ---
        double* grad_S = zero(G.dim);
        for(int s=0; s<G.dim; s++){
            grad_S[s] = (1 - prob[pS]) * G.nd[pV]->vec[s];
        }
        iadd(out[pS], grad_S, G.dim);
        delete[] grad_S;

        // --- Gradiente para T (resposta) ---
        double* grad_T = zero(G.dim);
        for(int s=0; s<G.dim; s++){
            grad_T[s] = (1 - prob[pT]) * G.nd[pV]->vec[s];
        }
        iadd(out[pT], grad_T, G.dim);
        delete[] grad_T;

        // --- Gradiente para V (contexto) ---
        double* avg = G.vecAverage(prob);
        imul(avg, -1, G.dim);
        // V é contexto para S
        iadd(out[pV], G.nd[pS]->vec, G.dim);
        iadd(out[pV], avg, G.dim);
        // V é contexto para T
        iadd(out[pV], G.nd[pT]->vec, G.dim);
        iadd(out[pV], avg, G.dim);
        delete[] avg;

        double* auxV1 = zero(G.dim);
        copy(G.nd[pV]->vec, G.nd[pV]->vec+(G.dim-1), auxV1);
        imul(auxV1, -prob[pS], G.dim);
        iadd(out[pV], auxV1, G.dim);
        delete[] auxV1;
        
        double* auxV2 = zero(G.dim);
        copy(G.nd[pV]->vec, G.nd[pV]->vec+(G.dim-1), auxV2);
        imul(auxV2, -prob[pT], G.dim);
        iadd(out[pV], auxV2, G.dim);
        delete[] auxV2;

        delete[] prob;
    }
    return out;
}

void train_V_ST(graph* G, ifstream& F, double lr, int tmax){
    int t = 0;
    while (t < tmax){
        F.clear();
        F.seekg(0, ios::beg);
        vector<double*> grad = grad_V_ST(*G, F);
        for (int i=0; i<G->size(); i++){
            for (int s=0; s<G->dim; s++){
                G->nd[i]->vec[s] += lr * grad[i][s];
            }
            delete[] grad[i];
        }
        t++;
    }
}