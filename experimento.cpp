#include "Experimento.hpp"
#include "Centrality.hpp"
#include <iostream>
#include <vector>

void Experimento::evaluarImpactoGlobal(const Graph& g, const std::string& estado) {
    std::cout << "\n--- Métrica Globales: " << estado << " ---" << std::endl;
    double asp = Centrality::calculateAverageShortestPath(g);
    double diameter = Centrality::calculateNetworkDiameter(g);
    std::cout << "Average Shortest Path : " << asp << std::endl;
    std::cout << "Network Diameter      : " << diameter << std::endl;
}

void Experimento::ejecutarPerturbacion(Graph& g) {
    std::cout << "\n========================================";
    std::cout << "\n INICIANDO EXPERIMENTO DE PERTURBACION";
    std::cout << "\n========================================\n";

    evaluarImpactoGlobal(g, "GRAFO ORIGINAL");
    
    auto rankingBetweenness = Centrality::calculateBetweennessCentrality(g).first;
    auto rankingDegree = Centrality::calculateDegreeCentrality(g).first;

    if (rankingBetweenness.empty() || rankingDegree.empty()) return;

    int nodoTop = rankingBetweenness.front().first; 
    int nodoBottom = rankingDegree.back().first;    

    // 1. Añadir Arista
    std::cout << "\n[+] ANADIENDO ARISTA: Conectando nodo " << nodoTop << " con " << nodoBottom << std::endl;
    g.addEdge(nodoTop, nodoBottom, 1.0);
    evaluarImpactoGlobal(g, "TRAS ANADIR ARISTA");
    
    g.removeEdge(nodoTop, nodoBottom); // Restaurar

    // 2. Quitar Arista
    std::vector<int> vecinos = g.getNeighbors(nodoTop);
    if (!vecinos.empty()) {
        int vecinoCritico = vecinos[0];
        std::cout << "\n[-] QUITANDO ARISTA: Desconectando nodo " << nodoTop << " de " << vecinoCritico << std::endl;
        g.removeEdge(nodoTop, vecinoCritico);
        evaluarImpactoGlobal(g, "TRAS QUITAR ARISTA CRITICA");
        g.addEdge(nodoTop, vecinoCritico, 1.0); // Restaurar
    }
}