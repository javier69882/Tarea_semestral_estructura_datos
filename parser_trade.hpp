#pragma once

#include <string>
#include <vector>
#include "Graph.hpp"
#include "parser_proteinas.hpp" // Reutilizo struct MapeoGrafo del de las proteinas

//recibe un vector de grafos para llenarlos todos
MapeoGrafo cargarTradeNetworks(const std::vector<std::string>& archivos, std::vector<GraphList>& grafos);