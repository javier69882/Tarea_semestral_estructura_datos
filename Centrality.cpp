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

    // uso de variabbles temporales para el proceso iterativo
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

	bool isUnweightedGraph = true; // se diferencia entre grafo ponderado o no para elegir BFS o Dijkstra respectivamente
	const double unitWeight = 1.0;
	const double epsilon = 1e-9;
    
    // verificacion de grafo ponderado o no
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

    // se usa BFS en este caso
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
    // se usa Dijkstra en este caso
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
