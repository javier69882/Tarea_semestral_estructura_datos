#ifndef EXPERIMENTO_HPP
#define EXPERIMENTO_HPP

#include "Graph.hpp"
#include <string>

class Experimento {
private:
    static void evaluarImpactoGlobal(const Graph& g, const std::string& estado);

public:
    // Función principal
    static void ejecutarPerturbacion(Graph& g);
};

#endif // EXPERIMENTO_HPP