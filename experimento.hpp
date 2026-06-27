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
    
    // Aquí pienso añadir ciertas funciones como:
    // ejecuta las métricas 10 veces para sacar la varianza.
    // static void ejecutarEstudioDeTiempos(Graph& g, int iteraciones = 10);
};

#endif // EXPERIMENTO_HPP