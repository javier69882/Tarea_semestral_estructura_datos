#pragma once

#include <iostream>
#include <stdexcept>
#include <vector>
#include <algorithm>
#include <utility> 
class Graph {
protected:
    bool isDirected;

public:
    Graph(bool directed = false);
    virtual ~Graph() = default;
    virtual void addVertex() = 0;
    virtual void addEdge(int src, int dest, double weight = 1.0) = 0;
    virtual double getWeight(int u, int v) const = 0; //función para sacar el peso
    virtual void removeEdge(int src, int dest) = 0;
    virtual void print() const = 0;
    virtual int getNumVertices() const = 0;
    virtual std::vector<int> getNeighbors(int vertex) const = 0;
};

//Declaración de Lista de Adyacencia 
class GraphList : public Graph {
private:
    std::vector<std::vector<std::pair<int, double>>> adjList;

public:
    GraphList(bool directed = false);

    void addVertex() override;
    void addEdge(int u, int v, double weight = 1.0) override;
    double getWeight(int u, int v) const override;
    void removeEdge(int u, int v) override;
    int getNumVertices() const override;
    std::vector<int> getNeighbors(int vertex) const override;
    void print() const override;
};

//Declaración de Matriz de Adyacencia 
class GraphMatrix : public Graph {
private:
   std::vector<std::vector<double>> adjMatrix;
    int n;

public:
    GraphMatrix(bool directed = false);
    GraphMatrix(int V, bool directed = false);

    void addVertex() override;
    void addEdge(int u, int v, double weight = 1.0) override;
    double getWeight(int u, int v) const override;
    void removeEdge(int u, int v) override;
    int getNumVertices() const override;
    std::vector<int> getNeighbors(int vertex) const override;
    void print() const override;
};