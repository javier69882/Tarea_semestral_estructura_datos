#ifndef CENTRALITY_HPP
#define CENTRALITY_HPP

#include "Graph.hpp"
#include <unordered_map>
#include <vector>
#include <string>

class Centrality {
public:
    // 1. Degree Centrality
    // Fuente: Adaptado de la lógica estándar descrita en Newman, M. E. J. (2018), "Networks".
    static std::unordered_map<int, double> calculateDegreeCentrality(const Graph& g, bool isDirected = false);

    // 2. Betweenness Centrality
    // Fuente: Implementación basada en Brandes, U. (2001). "A faster algorithm for betweenness centrality".
    static std::unordered_map<int, double> calculateBetweennessCentrality(const Graph& g);

    // 3. Closeness Centrality
    // Fuente: Basado en el algoritmo de caminos más cortos de Dijkstra/BFS referenciado en NetworkX Developers (2026).
     static std::pair<std::vector<std::pair<int, double>>, double> calculateClosenessCentrality(const Graph& g);

    // 4. PageRank
    // Fuente: Algoritmo iterativo clásico (Page et al., 1999). Lógica de convergencia adaptada de NetworkX.
    static std::pair<std::vector<std::pair<int, double>>, double> calculatePageRank(const Graph& g, double dampingFactor = 0.85, int maxIterations = 100, double tolerance = 1e-6);

    // 5. Average Shortest Path
    // Fuente: Newman, M. E. J. (2018). "Networks". Requiere All-Pairs Shortest Path.
    static double calculateAverageShortestPath(const Graph& g);

    // 6. Network Diameter (Métrica Adicional 1)
    // Fuente: NetworkX Reference: Algorithms. Obtiene el máximo de las distancias mínimas.
    static int calculateNetworkDiameter(const Graph& g);

    // 7. Eigenvector Centrality (Métrica Adicional 2)
    // Fuente: Método de iteración de potencia (Power Iteration) detallado en Newman (2018).
    static std::unordered_map<int, double> calculateEigenvectorCentrality(const Graph& g, int maxIterations = 100, double tolerance = 1e-6);
};

#endif // CENTRALITY_HPP