#include "Experimento.hpp"
#include "Centrality.hpp"
#include <iostream>
#include <vector>

void Experimento::evaluarImpactoGlobal(const Graph& g, const std::string& estado) {
    std::cout << "\n--- Metricas Globales: " << estado << " ---" << std::endl;
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
    
    // Obtenemos los rankings para tomar decisiones
    auto rankingBetweenness = Centrality::calculateBetweennessCentrality(g).first;
    auto rankingDegree = Centrality::calculateDegreeCentrality(g).first;

    if (rankingBetweenness.size() < 2 || rankingDegree.size() < 2) {
        std::cout << "El grafo es demasiado pequeno para el experimento." << std::endl;
        return;
    }

    // --- IDENTIFICACION DE LUGARES ---
    int nodoCentro = rankingBetweenness.front().first;               // El nodo más crítico
    int nodoPeriferia1 = rankingDegree[rankingDegree.size() - 1].first; // El nodo menos conectado
    int nodoPeriferia2 = rankingDegree[rankingDegree.size() - 2].first; // El segundo menos conectado

    // =======================================================
    // PRUEBA 1: AÑADIR ARISTAS EN DISTINTOS LUGARES
    // =======================================================
    
    // Lugar A: Conectando el Centro con la Periferia
    std::cout << "\n[+] ANADIENDO ARISTA (Lugar 1): Conectando CENTRO (" << nodoCentro 
              << ") con PERIFERIA (" << nodoPeriferia1 << ")." << std::endl;
    g.addEdge(nodoCentro, nodoPeriferia1, 1.0);
    evaluarImpactoGlobal(g, "TRAS ANADIR ARISTA EN EL CENTRO");
    g.removeEdge(nodoCentro, nodoPeriferia1); // Restaurar grafo

    // Lugar B: Conectando dos nodos de la Periferia (Impacto local)
    std::cout << "\n[+] ANADIENDO ARISTA (Lugar 2): Conectando dos nodos en la PERIFERIA (" 
              << nodoPeriferia1 << " y " << nodoPeriferia2 << ")." << std::endl;
    g.addEdge(nodoPeriferia1, nodoPeriferia2, 1.0);
    evaluarImpactoGlobal(g, "TRAS ANADIR ARISTA EN LA PERIFERIA");
    g.removeEdge(nodoPeriferia1, nodoPeriferia2); // Restaurar grafo


    // =======================================================
    // PRUEBA 2: QUITAR ARISTAS EN DISTINTOS LUGARES
    // =======================================================

    // Lugar A: Desconectando el Centro
    std::vector<int> vecinosCentro = g.getNeighbors(nodoCentro);
    if (!vecinosCentro.empty()) {
        int vecinoCritico = vecinosCentro[0];
        std::cout << "\n[-] QUITANDO ARISTA (Lugar 1): Desconectando el CENTRO (" << nodoCentro 
                  << ") de su vecino " << vecinoCritico << "." << std::endl;
        g.removeEdge(nodoCentro, vecinoCritico);
        evaluarImpactoGlobal(g, "TRAS QUITAR ARISTA EN EL CENTRO");
        g.addEdge(nodoCentro, vecinoCritico, 1.0); // Restaurar grafo
    }

    // Lugar B: Desconectando la Periferia (Nodo irrelevante)
    // Buscamos un nodo con el menor Betweenness posible que tenga al menos 1 vecino
    int nodoAislable = -1;
    int vecinoAislable = -1;
    for(int i = rankingBetweenness.size() - 1; i >= 0; i--) {
        int n = rankingBetweenness[i].first;
        std::vector<int> vecinos = g.getNeighbors(n);
        if(!vecinos.empty()) {
            nodoAislable = n;
            vecinoAislable = vecinos[0];
            break;
        }
    }

    if (nodoAislable != -1) {
        std::cout << "\n[-] QUITANDO ARISTA (Lugar 2): Desconectando la PERIFERIA (" << nodoAislable 
                  << ") de su vecino " << vecinoAislable << "." << std::endl;
        g.removeEdge(nodoAislable, vecinoAislable);
        evaluarImpactoGlobal(g, "TRAS QUITAR ARISTA EN LA PERIFERIA");
        g.addEdge(nodoAislable, vecinoAislable, 1.0); // Restaurar grafo
    }

    std::cout << "\n========================================\n";
}