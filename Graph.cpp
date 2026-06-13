#include "Graph.hpp"

// Implementación de la clase base Graph
Graph::Graph(bool directed) : isDirected(directed){

}


// Implementacion de lista de adyacencia

GraphList::GraphList(bool directed) : Graph(directed) {}

void GraphList::addVertex() { 
    adjList.push_back(std::vector<std::pair<int, double>>());
}

void GraphList::addEdge(int u, int v, double weight){
    if (u < 0 || u >= adjList.size() || v < 0 || v >= adjList.size()) {
        throw std::out_of_range("Vértice no creado");
    }

    // busco duplicados antes de insertar
    //necesario, porque en el dataset de proteinas, que es no dirigido, hay aristas que podrian quedar repetidas, error que con matriz de adyacencia no ocurre
    for (const auto& arista : adjList[u]) {
        if (arista.first == v) {
            return; // Si ya son vecinos, salimos
        }
    }

    //si el bucle termina y no se activo el return, significa que la arista es nueva, inserta par, vertice peso
    adjList[u].push_back({v, weight});
    
   if (!isDirected && u != v) {
        adjList[v].push_back({u, weight});
    }
}

void GraphList::removeEdge(int u, int v) {
    if (u < 0 || u >= adjList.size() || v < 0 || v >= adjList.size()) return;

    // Borrado manual buscando el destino 
    for (auto it = adjList[u].begin(); it != adjList[u].end(); ++it) {
        if (it->first == v) {
            adjList[u].erase(it);
            break;
        }
    }

    if (!isDirected && u != v) {
        for (auto it = adjList[v].begin(); it != adjList[v].end(); ++it) {
            if (it->first == u) {
                adjList[v].erase(it);
                break;
            }
        }
    }
}

int GraphList::getNumVertices() const {
    return adjList.size();
}

std::vector<int> GraphList::getNeighbors(int vertex) const {
    if (vertex < 0 || vertex >= adjList.size()) throw std::out_of_range("Vértice no creado");
    
    
    std::vector<int> vecinos;
    for (const auto& arista : adjList[vertex]) {
        vecinos.push_back(arista.first);
    }
    return vecinos;
}

double GraphList::getWeight(int u, int v) const {
    if (u < 0 || u >= adjList.size() || v < 0 || v >= adjList.size()) return 0.0;
    
    for (const auto& arista : adjList[u]) {
        if (arista.first == v) {
            return arista.second; // Retornamos el peso
        }
    }
    return 0.0; 
}

void GraphList::print() const {
    std::cout << "Lista de adyacencia (con pesos):\n";
    for (size_t i = 0; i < adjList.size(); i++) {
        std::cout << i << ":";
        for (const auto& arista : adjList[i]) {
            std::cout << " -> " << arista.first << " [W:" << arista.second << "]";
        }
        std::cout << "\n";
    }
}

// fin lista de ayacencia



// Implementación de GraphMatrix

GraphMatrix::GraphMatrix(bool directed) : Graph(directed), n(0) {}

GraphMatrix::GraphMatrix(int V, bool directed) : Graph(directed), n(V) { 
    //inicializamos la matriz con 0.0 (double)
    adjMatrix.resize(n, std::vector<double>(n, 0.0)); 
}

void GraphMatrix::addVertex() {
    n++;
    for (auto &row : adjMatrix) {
        row.push_back(0.0);
    }
    adjMatrix.push_back(std::vector<double>(n, 0.0));
}

void GraphMatrix::addEdge(int u, int v, double weight) {
    if (u < 0 || u >= n || v < 0 || v >= n) throw std::out_of_range("Vértice no creado");

    adjMatrix[u][v] = weight; // Guardamos el peso directamente
    if (!isDirected && u != v) {
        adjMatrix[v][u] = weight;
    }
}

void GraphMatrix::removeEdge(int u, int v) {
    if (u < 0 || u >= n || v < 0 || v >= n) return;

    adjMatrix[u][v] = 0.0;
    if (!isDirected && u != v) {
        adjMatrix[v][u] = 0.0;
    }
}

// 
double GraphMatrix::getWeight(int u, int v) const {
    if (u < 0 || u >= n || v < 0 || v >= n) return 0.0;
    return adjMatrix[u][v];
}

int GraphMatrix::getNumVertices() const {
    return n;
}

std::vector<int> GraphMatrix::getNeighbors(int vertex) const {
    if (vertex < 0 || vertex >= n) throw std::out_of_range("Vértice no creado");
    std::vector<int> neighbors;
    for (int i = 0; i < n; i++) {
        if (adjMatrix[vertex][i] != 0.0) { //Si el peso no es 0, hay conexion
            neighbors.push_back(i);
        }
    }
    return neighbors;
}

void GraphMatrix::print() const {
    std::cout << "Matriz de Adyacencia:\n";
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            std::cout << adjMatrix[i][j] << " ";
        }
        std::cout << "\n";
    }
}

//fin de matriz de adyacencia