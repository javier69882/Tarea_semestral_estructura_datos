#pragma once

#include <iostream>
#include <stdexcept>
#include <vector>
#include <algorithm>

class Graph {
protected:
    bool isDirected;

public:
    Graph(bool directed = false);
    virtual ~Graph() = default;
    virtual void addVertex() = 0;
    virtual void addEdge(int src, int dest) = 0;
    virtual void removeEdge(int src, int dest) = 0;
    virtual void print() const = 0;
    virtual int getNumVertices() const = 0;
    virtual std::vector<int> getNeighbors(int vertex) const = 0;
};

//Declaración de Lista de Adyacencia 
class GraphList : public Graph {
private:
    std::vector<std::vector<int>> adjList;

public:
    GraphList(bool directed = false);

    void addVertex() override;
    void addEdge(int u, int v) override;
    void removeEdge(int u, int v) override;
    int getNumVertices() const override;
    std::vector<int> getNeighbors(int vertex) const override;
    void print() const override;
};

//Declaración de Matriz de Adyacencia 
class GraphMatrix : public Graph {
private:
    std::vector<std::vector<int>> adjMatrix;
    int n;

public:
    GraphMatrix(bool directed = false);
    GraphMatrix(int V, bool directed = false);

    void addVertex() override;
    void addEdge(int u, int v) override;
    void removeEdge(int u, int v) override;
    int getNumVertices() const override;
    std::vector<int> getNeighbors(int vertex) const override;
    void print() const override;
};