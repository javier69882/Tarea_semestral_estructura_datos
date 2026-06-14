#include "Centrality.hpp"

#include "Graph.hpp"
#include <functional>
#include <limits>
#include <queue>
#include <cmath>
#include <unordered_map>
#include <vector>
#include <string>

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
