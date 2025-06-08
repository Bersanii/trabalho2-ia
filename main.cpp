#include "grafo.h"
#include "train.h"
#include <fstream>
#include <iostream>
#include <string>

void run_experiment(int dim, int topology_type, double lr, int tmax) {
    ifstream F("data.txt");
    if (!F.is_open()) {
        cerr << "Erro ao abrir data.txt" << endl;
        return;
    }

    graph G(dim);
    string topology_name;

    switch (topology_type) {
        case 0: // S <- V -> T (Original BOW)
            topology_name = "S_V_T_bow";
            cout << "Carregando topologia: " << topology_name << endl;
            G.load(F);
            break;
        case 1: // S -> V -> T
            topology_name = "S_V_T_seq";
            cout << "Carregando topologia: " << topology_name << endl;
            G.load_S_V_T(F);
            break;
        case 2: // S <- V <- T
            topology_name = "V_ST";
            cout << "Carregando topologia: " << topology_name << endl;
            G.load_S_V_T_rev(F);
            break;
        default:
            cerr << "Topologia inválida!" << endl;
            F.close();
            return;
    }
    F.clear();
    F.seekg(0, ios::beg);

    cout << "Iniciando treinamento para dim=" << dim << " topologia=" << topology_name << endl;
    switch (topology_type) {
        case 0:
            trainBOW(&G, F, lr, tmax);
            break;
        case 1:
            train_S_V_T(&G, F, lr, tmax);
            break;
        case 2:
            train_V_ST(&G, F, lr, tmax);
            break;
    }
    cout << "Treinamento concluído." << endl;

    string out_filename = "vecs_dim" + to_string(dim) + "_" + topology_name + ".dat";
    ofstream V(out_filename);
    if(V.is_open()){
        G.writeVecs(V);
        V.close();
        cout << "Vetores salvos em: " << out_filename << endl;
    } else {
        cerr << "Erro ao abrir arquivo de saída: " << out_filename << endl;
    }

    F.close();
    cout << "------------------------------------------" << endl;
}


int main() {
    int dims[] = {2, 4, 8};
    double learning_rate = 0.01;
    int max_iterations = 200;

    for (int dim : dims) {
        // Topologia 0: S <- V -> T (CBOW Original)
        // run_experiment(dim, 0, learning_rate, max_iterations);
        
        // Topologia 1: S -> V -> T
        // run_experiment(dim, 1, learning_rate, max_iterations);

        // Topologia 2: S <- V <- T
        run_experiment(dim, 2, learning_rate, max_iterations);
    }

    return 0;
}