#pragma once

#include <string>
#include <unordered_map>
#include <vector>
#include "Graph.hpp" 
//rutaDataset = "datasets/yeast.edgelist"
//compilar asi g++ main_proteinas.cpp Graph.cpp parser_proteinas.cpp -o proyecto_grafos

//Estructura para guardar la traduccion entre nombres string y numeros enteros para los vertices
struct MapeoGrafo {
    std::unordered_map<std::string, int> nombre_a_id;
    std::vector<std::string> id_a_nombre;
};

// Declaración de la función principal del parser
MapeoGrafo cargarEdgeList(const std::string& rutaArchivo, Graph& grafo);