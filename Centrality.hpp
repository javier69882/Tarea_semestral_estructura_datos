#ifndef CENTRALITY_HPP
#define CENTRALITY_HPP

#include "Graph.hpp"
#include <unordered_map>
#include <vector>
#include <string>
#include <utility> 

/**
 * @class Centrality
 * @brief Clase estática que provee implementaciones algorítmicas para el cálculo de diversas medidas de centralidad y topología en grafos.
 * Todos los métodos devuelven el resultado del cálculo y el tiempo de ejecución (en milisegundos)
 * para facilitar el análisis experimental del rendimiento computacional.
 */
class Centrality {
public:
    /**
     * @brief Calcula la Centralidad de Grado (Degree Centrality) de todos los vértices.
     * Para grafos no dirigidos, se calcula el grado normalizado por (V-1).
     * Para grafos dirigidos, se suma el in-degree y out-degree, normalizado por 2*(V-1).
     * @param g Referencia constante al grafo a analizar.
     * @param isDirected Booleano que indica si el grafo debe tratarse como dirigido.
     * @return std::pair con el vector de pares (ID_Vertice, Puntaje) ordenado de mayor a menor, y el tiempo de ejecución en ms.
     * @note Complejidad Teórica (Peor Caso): O(V + E)
     * Justificación: Requiere recorrer todos los vértices y sus listas de adyacencia una única vez para contabilizar las conexiones.
     */
    static std::pair<std::vector<std::pair<int, double>>, double> calculateDegreeCentrality(const Graph& g, bool isDirected = false);

    /**
     * @brief Calcula la Centralidad de Intermediación (Betweenness Centrality) usando el Algoritmo de Brandes.
     * Mide la probabilidad de que un vértice esté en el camino más corto entre otros dos vértices.
     * Implementa detección automática de pesos. En grafos ponderados, aplica la transformación inversa de pesos (1/peso) 
     * para convertir medidas de flujo/similitud en métricas de distancia válidas para Dijkstra.
     * @param g Referencia constante al grafo a analizar.
     * @param isDirected Booleano que indica si la normalización debe considerar dirección.
     * @return std::pair con el vector ordenado de (ID_Vertice, Puntaje) y el tiempo en ms.
     * @note Complejidad Teórica (Peor Caso): 
     * - Grafos no ponderados (BFS): O(V * E)
     * - Grafos ponderados (Dijkstra con Priority Queue): O(V * E + V^2 * log(V))
     * Justificación: El algoritmo de Brandes ejecuta un recorrido de caminos mínimos desde cada vértice V, y luego realiza una fase de acumulación hacia atrás (backward propagation).
     */
    static std::pair<std::vector<std::pair<int, double>>, double> calculateBetweennessCentrality(const Graph& g, bool isDirected = false);

    /**
     * @brief Calcula la Centralidad de Cercanía (Closeness Centrality).
     * Implementa la corrección de Wasserman y Faust para penalizar nodos en grafos con componentes desconectados.
     * @param g Referencia constante al grafo a analizar.
     * @return std::pair con el vector ordenado de (ID_Vertice, Puntaje) y el tiempo en ms.
     * @note Complejidad Teórica (Peor Caso): O(V * (V + E)) para grafos no ponderados y O(V * (E + V log V)) para ponderados.
     * Justificación: Requiere calcular el problema de caminos mínimos desde todos los orígenes (All-Pairs Shortest Path).
     */
    static std::pair<std::vector<std::pair<int, double>>, double> calculateClosenessCentrality(const Graph& g);

    /**
     * @brief Calcula el algoritmo de PageRank iterativo.
     * Asigna importancia a un nodo basándose en la importancia de los nodos que apuntan a él.
     * @param g Referencia constante al grafo.
     * @param dampingFactor Probabilidad de continuar navegando por la red (típicamente 0.85).
     * @param maxIterations Límite máximo de iteraciones para forzar la parada si no converge.
     * @param tolerance Criterio de convergencia (diferencia mínima entre iteraciones).
     * @return std::pair con el vector ordenado de (ID_Vertice, Puntaje) y el tiempo en ms.
     * @note Complejidad Teórica (Peor Caso): O(I * (V + E))
     * Justificación: En cada iteración 'I', se recorren todos los vértices V y sus aristas entrantes E para actualizar el vector de rangos.
     */
    static std::pair<std::vector<std::pair<int, double>>, double> calculatePageRank(const Graph& g, double dampingFactor = 0.85, int maxIterations = 100, double tolerance = 1e-6);

    /**
     * @brief Calcula el Camino Más Corto Promedio (Average Shortest Path) de toda la red.
     * Aplica la transformación inversa de pesos (1/peso) en caso de grafos ponderados.
     * @param g Referencia constante al grafo.
     * @return double con el promedio de todas las distancias mínimas posibles.
     * @note Complejidad Teórica (Peor Caso): O(V * (V + E))
     * Justificación: Al igual que Closeness, ejecuta un recorrido BFS o Dijkstra desde cada uno de los vértices para obtener la sumatoria total de distancias.
     */
    static double calculateAverageShortestPath(const Graph& g);

    /**
     * @brief Calcula el Diámetro de la Red (Métrica Adicional 1).
     * Encuentra la máxima distancia posible entre cualquier par de vértices en los caminos más cortos de la red.
     * @param g Referencia constante al grafo.
     * @return double con el diámetro (distancia máxima).
     * @note Complejidad Teórica (Peor Caso): O(V * (V + E))
     * Justificación: Requiere un All-Pairs Shortest Path completo para garantizar encontrar el valor máximo absoluto de distancia mínima.
     */
    static double calculateNetworkDiameter(const Graph& g);

    /**
     * @brief Calcula la Centralidad de Vector Propio (Eigenvector Centrality) (Métrica Adicional 2).
     * Utiliza el método de Iteración de la Potencia (Power Iteration) con normalización euclidiana.
     * A diferencia de los algoritmos de caminos, este método utiliza los pesos de forma directa (proporcionalidad).
     * @param g Referencia constante al grafo.
     * @param maxIterations Límite de iteraciones.
     * @param tolerance Tolerancia de convergencia.
     * @return std::pair con el vector ordenado de (ID_Vertice, Puntaje) y el tiempo en ms.
     * @note Complejidad Teórica (Peor Caso): O(I * (V + E))
     * Justificación: Multiplicación iterativa matriz-vector esparcida. Converge rápidamente si la brecha espectral es grande.
     */
    static std::pair<std::vector<std::pair<int, double>>, double> calculateEigenvectorCentrality(const Graph& g, int maxIterations = 100, double tolerance = 1e-6);
};

#endif // CENTRALITY_HPP