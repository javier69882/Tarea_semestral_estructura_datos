#ifndef EXPERIMENTO_HPP
#define EXPERIMENTO_HPP

#include "Graph.hpp"
#include "parser_trade.hpp"
#include <string>
#include <fstream>

class Experimento {
private:
    static void evaluarImpactoGlobal(const Graph& g, const std::string& nombreDataset, const std::string& estado, const std::string& arista, std::ofstream& csv);

public:
    static void ejecutarPerturbacion(Graph& g, const std::string& nombreDataset, std::ofstream& csv, const MapeoGrafo& traductor);
    
    
    static void ejecutarAumento(Graph& g, const std::string& nombreDataset, std::ofstream& csv, const MapeoGrafo& traductor);
};

#endif // EXPERIMENTO_HPP