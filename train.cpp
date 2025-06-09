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


/*
    Implementação para a topologia S -> V -> T
    
    duas relações: P(S|V) e P(V|T)
*/
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

        /*
            P(S|V)
            S é resposta, V é contexto
        */

        // Gradiente para a resposta S(dado o contexto V)
        double* prob_S = G.probGivenContext(V, V); // Contexto é apenas V
        double* grad_S = zero(G.dim);
        for(int s=0; s<G.dim; s++){
            grad_S[s] = (1 - prob_S[pS]) * G.nd[pV]->vec[s];
        }
        iadd(out[pS], grad_S, G.dim);
        delete[] grad_S;

        // Gradiente para o contexto V(dado a resposta S)
        double* avg_V = G.vecAverage(prob_S);
        imul(avg_V, -1, G.dim);
        iadd(out[pV], G.nd[pS]->vec, G.dim);
        iadd(out[pV], avg_V, G.dim);
        delete[] avg_V;
        double* auxV = zero(G.dim);
        copy(G.nd[pV]->vec, G.nd[pV]->vec + (G.dim - 1), auxV);
        imul(auxV, -prob_S[pV], G.dim);
        iadd(out[pS], auxV, G.dim);
        delete[] auxV;
        delete[] prob_S;


        // -----------------------------------------




        /*
            P(V|T))
            V é resposta, T é contexto
        
        */

        // Gradiente para a resposta V(dado contexto T)
        double* prob_V = G.probGivenContext(T, T); // Contexto é apenas V
        double* grad_V = zero(G.dim);
        for (int s=0; s<G.dim; s++){
            grad_V[s] = (1 - prob_V[pV]) * G.nd[pT]->vec[s];
        }
        iadd(out[pV], grad_V, G.dim);
        delete[] grad_V;


        // Gradiente para o contexto T (dado a resposta V)
        double* avg_T = G.vecAverage(prob_V);
        imul(avg_T, -1, G.dim);
        // Note que o gradiente para V é acumulado
        iadd(out[pT], G.nd[pV]->vec, G.dim);
        iadd(out[pT], avg_T, G.dim); 
        delete[] avg_T;
        double* auxT = zero(G.dim);
        copy(G.nd[pT]->vec, G.nd[pT]->vec + (G.dim - 1), auxT);
        imul(auxT, -prob_V[pT], G.dim);
        iadd(out[pT], auxT, G.dim);
        delete[] auxT;
        delete[] prob_V;
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


/*
    Implementação para a topologia S <- V <- T

    duas relacoes P(V|S) e P(T|V)

*/
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

        /*
            P(V|S)
            V é resposta, S é contexto
        */

        // Gradiente para a resposta V(dado o contexto S)
        double* prob_V = G.probGivenContext(S, S); // Contexto é apenas S
        double* grad_V = zero(G.dim);
        for(int s=0; s<G.dim; s++){
            grad_V[s] = (1 - prob_V[pV]) * G.nd[pS]->vec[s];
        }
        iadd(out[pV], grad_V, G.dim);  
        delete[] grad_V;

        // Gradiente para o contexto S(dado a resposta V)
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


        // -----------------------------------------




        /*
            P(T|V)
            T é resposta, V é contexto
        
        */

        // Gradiente para a resposta T(dado contexto V)
        double* prob_T = G.probGivenContext(V, V); // Contexto é apenas V
        double* grad_T = zero(G.dim);
        for (int s=0; s<G.dim; s++){
            grad_T[s] = (1 - prob_T[pT]) * G.nd[pV]->vec[s];
        }
        iadd(out[pT], grad_T, G.dim);
        delete[] grad_T;


        // Gradiente para o contexto V (dado a resposta T)
        double* avg_V = G.vecAverage(prob_T);
        imul(avg_V, -1, G.dim);
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

vector<double*> grad_Sto_V_toT(graph& G, ifstream& F){
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

        /*
            P(S|V)
            S é resposta, V é contexto
        */

        // Gradiente para a resposta S(dado o contexto V)
        double* prob_S = G.probGivenContext(V, V); // Contexto é apenas V
        double* grad_S = zero(G.dim);
        for(int s=0; s<G.dim; s++){
            grad_S[s] = (1 - prob_S[pS]) * G.nd[pV]->vec[s];
        }
        iadd(out[pS], grad_S, G.dim);
        delete[] grad_S;

        // Gradiente para o contexto V(dado a resposta S)
        double* avg_V = G.vecAverage(prob_S);
        imul(avg_V, -1, G.dim);
        iadd(out[pV], G.nd[pS]->vec, G.dim);
        iadd(out[pV], avg_V, G.dim);
        delete[] avg_V;
        double* auxV = zero(G.dim);
        copy(G.nd[pV]->vec, G.nd[pV]->vec + (G.dim - 1), auxV);
        imul(auxV, -prob_S[pV], G.dim);
        iadd(out[pS], auxV, G.dim);
        delete[] auxV;
        delete[] prob_S;


        // -----------------------------------------

        /*
            P(T|V)
            T é resposta, V é contexto
        
        */

        // Gradiente para a resposta T(dado contexto V)
        double* prob_T = G.probGivenContext(V, V); // Contexto é apenas V
        double* grad_T = zero(G.dim);
        for (int s=0; s<G.dim; s++){
            grad_T[s] = (1 - prob_T[pT]) * G.nd[pV]->vec[s];
        }
        iadd(out[pT], grad_T, G.dim);
        delete[] grad_T;


        // Gradiente para o contexto V (dado a resposta T)
        avg_V = G.vecAverage(prob_T);
        imul(avg_V, -1, G.dim);
        iadd(out[pV], G.nd[pT]->vec, G.dim);
        iadd(out[pV], avg_V, G.dim); 
        delete[] avg_V;
        auxV = zero(G.dim);
        copy(G.nd[pV]->vec, G.nd[pV]->vec + (G.dim - 1), auxV);
        imul(auxV, -prob_T[pV], G.dim);
        iadd(out[pV], auxV, G.dim);
        delete[] auxV;
        delete[] prob_T;
    }

    return out;

}



void train_Sto_V_toT(graph* G, ifstream& F, double lr, int tmax){
    int t = 0;
    while (t < tmax){
        F.clear();
        F.seekg(0, ios::beg);
        vector<double*> grad = grad_Sto_V_toT(*G, F);
        for (int i=0; i<G->size(); i++){
            for (int s=0; s<G->dim; s++){
                G->nd[i]->vec[s] += lr * grad[i][s];
            }
            delete[] grad[i];
        }
        t++;
    }

}


/*
    Implementação para a topologia S -> V <- T

    duas relacoes: P(S|V) e P(T|V)

*/


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

