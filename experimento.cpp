#include "Experimento.hpp"
#include "Centrality.hpp"
#include <iostream>
#include <vector>
#include <iomanip>
#include <fstream>
#include <cmath>     
#include <algorithm>

void Experimento::evaluarImpactoGlobal(const Graph& g, const std::string& nombreDataset, const std::string& estado, const std::string& arista, std::ofstream& csv) {
    bool isDirected = (nombreDataset.find("net") != std::string::npos);
    int elemento = 0; 

    auto resDeg = Centrality::calculateDegreeCentrality(g, isDirected);
    double deg = (resDeg.first.size() > elemento) ? resDeg.first[elemento].second : 0.0;
                

    auto resBet = Centrality::calculateBetweennessCentrality(g);
    double bet = (resBet.first.size() > elemento) ? resBet.first[elemento].second : 0.0;

    auto resClo = Centrality::calculateClosenessCentrality(g);
    double clo = (resClo.first.size() > elemento) ? resClo.first[elemento].second : 0.0;

    auto resPr = Centrality::calculatePageRank(g);
    double pr = (resPr.first.size() > elemento) ? resPr.first[elemento].second : 0.0;

    auto resEig = Centrality::calculateEigenvectorCentrality(g);
    double eig = (resEig.first.size() > elemento) ? resEig.first[elemento].second : 0.0;

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


void Experimento::ejecutarAumento(Graph& g, const std::string& nombreDataset, std::ofstream& csv, const MapeoGrafo& traductor) {
    std::cout << "\n========================================";
    std::cout << "\n INICIANDO AUMENTO DE ARISTAS: " << nombreDataset;
    std::cout << "\n========================================\n";

    // --------------------------------------------------------------------
    // DETECCIÓN DE GRAFO PONDERADO Y CÁLCULO DEL PERCENTIL 75
    // --------------------------------------------------------------------
    std::vector<double> todosLosPesos;
    bool esPonderado = false;
    const double epsilon = 1e-6;
    const int vertexCount = g.getNumVertices();

    // Recolectamos todos los pesos válidos de la red
    for (int u = 0; u < vertexCount; ++u) {
        for (int v : g.getNeighbors(u)) {
            double peso = g.getWeight(u, v);
            if (peso > 0.0) {
                todosLosPesos.push_back(peso);
                // Si encontramos un peso que no sea 1.0, el grafo es ponderado
                if (std::fabs(peso - 1.0) > epsilon) {
                    esPonderado = true;
                }
            }
        }
    }

    // Determinamos el peso que usaremos para las nuevas aristas
    double pesoAAgregar = 1.0; 
    if (esPonderado && !todosLosPesos.empty()) {
        // Ordenamos los pesos de menor a mayor para calcular el percentil
        std::sort(todosLosPesos.begin(), todosLosPesos.end());
        
        // Posición del percentil 50 usando el método de rango más cercano
        size_t idxP50 = static_cast<size_t>(0.50 * (todosLosPesos.size() - 1));
        pesoAAgregar = todosLosPesos[idxP50];
        
        std::cout << "-> [PONDERADO] Detectado. Total aristas: " << todosLosPesos.size() 
                  << " | Peso P50 calculado: " << pesoAAgregar << std::endl;
    } else {
        std::cout << "-> [NO PONDERADO] Detectado o sin aristas. Usando peso estandar: 1.0" << std::endl;
    }

    // --------------------------------------------------------------------
    // EVALUACIÓN DEL GRAFO ORIGINAL
    // --------------------------------------------------------------------
    std::cout << "-> Evaluando GRAFO ORIGINAL..." << std::endl;
    evaluarImpactoGlobal(g, nombreDataset, "Original", "Ninguna", csv);
    
    bool isDirected = (nombreDataset.find("net") != std::string::npos);
    auto rankingBetweenness = Centrality::calculateBetweennessCentrality(g).first;
    auto rankingDegree = Centrality::calculateDegreeCentrality(g, isDirected).first;

    if (rankingBetweenness.empty() || rankingDegree.empty()) return;

    // Helper interno para saber si dos nodos ya están conectados
    auto sonVecinos = [&](int u, int v) {
        std::vector<int> vecinos = g.getNeighbors(u);
        for (int vec : vecinos) {
            if (vec == v) return true;
        }
        return false;
    };

    // ====================================================================
    // LUGAR 1: HUB-HUB (Conectar los dos nodos más vitales)
    // ====================================================================
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
        g.addEdge(hub1, hub2, pesoAAgregar); // <--- USAMOS EL PESO DINÁMICO
        evaluarImpactoGlobal(g, nombreDataset, "Aumento_Hub_Hub", aristaStr, csv);
        g.removeEdge(hub1, hub2); 
    }

    // ====================================================================
    // LUGAR 2: HUB-PERIFERIA (Atajo directo al nodo más aislado)
    // ====================================================================
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
        g.addEdge(hub1, peri1, pesoAAgregar); // <--- USAMOS EL PESO DINÁMICO
        evaluarImpactoGlobal(g, nombreDataset, "Aumento_Hub_Peri", aristaStr, csv);
        g.removeEdge(hub1, peri1);
    }

    // ====================================================================
    // LUGAR 3: PERIFERIA-PERIFERIA (Unir a dos aislados)
    // ====================================================================
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
        g.addEdge(p1, p2, pesoAAgregar); // <--- USAMOS EL PESO DINÁMICO
        evaluarImpactoGlobal(g, nombreDataset, "Aumento_Peri_Peri", aristaStr, csv);
        g.removeEdge(p1, p2);
    }
}