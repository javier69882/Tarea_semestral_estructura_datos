#include "Graph.hpp"

// Implementación de la clase base Graph
Graph::Graph(bool directed) : isDirected(directed){

}


// Implementacion de lista de adyacencia

GraphList::GraphList(bool directed) : Graph(directed) {}

void GraphList::addVertex() { 
    adjList.push_back(std::vector<int>()); 
}

void GraphList::addEdge(int u, int v) {
    if (u < 0 || u >= adjList.size() || v < 0 || v >= adjList.size()) {
        throw std::out_of_range("Vértice no creado");
    }

    // busco duplicados antes de insertar
    //necesario, porque en el dataset de proteinas, que es no dirigido, hay aristas que podrian quedar repetidas, error que con matriz de adyacencia no ocurre
    for (int vecino : adjList[u]) {
        if (vecino == v) {
            return; // si ya son vecinos, salimos inmediatamente y no insertamos nada
        }
    }

    //si el bucle termina y no se activo el return, significa que la arista es nueva
    adjList[u].push_back(v);
    
    if (!isDirected && u != v) {
        adjList[v].push_back(u);
    }
}

void GraphList::removeEdge(int u, int v) {
    if (u < 0 || u >= adjList.size() || v < 0 || v >= adjList.size()) return;

    adjList[u].erase(std::remove(adjList[u].begin(), adjList[u].end(), v), adjList[u].end());

    if (!isDirected && u != v) {
        adjList[v].erase(std::remove(adjList[v].begin(), adjList[v].end(), u), adjList[v].end());
    }
}

int GraphList::getNumVertices() const {
    return adjList.size();
}

std::vector<int> GraphList::getNeighbors(int vertex) const {
    if (vertex < 0 || vertex >= adjList.size()) throw std::out_of_range("Vértice no creado");
    return adjList[vertex];
}

void GraphList::print() const {
    std::cout << "Lista de adyacencia:\n";
    for (size_t i = 0; i < adjList.size(); i++) {
        std::cout << i << ":";
        for (int neighbor : adjList[i]) {
            std::cout << " -> " << neighbor;
        }
        std::cout << "\n";
    }
}

// fin lista de ayacencia



// Implementación de GraphMatrix

GraphMatrix::GraphMatrix(bool directed) : Graph(directed), n(0) {}

GraphMatrix::GraphMatrix(int V, bool directed) : Graph(directed), n(V) { 
    adjMatrix.resize(n, std::vector<int>(n, 0)); 
}

void GraphMatrix::addVertex() {
    n++;
    for (auto &row : adjMatrix) {
        row.push_back(0);
    }
    adjMatrix.push_back(std::vector<int>(n, 0));
}

void GraphMatrix::addEdge(int u, int v) {
    if (u < 0 || u >= n || v < 0 || v >= n) throw std::out_of_range("Vértice no creado");

    adjMatrix[u][v] = 1;
    if (!isDirected && u != v) {
        adjMatrix[v][u] = 1;
    }
}

void GraphMatrix::removeEdge(int u, int v) {
    if (u < 0 || u >= n || v < 0 || v >= n) return;

    adjMatrix[u][v] = 0;
    if (!isDirected && u != v) {
        adjMatrix[v][u] = 0;
    }
}

int GraphMatrix::getNumVertices() const {
    return n;
}

std::vector<int> GraphMatrix::getNeighbors(int vertex) const {
    if (vertex < 0 || vertex >= n) throw std::out_of_range("Vértice no creado");
    std::vector<int> neighbors;
    for (int i = 0; i < n; i++) {
        if (adjMatrix[vertex][i] == 1) {
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