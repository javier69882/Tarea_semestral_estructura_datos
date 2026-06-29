#include "Experimento.hpp"
#include "Centrality.hpp"
#include <iostream>
#include <vector>
#include <iomanip>
#include <fstream>
#include <cmath>     
#include <algorithm>

// Se añade el parámetro 'const MapeoGrafo& traductor'
void Experimento::evaluarImpactoGlobal(const Graph& g, const std::string& nombreDataset, const std::string& estado, const std::string& arista, std::ofstream& csv, const MapeoGrafo& traductor) {
    bool isDirected = (nombreDataset.find("net") != std::string::npos);
    int elemento = 0; // Posición TOP 1

    // Cabecera visual para la consola
    std::cout << "\n>>> RESULTADOS PARA ESTADO: [" << estado << "] | Arista: " << arista << " <<<\n";

    // Función lambda interna para imprimir el Top 30 sin repetir código
    auto printTop30 = [&](const std::vector<std::pair<int, double>>& ranking, const std::string& titulo) {
        std::cout << "  --- Top 30 " << titulo << " ---\n";
        for (size_t i = 0; i < 30 && i < ranking.size(); ++i) {
            std::cout << "    " << i + 1 << ". " << traductor.id_a_nombre.at(ranking[i].first) 
                      << " (Puntaje: " << ranking[i].second << ")\n";
        }
    };

    // DEGREE
    auto resDeg = Centrality::calculateDegreeCentrality(g, isDirected);
    printTop30(resDeg.first, "Degree");
    double deg = 0.0; std::string topDeg = "N/A";
    if (resDeg.first.size() > elemento) {
        deg = resDeg.first[elemento].second;
        topDeg = traductor.id_a_nombre.at(resDeg.first[elemento].first);
    }

    // BETWEENNESS
    auto resBet = Centrality::calculateBetweennessCentrality(g);
    printTop30(resBet.first, "Betweenness");
    double bet = 0.0; std::string topBet = "N/A";
    if (resBet.first.size() > elemento) {
        bet = resBet.first[elemento].second;
        topBet = traductor.id_a_nombre.at(resBet.first[elemento].first);
    }

    // CLOSENESS
    auto resClo = Centrality::calculateClosenessCentrality(g);
    printTop30(resClo.first, "Closeness");
    double clo = 0.0; std::string topClo = "N/A";
    if (resClo.first.size() > elemento) {
        clo = resClo.first[elemento].second;
        topClo = traductor.id_a_nombre.at(resClo.first[elemento].first);
    }

    // PAGERANK
    auto resPr = Centrality::calculatePageRank(g);
    printTop30(resPr.first, "PageRank");
    double pr = 0.0; std::string topPr = "N/A";
    if (resPr.first.size() > elemento) {
        pr = resPr.first[elemento].second;
        topPr = traductor.id_a_nombre.at(resPr.first[elemento].first);
    }

    // EIGENVECTOR
    auto resEig = Centrality::calculateEigenvectorCentrality(g);
    printTop30(resEig.first, "Eigenvector");
    double eig = 0.0; std::string topEig = "N/A";
    if (resEig.first.size() > elemento) {
        eig = resEig.first[elemento].second;
        topEig = traductor.id_a_nombre.at(resEig.first[elemento].first);
    }

    // MÉTRICAS GLOBALES (No tienen un "Top")
    double asp = Centrality::calculateAverageShortestPath(g);
    double dia = Centrality::calculateNetworkDiameter(g);
    
    std::cout << "  --- Metricas Globales ---\n";
    std::cout << "    Average Shortest Path: " << asp << "\n";
    std::cout << "    Diameter: " << dia << "\n";
    std::cout << "--------------------------------------------------\n";

    // Escribimos los valores y los nodos Top 1 correspondientes al CSV
    csv << nombreDataset << "," << estado << "," << arista << ","
        << deg << "," << topDeg << ","
        << bet << "," << topBet << ","
        << clo << "," << topClo << ","
        << pr << "," << topPr << ","
        << asp << "," << dia << "," 
        << eig << "," << topEig << "\n";
}

void Experimento::ejecutarPerturbacion(Graph& g, const std::string& nombreDataset, std::ofstream& csv, const MapeoGrafo& traductor) {
    std::cout << "\n========================================";
    std::cout << "\n INICIANDO PERTURBACION: " << nombreDataset;
    std::cout << "\n========================================\n";

    std::cout << "-> Evaluando GRAFO ORIGINAL..." << std::endl;
    // Añadimos 'traductor' al final
    evaluarImpactoGlobal(g, nombreDataset, "Original", "Ninguna", csv, traductor);
    
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
        evaluarImpactoGlobal(g, nombreDataset, "Perturbado_Hub", aristaStr, csv, traductor);
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
        evaluarImpactoGlobal(g, nombreDataset, "Perturbado_Periferia", aristaStr, csv, traductor);
        g.addEdge(nodoPeriferia, vecino, pesoOrig);
    }

    // LUGAR 3: MEDIO
    int nodoMedio = -1;
    std::vector<int> vecinosMedio;
    
    int indexMitad = rankingBetweenness.size() / 2;
    for (size_t i = indexMitad; i < rankingBetweenness.size(); ++i) {
        vecinosMedio = g.getNeighbors(rankingBetweenness[i].first);
        if (!vecinosMedio.empty()) {
            nodoMedio = rankingBetweenness[i].first;
            break; 
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
        evaluarImpactoGlobal(g, nombreDataset, "Perturbado_Medio", aristaStr, csv, traductor);
        g.addEdge(nodoMedio, vecino, pesoOrig);
    } else {
        std::cout << "[-] LUGAR 3 (MEDIO): Ningun nodo desde la mitad hacia abajo tiene vecinos." << std::endl;
    }
}

void Experimento::ejecutarAumento(Graph& g, const std::string& nombreDataset, std::ofstream& csv, const MapeoGrafo& traductor) {
    std::cout << "\n========================================";
    std::cout << "\n INICIANDO AUMENTO DE ARISTAS: " << nombreDataset;
    std::cout << "\n========================================\n";

    // PONDERADO / PERCENTIL 75
    std::vector<double> todosLosPesos;
    bool esPonderado = false;
    const double epsilon = 1e-6;
    const int vertexCount = g.getNumVertices();

    for (int u = 0; u < vertexCount; ++u) {
        for (int v : g.getNeighbors(u)) {
            double peso = g.getWeight(u, v);
            if (peso > 0.0) {
                todosLosPesos.push_back(peso);
                if (std::fabs(peso - 1.0) > epsilon) {
                    esPonderado = true;
                }
            }
        }
    }

    double pesoAAgregar = 1.0; 
    if (esPonderado && !todosLosPesos.empty()) {
        std::sort(todosLosPesos.begin(), todosLosPesos.end());
        size_t idxP50 = static_cast<size_t>(0.50 * (todosLosPesos.size() - 1));
        pesoAAgregar = todosLosPesos[idxP50];
        
        std::cout << "-> [PONDERADO] Detectado. Total aristas: " << todosLosPesos.size() 
                  << " | Peso P50 calculado: " << pesoAAgregar << std::endl;
    } else {
        std::cout << "-> [NO PONDERADO] Detectado o sin aristas. Usando peso estandar: 1.0" << std::endl;
    }

    std::cout << "-> Evaluando GRAFO ORIGINAL..." << std::endl;
    evaluarImpactoGlobal(g, nombreDataset, "Original", "Ninguna", csv, traductor);
    
    bool isDirected = (nombreDataset.find("net") != std::string::npos);
    auto rankingBetweenness = Centrality::calculateBetweennessCentrality(g).first;
    auto rankingDegree = Centrality::calculateDegreeCentrality(g, isDirected).first;

    if (rankingBetweenness.empty() || rankingDegree.empty()) return;

    auto sonVecinos = [&](int u, int v) {
        std::vector<int> vecinos = g.getNeighbors(u);
        for (int vec : vecinos) {
            if (vec == v) return true;
        }
        return false;
    };

    // LUGAR 1: HUB-HUB
    int hub1 = rankingBetweenness[0].first;
    int hub2 = -1;
    for (size_t i = 1; i < rankingBetweenness.size(); ++i) {
        int candidato = rankingBetweenness[i].first;
        if (candidato != hub1 && !sonVecinos(hub1, candidato)) {
            hub2 = candidato;
            break; 
        }
    }
    if (hub2 == -1 && rankingBetweenness.size() > 1) {
        hub2 = rankingBetweenness[1].first;
    }

    if (hub2 != -1) {
        std::string n1 = traductor.id_a_nombre.at(hub1);
        std::string n2 = traductor.id_a_nombre.at(hub2);
        std::string aristaStr = n1 + "+" + n2;
        
        std::cout << "[+] LUGAR 1 (HUB-HUB): Añadiendo arista entre " << aristaStr << " con peso " << pesoAAgregar << std::endl;
        g.addEdge(hub1, hub2, pesoAAgregar);
        evaluarImpactoGlobal(g, nombreDataset, "Aumento_Hub_Hub", aristaStr, csv, traductor);
        g.removeEdge(hub1, hub2); 
    }

    // LUGAR 2: HUB-PERIFERIA
    int peri1 = -1;
    for (auto it = rankingDegree.rbegin(); it != rankingDegree.rend(); ++it) {
        if (!sonVecinos(hub1, it->first) && hub1 != it->first) {
            peri1 = it->first;
            break;
        }
    }

    if (peri1 == -1 && !rankingDegree.empty()) {
        peri1 = rankingDegree.back().first;
    }

    if (peri1 != -1) {
        std::string n1 = traductor.id_a_nombre.at(hub1);
        std::string n2 = traductor.id_a_nombre.at(peri1);
        std::string aristaStr = n1 + "+" + n2;

        std::cout << "[+] LUGAR 2 (HUB-PERIFERIA): Añadiendo atajo entre " << aristaStr << " con peso " << pesoAAgregar << std::endl;
        g.addEdge(hub1, peri1, pesoAAgregar);
        evaluarImpactoGlobal(g, nombreDataset, "Aumento_Hub_Peri", aristaStr, csv, traductor);
        g.removeEdge(hub1, peri1);
    }

    // LUGAR 3: PERIFERIA-PERIFERIA
    int p1 = -1, p2 = -1;
    for (auto it1 = rankingDegree.rbegin(); it1 != rankingDegree.rend(); ++it1) {
        for (auto it2 = std::next(it1); it2 != rankingDegree.rend(); ++it2) {
            if (!sonVecinos(it1->first, it2->first)) {
                p1 = it1->first;
                p2 = it2->first;
                break;
            }
        }
        if (p1 != -1) break;
    }

    if (p1 != -1 && p2 != -1) {
        std::string n1 = traductor.id_a_nombre.at(p1);
        std::string n2 = traductor.id_a_nombre.at(p2);
        std::string aristaStr = n1 + "+" + n2;

        std::cout << "[+] LUGAR 3 (PERI-PERI): Añadiendo arista entre " << aristaStr << " con peso " << pesoAAgregar << std::endl;
        g.addEdge(p1, p2, pesoAAgregar);
        evaluarImpactoGlobal(g, nombreDataset, "Aumento_Peri_Peri", aristaStr, csv, traductor);
        g.removeEdge(p1, p2);
    }
}