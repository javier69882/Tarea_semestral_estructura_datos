#include "Experimento.hpp"
#include "Centrality.hpp"
#include <iostream>
#include <vector>
#include <iomanip>
#include <fstream>

void Experimento::evaluarImpactoGlobal(const Graph& g, const std::string& nombreDataset, const std::string& estado, const std::string& arista, std::ofstream& csv) {
    bool isDirected = (nombreDataset.find("net") != std::string::npos);

    auto resDeg = Centrality::calculateDegreeCentrality(g, isDirected);
    double deg = resDeg.first.empty() ? 0.0 : resDeg.first.front().second;

    auto resBet = Centrality::calculateBetweennessCentrality(g);
    double bet = resBet.first.empty() ? 0.0 : resBet.first.front().second;

    auto resClo = Centrality::calculateClosenessCentrality(g);
    double clo = resClo.first.empty() ? 0.0 : resClo.first.front().second;

    auto resPr = Centrality::calculatePageRank(g);
    double pr = resPr.first.empty() ? 0.0 : resPr.first.front().second;

    auto resEig = Centrality::calculateEigenvectorCentrality(g);
    double eig = resEig.first.empty() ? 0.0 : resEig.first.front().second;

    double asp = Centrality::calculateAverageShortestPath(g);
    double dia = Centrality::calculateNetworkDiameter(g);

    // Se escribe la arista en la nueva columna
    csv << nombreDataset << "," << estado << "," << arista << ","
        << deg << "," << bet << "," << clo << ","
        << pr << "," << asp << "," << dia << "," << eig << "\n";
}

void Experimento::ejecutarPerturbacion(Graph& g, const std::string& nombreDataset, std::ofstream& csv, const MapeoGrafo& traductor) {
    std::cout << "\n========================================";
    std::cout << "\n INICIANDO PERTURBACION: " << nombreDataset;
    std::cout << "\n========================================\n";

    std::cout << "-> Evaluando GRAFO ORIGINAL..." << std::endl;
    // Para el original, la arista es "Ninguna"
    evaluarImpactoGlobal(g, nombreDataset, "Original", "Ninguna", csv);
    
    bool isDirected = (nombreDataset.find("net") != std::string::npos);
    auto rankingBetweenness = Centrality::calculateBetweennessCentrality(g).first;
    auto rankingDegree = Centrality::calculateDegreeCentrality(g, isDirected).first;

    if (rankingBetweenness.empty() || rankingDegree.empty()) return;

    // LUGAR 1: HUB
    int nodoHub = rankingBetweenness.front().first; 
    std::vector<int> vecinosHub = g.getNeighbors(nodoHub);
    if (!vecinosHub.empty()) {
        int vecino = vecinosHub[0];
        double pesoOrig = g.getWeight(nodoHub, vecino);
        std::string nombreHub = traductor.id_a_nombre.at(nodoHub);
        std::string nombreVecino = traductor.id_a_nombre.at(vecino);
        std::string aristaStr = nombreHub + "-" + nombreVecino;
        
        std::cout << "[-] LUGAR 1 (HUB): Desconectando " << aristaStr << std::endl;
        g.removeEdge(nodoHub, vecino);
        evaluarImpactoGlobal(g, nombreDataset, "Perturbado_Hub", aristaStr, csv);
        g.addEdge(nodoHub, vecino, pesoOrig);
    }

    // LUGAR 2: PERIFERIA
    int nodoPeriferia = -1;
    std::vector<int> vecinosPeriferia;
    for (auto it = rankingDegree.rbegin(); it != rankingDegree.rend(); ++it) {
        vecinosPeriferia = g.getNeighbors(it->first);
        if (!vecinosPeriferia.empty()) { nodoPeriferia = it->first; break; }
    }
    if (nodoPeriferia != -1 && !vecinosPeriferia.empty()) {
        int vecino = vecinosPeriferia[0];
        double pesoOrig = g.getWeight(nodoPeriferia, vecino);
        std::string nombrePeri = traductor.id_a_nombre.at(nodoPeriferia);
        std::string nombreVecino = traductor.id_a_nombre.at(vecino);
        std::string aristaStr = nombrePeri + "-" + nombreVecino;

        std::cout << "[-] LUGAR 2 (PERIFERIA): Desconectando " << aristaStr << std::endl;
        g.removeEdge(nodoPeriferia, vecino);
        evaluarImpactoGlobal(g, nombreDataset, "Perturbado_Periferia", aristaStr, csv);
        g.addEdge(nodoPeriferia, vecino, pesoOrig);
    }

    // LUGAR 3: MEDIO
    
    int nodoMedio = -1;
    std::vector<int> vecinosMedio;
    
    // Empezamos a buscar desde la mitad del ranking hacia arriba
    int indexMitad = rankingBetweenness.size() / 2;
    for (size_t i = indexMitad; i < rankingBetweenness.size(); ++i) {
        vecinosMedio = g.getNeighbors(rankingBetweenness[i].first);
        if (!vecinosMedio.empty()) {
            nodoMedio = rankingBetweenness[i].first;
            break; // Encontramos al primer "nodo medio" que sí tiene a quién exportar
        }
    }

    if (nodoMedio != -1 && !vecinosMedio.empty()) {
        int vecino = vecinosMedio[0];
        double pesoOrig = g.getWeight(nodoMedio, vecino);
        std::string nombreMed = traductor.id_a_nombre.at(nodoMedio);
        std::string nombreVecino = traductor.id_a_nombre.at(vecino);
        std::string aristaStr = nombreMed + "-" + nombreVecino;

        std::cout << "[-] LUGAR 3 (MEDIO): Desconectando " << aristaStr << std::endl;
        g.removeEdge(nodoMedio, vecino);
        evaluarImpactoGlobal(g, nombreDataset, "Perturbado_Medio", aristaStr, csv);
        g.addEdge(nodoMedio, vecino, pesoOrig);
    } else {
        std::cout << "[-] LUGAR 3 (MEDIO): Ningun nodo desde la mitad hacia abajo tiene vecinos." << std::endl;
    }
}