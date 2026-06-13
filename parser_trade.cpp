#include "parser_trade.hpp"
#include <iostream>
#include <fstream>
#include <sstream>

MapeoGrafo cargarTradeNetworks(const std::vector<std::string>& archivos, std::vector<GraphList>& grafos) {
    MapeoGrafo mapa;
    if (archivos.empty() || grafos.size() != archivos.size()) {
        std::cerr << "Error: La cantidad de archivos no coincide con la cantidad de grafos.\n";
        return mapa;
    }

    std::cout << "Iniciando lectura de la Red de Comercio (Analisis Temporal)...\n";

    for (size_t i = 0; i < archivos.size(); i++) {
        std::ifstream archivo(archivos[i]);
        if (!archivo.is_open()) {
            std::cerr << "Error abriendo " << archivos[i] << "\n";
            continue;
        }

        std::string linea;
        bool leyendoVertices = false;
        bool leyendoArcos = false;

        while (std::getline(archivo, linea)) {
            if (linea.empty()) continue; 

            // Detectar secciones
            if (linea.find("*Vertices") != std::string::npos) {
                leyendoVertices = true;
                leyendoArcos = false;
                continue; // Saltamos a la siguiente línea
            }
            if (linea.find("*Arcs") != std::string::npos) {
                leyendoVertices = false;
                leyendoArcos = true;
                continue; // Saltamos a la siguiente línea
            }

            // Procesar Países (Nodos)
            if (leyendoVertices) {
                std::stringstream ss(linea);
                int idPajek;
                std::string nombreComillas;
                
                ss >> idPajek >> nombreComillas;
                
                //quito comillas ("CHL" -> CHL)
                std::string nombreLimpio = "";
                for(char c : nombreComillas) {
                    if(c != '"') nombreLimpio += c;
                }

                // Solo llenamos el diccionario traductor en el primer archivo año 2000, porque se mantiene
                if (i == 0) {
                    mapa.nombre_a_id[nombreLimpio] = idPajek - 1;
                    mapa.id_a_nombre.push_back(nombreLimpio);
                }
                
                grafos[i].addVertex(); 
            }

            // Procesar Conexiones (Aristas)
            if (leyendoArcos) {
                std::stringstream ss(linea);
                int u, v;
                double peso;
                if (ss >> u >> v >> peso) {
                    //restamos 1 porque Pajek parte en 1 y C++ en 0
                    grafos[i].addEdge(u - 1, v - 1, peso);
                }
            }
        }
        archivo.close();
        std::cout << "-> Red del archivo [" << archivos[i] << "] cargada con exito.\n";
    }
    
    return mapa;
}