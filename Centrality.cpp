#include "Centrality.hpp"

#include "Graph.hpp"
#include <functional>
#include <limits>
#include <queue>
#include <cmath>
#include <unordered_map>
#include <vector>
#include <string>
#include <cmath> 
#include <chrono> 
#include <algorithm> 


// 1. Degree Centrality
std::pair<std::vector<std::pair<int, double>>, double> Centrality::calculateDegreeCentrality(const Graph& g, bool isDirected) {
    auto start = std::chrono::high_resolution_clock::now();

    int vertexCount = g.getNumVertices();
    std::vector<std::pair<int, double>> rankingFinal;

    // Caso borde: verificamos si el grafo está vacío o tiene 1 nodo
    if (vertexCount <= 1) {
        return std::make_pair(rankingFinal, 0.0);
    }

    std::unordered_map<int, double> degreeScores;
    // V - 1 para la normalización
    double maxConexiones = static_cast<double>(vertexCount - 1);

    // Recorremos usando la interfaz pública del ADT Grafo
    for (int i = 0; i < vertexCount; ++i) {
        // g.getNeighbors(i) devuelve el vector de vecinos. Su tamaño es el grado.
        double grado = static_cast<double>(g.getNeighbors(i).size()); 
        
        degreeScores[i] = grado / maxConexiones; 
    }

// --- BLOQUE DE ORDENAMIENTO ESTANDARIZADO ---
    for (const auto& par : degreeScores) {
        rankingFinal.emplace_back(par.first, par.second); 
    }

    std::sort(rankingFinal.begin(), rankingFinal.end(), [](const std::pair<int, double>& a, const std::pair<int, double>& b) {
        return a.second > b.second; // Orden descendente
    });

    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double, std::milli> duracion = end - start;

    return std::make_pair(rankingFinal, duracion.count());
}

// 2. Betweenness Centrality
// El siguiente bloque implementa el Algoritmo de Brandes para la centralidad de intermediación.
// La estructura de estructuras de datos (uso de P, sigma, d, delta y S) fue adaptada a partir 
// de la lógica fundacional descrita en "A Faster Algorithm for Betweenness Centrality" (Brandes, 2001)
// y guiada por las convenciones de la Boost Graph Library (BGL) para C++.
std::pair<std::vector<std::pair<int, double>>, double> Centrality::calculateBetweennessCentrality(const Graph& g) {
    auto start = std::chrono::high_resolution_clock::now();

    int n = g.getNumVertices();
    std::vector<std::pair<int, double>> rankingFinal;
    if (n <= 1) return std::make_pair(rankingFinal, 0.0);

    // Mapa para acumular los puntajes de intermediación
    std::unordered_map<int, double> cb;
    for (int i = 0; i < n; ++i) cb[i] = 0.0;

    // El algoritmo de Brandes itera tomando cada nodo como "fuente" (s)
    for (int s = 0; s < n; ++s) {
        std::stack<int> S; // Pila para guardar el orden de visita y hacer el cálculo hacia atrás
        std::vector<std::vector<int>> P(n); // Lista de predecesores en el camino más corto
        std::vector<int> sigma(n, 0); // Número de caminos más cortos desde 's' hasta 'v'
        sigma[s] = 1;
        std::vector<int> d(n, -1); // Distancias
        d[s] = 0;
        std::queue<int> Q;
        Q.push(s);

        // Fase 1: BFS para encontrar los caminos más cortos y contar las multiplicidades (sigma)
        while (!Q.empty()) {
            int v = Q.front();
            Q.pop();
            S.push(v); // Se apila para procesar en orden inverso luego

            for (int w : g.getNeighbors(v)) {
                // Si 'w' se descubre por primera vez
                if (d[w] < 0) {
                    Q.push(w);
                    d[w] = d[v] + 1;
                }
                // Si el camino a través de 'v' es un camino más corto hacia 'w'
                if (d[w] == d[v] + 1) {
                    sigma[w] += sigma[v];
                    P[w].push_back(v);
                }
            }
        }

        // Fase 2: Acumulación de dependencias hacia atrás
        std::vector<double> delta(n, 0.0);
        while (!S.empty()) {
            int w = S.top();
            S.pop();
            for (int v : P[w]) {
                // Cálculo de la fracción de caminos que pasan por v
                delta[v] += (static_cast<double>(sigma[v]) / sigma[w]) * (1.0 + delta[w]);
            }
            if (w != s) {
                cb[w] += delta[w];
            }
        }
    }

    // Fase 3: Normalización y Traslado a Vector (Grafo no 
    double divisorNormalizacion = static_cast<double>((n - 1) * (n - 2)); 
    
    for (const auto& par : cb) {
        // En grafos no dirigidos, cada camino se cuenta dos veces, por eso el / 2.0 extra.
        double valorCentralidad = (par.second / 2.0); 
        
        // Si desean normalizar la métrica entre 0 y 1 para comparar grafos de distinto tamaño:
        if (divisorNormalizacion > 0) {
            valorCentralidad = valorCentralidad / divisorNormalizacion;
        }

        rankingFinal.emplace_back(par.first, valorCentralidad);
    }

    // Ordenamiento estandarizado
    std::sort(rankingFinal.begin(), rankingFinal.end(), [](const std::pair<int, double>& a, const std::pair<int, double>& b) {
        return a.second > b.second;
    });

    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double, std::milli> duracion = end - start;

    return std::make_pair(rankingFinal, duracion.count());
}

// 3. Closeness Centrality
std::pair<std::vector<std::pair<int, double>>, double> Centrality::calculateClosenessCentrality(const Graph& g) {
    auto start = std::chrono::high_resolution_clock::now();

    const int vertexCount = g.getNumVertices();
    std::vector<std::pair<int, double>> rankingFinal;

    // Si el grafo esta vacio o tiene 1 nodo, devolvemos vacio
    if (vertexCount <= 1) {
        return std::make_pair(rankingFinal, 0.0);
    }

    std::unordered_map<int, double> closenessScores;

    const GraphList* listGraph = dynamic_cast<const GraphList*>(&g);
    const bool hasAdjacencyListFastPath = (listGraph != nullptr);

    bool isUnweightedGraph = true; 
    const double unitWeight = 1.0;
    const double epsilon = 1e-9;
    
    // Verificacion de grafo ponderado o no igual que en avg first path, para elegir BFS o Dijkstra respectivamente
    if (hasAdjacencyListFastPath) {
        for (int u = 0; u < vertexCount && isUnweightedGraph; ++u) {
            for (const auto& edge : listGraph->getAdjacencyListRef(u)) {
                if (edge.second <= 0.0 || std::fabs(edge.second - unitWeight) > epsilon) {
                    isUnweightedGraph = false;
                    break;
                }
            }
        }
    } else {
        for (int u = 0; u < vertexCount && isUnweightedGraph; ++u) {
            for (int v : g.getNeighbors(u)) {
                const double edgeWeight = g.getWeight(u, v);
                if (edgeWeight <= 0.0 || std::fabs(edgeWeight - unitWeight) > epsilon) {
                    isUnweightedGraph = false;
                    break;
                }
            }
        }
    }

    const double infinity = std::numeric_limits<double>::infinity();

   
    // CASO 1: BFS para Grafos No Ponderados
   
    if (isUnweightedGraph) {
        if (hasAdjacencyListFastPath) {
            std::vector<int> distances(vertexCount, 0);
            std::vector<int> visited(vertexCount, 0);
            int traversalTag = 1;

            for (int source = 0; source < vertexCount; ++source) {
                if (traversalTag == std::numeric_limits<int>::max()) {
                    std::fill(visited.begin(), visited.end(), 0);
                    traversalTag = 1;
                }

                visited[source] = traversalTag;
                distances[source] = 0;
                std::queue<int> frontier;
                frontier.push(source);

                double sumDistances = 0.0; // Acumulador local para el nodo source

                while (!frontier.empty()) {
                    const int currentVertex = frontier.front();
                    frontier.pop();

                    for (const auto& edge : listGraph->getAdjacencyListRef(currentVertex)) {
                        const int neighbor = edge.first;
                        if (visited[neighbor] == traversalTag) {
                            continue;
                        }

                        visited[neighbor] = traversalTag;
                        distances[neighbor] = distances[currentVertex] + 1;
                        frontier.push(neighbor);

                        sumDistances += static_cast<double>(distances[neighbor]);
                    }
                }
                ++traversalTag;

                // formula del informe
                if (sumDistances > 0.0) {
                    closenessScores[source] = (vertexCount - 1) / sumDistances;
                } else {
                    closenessScores[source] = 0.0; // Nodo aislado
                }
            }
        } else {
            // Version BFS para Grafos Generales / Matrices
            for (int source = 0; source < vertexCount; ++source) {
                std::vector<double> distances(vertexCount, infinity);
                distances[source] = 0.0;
                std::queue<int> frontier;
                frontier.push(source);

                double sumDistances = 0.0;

                while (!frontier.empty()) {
                    const int currentVertex = frontier.front();
                    frontier.pop();

                    for (int neighbor : g.getNeighbors(currentVertex)) {
                        if (distances[neighbor] != infinity) continue;

                        distances[neighbor] = distances[currentVertex] + 1.0;
                        frontier.push(neighbor);
                        sumDistances += distances[neighbor];
                    }
                }

                if (sumDistances > 0.0) closenessScores[source] = (vertexCount - 1) / sumDistances;
                else closenessScores[source] = 0.0;
            }
        }
    } 
  
    // CASO 2: Dijkstra para Grafos Ponderados
  
    else {
        for (int source = 0; source < vertexCount; ++source) {
            std::vector<double> distances(vertexCount, infinity);
            distances[source] = 0.0;

            using QueueEntry = std::pair<double, int>;
            std::priority_queue<QueueEntry, std::vector<QueueEntry>, std::greater<QueueEntry>> frontier;
            frontier.push({0.0, source});

            while (!frontier.empty()) {
                const auto [currentDistance, currentVertex] = frontier.top();
                frontier.pop();

                if (currentDistance > distances[currentVertex]) continue;

                if (hasAdjacencyListFastPath) {
                    for (const auto& edge : listGraph->getAdjacencyListRef(currentVertex)) {
                        const int neighbor = edge.first;
                        const double edgeWeight = edge.second;
                        if (edgeWeight <= 0.0) continue;

                        const double candidateDistance = currentDistance + edgeWeight;
                        if (candidateDistance < distances[neighbor]) {
                            distances[neighbor] = candidateDistance;
                            frontier.push({candidateDistance, neighbor});
                        }
                    }
                } else {
                    for (int neighbor : g.getNeighbors(currentVertex)) {
                        const double edgeWeight = g.getWeight(currentVertex, neighbor);
                        if (edgeWeight <= 0.0) continue;

                        const double candidateDistance = currentDistance + edgeWeight;
                        if (candidateDistance < distances[neighbor]) {
                            distances[neighbor] = candidateDistance;
                            frontier.push({candidateDistance, neighbor});
                        }
                    }
                }
            }

            // Una vez terminadas las rutas de Dijkstra, sumamos las distancias validas
            double sumDistances = 0.0;
            for (int target = 0; target < vertexCount; ++target) {
                if (target != source && distances[target] != infinity) {
                    sumDistances += distances[target];
                }
            }

            if (sumDistances > 0.0) closenessScores[source] = (vertexCount - 1) / sumDistances;
            else closenessScores[source] = 0.0;
        }
    }

    
    //PASAR A VECTOR Y ORDENAR
    
    for (const auto& par : closenessScores) {
        rankingFinal.push_back(par);
    }

    std::sort(rankingFinal.begin(), rankingFinal.end(), [](const std::pair<int, double>& a, const std::pair<int, double>& b) {
        return a.second > b.second; 
    });

    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double, std::milli> duracion = end - start;

    return std::make_pair(rankingFinal, duracion.count());
}


// 4. PageRank
// Fuente: Algoritmo iterativo clásico (Page et al., 1999). Lógica de convergencia adaptada de NetworkX.
std::pair<std::vector<std::pair<int, double>>, double> Centrality::calculatePageRank(const Graph& g, double dampingFactor, int maxIterations, double tolerance) {
    
    auto start = std::chrono::high_resolution_clock::now();

    int N = g.getNumVertices();
    std::vector<std::pair<int, double>> rankingVacio;
    if (N == 0) return std::make_pair(rankingVacio, 0.0);

    // Uso de variabbles temporales para el proceso iterativo
    std::unordered_map<int, double> PR_old;
    std::unordered_map<int, double> PR_new;

    std::vector<std::vector<int>> inEdges(N);
    std::vector<int> outDegree(N, 0);

    // Inicialización
    for (int i = 0; i < N; ++i) {
        PR_old[i] = 1.0 / N; 
        PR_new[i] = 0.0;
        std::vector<int> neighbors = g.getNeighbors(i);
        outDegree[i] = neighbors.size();
        for (int v : neighbors) {
            inEdges[v].push_back(i); 
        }
    }

    // Proceso Iterativo
    for (int iter = 0; iter < maxIterations; ++iter) {
        double diff = 0.0; 

        for (int u = 0; u < N; ++u) {
            double sumatoria = 0.0;
            for (int v : inEdges[u]) {
                sumatoria += PR_old[v] / outDegree[v];
            }
            PR_new[u] = ((1.0 - dampingFactor) / N) + (dampingFactor * sumatoria);
            diff += std::abs(PR_new[u] - PR_old[u]);
        }

        PR_old = PR_new;
        if (diff < tolerance) break; 
    }

  
    //PASAR A VECTOR Y ORDENAR 
   
    //Copiamos los resultados finales del mapa al vector de salida
    std::vector<std::pair<int, double>> rankingFinal(PR_old.begin(), PR_old.end());

    //Ordenamos el vector de mayor a menor puntaje usando sort de Stl, indicando que compare el segundo elemento del par (el puntaje) para ordenar
    std::sort(rankingFinal.begin(), rankingFinal.end(), [](const std::pair<int, double>& a, const std::pair<int, double>& b) {
        return a.second > b.second; 
    });

    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double, std::milli> duracion = end - start;

    // Retornamos el vector ya ordenado y el tiempo
    return std::make_pair(rankingFinal, duracion.count());
}


// 5. Average Shortest Path
double Centrality::calculateAverageShortestPath(const Graph& g) {
	const int vertexCount = g.getNumVertices();
	if (vertexCount <= 1) {
		return 0.0;
	}

	const GraphList* listGraph = dynamic_cast<const GraphList*>(&g);
	const bool hasAdjacencyListFastPath = (listGraph != nullptr);

	bool isUnweightedGraph = true; // Se diferencia entre grafo ponderado o no para elegir BFS o Dijkstra respectivamente
	const double unitWeight = 1.0;
	const double epsilon = 1e-9;
    
    // Verificacion de grafo ponderado o no
	if (hasAdjacencyListFastPath) {
		for (int u = 0; u < vertexCount && isUnweightedGraph; ++u) {
			for (const auto& edge : listGraph->getAdjacencyListRef(u)) {
				if (edge.second <= 0.0 || std::fabs(edge.second - unitWeight) > epsilon) {
					isUnweightedGraph = false;
					break;
				}
			}
		}
	} 
    else {
		for (int u = 0; u < vertexCount && isUnweightedGraph; ++u) {
			for (int v : g.getNeighbors(u)) {
				const double edgeWeight = g.getWeight(u, v);
				if (edgeWeight <= 0.0 || std::fabs(edgeWeight - unitWeight) > epsilon) {
					isUnweightedGraph = false;
					break;
				}
			}
		}
	}

	const double infinity = std::numeric_limits<double>::infinity();
	double totalDistance = 0.0;
	std::size_t reachablePairs = 0;

    // Se usa BFS en este caso
	if (isUnweightedGraph) {
		if (hasAdjacencyListFastPath) {
			std::vector<int> distances(vertexCount, 0);
			std::vector<int> visited(vertexCount, 0);
			int traversalTag = 1;

			for (int source = 0; source < vertexCount; ++source) {
				if (traversalTag == std::numeric_limits<int>::max()) {
					std::fill(visited.begin(), visited.end(), 0);
					traversalTag = 1;
				}

				visited[source] = traversalTag;
				distances[source] = 0;
				std::queue<int> frontier;
				frontier.push(source);

				while (!frontier.empty()) {
					const int currentVertex = frontier.front();
					frontier.pop();

					for (const auto& edge : listGraph->getAdjacencyListRef(currentVertex)) {
						const int neighbor = edge.first;
						if (visited[neighbor] == traversalTag) {
							continue;
						}

						visited[neighbor] = traversalTag;
						distances[neighbor] = distances[currentVertex] + 1;
						frontier.push(neighbor);

						totalDistance += static_cast<double>(distances[neighbor]);
						++reachablePairs;
					}
				}

				++traversalTag;
			}
		} else {
			for (int source = 0; source < vertexCount; ++source) {
				std::vector<double> distances(vertexCount, infinity);
				distances[source] = 0.0;

				std::queue<int> frontier;
				frontier.push(source);

				while (!frontier.empty()) {
					const int currentVertex = frontier.front();
					frontier.pop();

					for (int neighbor : g.getNeighbors(currentVertex)) {
						if (distances[neighbor] != infinity) {
							continue;
						}

						distances[neighbor] = distances[currentVertex] + 1.0;
						frontier.push(neighbor);

						totalDistance += distances[neighbor];
						++reachablePairs;
					}
				}
			}
		}
	} 
    // Se usa Dijkstra en este caso
    else {
		for (int source = 0; source < vertexCount; ++source) {
			std::vector<double> distances(vertexCount, infinity);
			distances[source] = 0.0;

			using QueueEntry = std::pair<double, int>;
			std::priority_queue<QueueEntry, std::vector<QueueEntry>, std::greater<QueueEntry>> frontier;
			frontier.push({0.0, source});

			while (!frontier.empty()) {
				const auto [currentDistance, currentVertex] = frontier.top();
				frontier.pop();

				if (currentDistance > distances[currentVertex]) {
					continue;
				}

				if (hasAdjacencyListFastPath) {
					for (const auto& edge : listGraph->getAdjacencyListRef(currentVertex)) {
						const int neighbor = edge.first;
						const double edgeWeight = edge.second;
						if (edgeWeight <= 0.0) {
							continue;
						}

						const double candidateDistance = currentDistance + edgeWeight;
						if (candidateDistance < distances[neighbor]) {
							distances[neighbor] = candidateDistance;
							frontier.push({candidateDistance, neighbor});
						}
					}
				} else {
					for (int neighbor : g.getNeighbors(currentVertex)) {
						const double edgeWeight = g.getWeight(currentVertex, neighbor);
						if (edgeWeight <= 0.0) {
							continue;
						}

						const double candidateDistance = currentDistance + edgeWeight;
						if (candidateDistance < distances[neighbor]) {
							distances[neighbor] = candidateDistance;
							frontier.push({candidateDistance, neighbor});
						}
					}
				}
			}

			for (int target = 0; target < vertexCount; ++target) {
				if (target == source || distances[target] == infinity) {
					continue;
				}

				totalDistance += distances[target];
				++reachablePairs;
			}
		}
	}

	if (reachablePairs == 0) {
		return 0.0;
	}

	return totalDistance / static_cast<double>(reachablePairs);
}

// 6. Network Diameter
// Fuente algorítmica: NetworkX Reference. Obtiene el máximo de las distancias mínimas.
double Centrality::calculateNetworkDiameter(const Graph& g) {
    const int vertexCount = g.getNumVertices();
    if (vertexCount <= 1) {
        return 0.0;
    }

    const GraphList* listGraph = dynamic_cast<const GraphList*>(&g);
    const bool hasAdjacencyListFastPath = (listGraph != nullptr);

    bool isUnweightedGraph = true; 
    const double unitWeight = 1.0;
    const double epsilon = 1e-9;
    
    // Verificación de grafo ponderado o no (Igual que en ASP)
    if (hasAdjacencyListFastPath) {
        for (int u = 0; u < vertexCount && isUnweightedGraph; ++u) {
            for (const auto& edge : listGraph->getAdjacencyListRef(u)) {
                if (edge.second <= 0.0 || std::fabs(edge.second - unitWeight) > epsilon) {
                    isUnweightedGraph = false;
                    break;
                }
            }
        }
    } else {
        for (int u = 0; u < vertexCount && isUnweightedGraph; ++u) {
            for (int v : g.getNeighbors(u)) {
                const double edgeWeight = g.getWeight(u, v);
                if (edgeWeight <= 0.0 || std::fabs(edgeWeight - unitWeight) > epsilon) {
                    isUnweightedGraph = false;
                    break;
                }
            }
        }
    }

    const double infinity = std::numeric_limits<double>::infinity();
    double globalDiameter = 0.0; // Aquí guardaremos la distancia máxima encontrada

    // CASO 1: BFS para Grafos No Ponderados
    if (isUnweightedGraph) {
        if (hasAdjacencyListFastPath) {
            std::vector<int> distances(vertexCount, 0);
            std::vector<int> visited(vertexCount, 0);
            int traversalTag = 1;

            for (int source = 0; source < vertexCount; ++source) {
                if (traversalTag == std::numeric_limits<int>::max()) {
                    std::fill(visited.begin(), visited.end(), 0);
                    traversalTag = 1;
                }

                visited[source] = traversalTag;
                distances[source] = 0;
                std::queue<int> frontier;
                frontier.push(source);

                while (!frontier.empty()) {
                    const int currentVertex = frontier.front();
                    frontier.pop();

                    for (const auto& edge : listGraph->getAdjacencyListRef(currentVertex)) {
                        const int neighbor = edge.first;
                        if (visited[neighbor] == traversalTag) {
                            continue;
                        }

                        visited[neighbor] = traversalTag;
                        distances[neighbor] = distances[currentVertex] + 1;
                        frontier.push(neighbor);

                        // Actualizamos el diámetro global si encontramos un camino más largo
                        if (distances[neighbor] > globalDiameter) {
                            globalDiameter = distances[neighbor];
                        }
                    }
                }
                ++traversalTag;
            }
        } else {
            for (int source = 0; source < vertexCount; ++source) {
                std::vector<double> distances(vertexCount, infinity);
                distances[source] = 0.0;
                std::queue<int> frontier;
                frontier.push(source);

                while (!frontier.empty()) {
                    const int currentVertex = frontier.front();
                    frontier.pop();

                    for (int neighbor : g.getNeighbors(currentVertex)) {
                        if (distances[neighbor] != infinity) {
                            continue;
                        }

                        distances[neighbor] = distances[currentVertex] + 1.0;
                        frontier.push(neighbor);

                        if (distances[neighbor] > globalDiameter) {
                            globalDiameter = distances[neighbor];
                        }
                    }
                }
            }
        }
    } 
    // CASO 2: Dijkstra para Grafos Ponderados
    else {
        for (int source = 0; source < vertexCount; ++source) {
            std::vector<double> distances(vertexCount, infinity);
            distances[source] = 0.0;

            using QueueEntry = std::pair<double, int>;
            std::priority_queue<QueueEntry, std::vector<QueueEntry>, std::greater<QueueEntry>> frontier;
            frontier.push({0.0, source});

            while (!frontier.empty()) {
                const auto [currentDistance, currentVertex] = frontier.top();
                frontier.pop();

                if (currentDistance > distances[currentVertex]) {
                    continue;
                }

                if (hasAdjacencyListFastPath) {
                    for (const auto& edge : listGraph->getAdjacencyListRef(currentVertex)) {
                        const int neighbor = edge.first;
                        const double edgeWeight = edge.second;
                        if (edgeWeight <= 0.0) continue;

                        const double candidateDistance = currentDistance + edgeWeight;
                        if (candidateDistance < distances[neighbor]) {
                            distances[neighbor] = candidateDistance;
                            frontier.push({candidateDistance, neighbor});
                        }
                    }
                } else {
                    for (int neighbor : g.getNeighbors(currentVertex)) {
                        const double edgeWeight = g.getWeight(currentVertex, neighbor);
                        if (edgeWeight <= 0.0) continue;

                        const double candidateDistance = currentDistance + edgeWeight;
                        if (candidateDistance < distances[neighbor]) {
                            distances[neighbor] = candidateDistance;
                            frontier.push({candidateDistance, neighbor});
                        }
                    }
                }
            }

            // Una vez que Dijkstra termina para este nodo fuente, buscamos la distancia máxima válida
            for (int target = 0; target < vertexCount; ++target) {
                if (distances[target] != infinity && distances[target] > globalDiameter) {
                    globalDiameter = distances[target];
                }
            }
        }
    }

    return globalDiameter;
}

// 7. Eigenvector Centrality
// Fuente algorítmica: Método de iteración de potencia (Power Iteration) detallado en Newman (2018).
std::pair<std::vector<std::pair<int, double>>, double> Centrality::calculateEigenvectorCentrality(const Graph& g, int maxIterations, double tolerance) {
    auto start = std::chrono::high_resolution_clock::now();

    int N = g.getNumVertices();
    std::vector<std::pair<int, double>> rankingFinal;
    if (N == 0) return std::make_pair(rankingFinal, 0.0);

    // Inicializamos todos los nodos con un valor de 1.0 / N
    std::unordered_map<int, double> x_old;
    std::unordered_map<int, double> x_new;
    for (int i = 0; i < N; ++i) {
        x_old[i] = 1.0 / N;
        x_new[i] = 0.0;
    }

    // Proceso iterativo (Power Iteration)
    for (int iter = 0; iter < maxIterations; ++iter) {
        double sum_squares = 0.0;

        // 1. Multiplicación del vector por la matriz/lista de adyacencia
        for (int u = 0; u < N; ++u) {
            double sumatoria_vecinos = 0.0;
            // Solo sumamos la influencia de los vecinos directos
            for (int v : g.getNeighbors(u)) {
                sumatoria_vecinos += x_old[v]; 
            }
            x_new[u] = sumatoria_vecinos;
            // Guardamos la suma de los cuadrados para la normalización posterior
            sum_squares += x_new[u] * x_new[u]; 
        }

        // 2. Normalización (Euclidiana) para evitar que los valores tiendan a infinito
        double norm = std::sqrt(sum_squares);
        if (norm == 0) { // Caso borde de grafo totalmente desconectado
            norm = 1.0;
        }

        double diff = 0.0;
        for (int u = 0; u < N; ++u) {
            x_new[u] = x_new[u] / norm;
            // Calculamos la convergencia
            diff += std::abs(x_new[u] - x_old[u]);
        }

        x_old = x_new;

        // 3. Criterio de parada
        if (diff < tolerance) {
            break;
        }
    }

    // Traslado al vector final y ordenamiento estandarizado
    for (const auto& par : x_old) {
        rankingFinal.emplace_back(par.first, par.second);
    }

    std::sort(rankingFinal.begin(), rankingFinal.end(), [](const std::pair<int, double>& a, const std::pair<int, double>& b) {
        return a.second > b.second;
    });

    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double, std::milli> duracion = end - start;

    return std::make_pair(rankingFinal, duracion.count());
}

