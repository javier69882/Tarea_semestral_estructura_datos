#include <iostream>
#include <string>
#include <vector>
#include "Graph.hpp"
#include "parser_proteinas.hpp" 
#include "parser_trade.hpp" 
//COMPILADO
//g++ main.cpp Graph.cpp parser_proteinas.cpp parser_trade.cpp -o proyecto_grafos
//./proyecto_grafos proteinas
// O 
//./proyecto_grafos trade
int main(int argc, char* argv[]) {
    // Validar el argumento
    if (argc < 2) {
        std::cerr << "Error: Faltan parametros.\n";
        std::cerr << "Uso correcto: ./proyecto_grafos [proteinas | trade]\n";
        return 1; // Terminamos el programa con error
    }

    // guardamos el modo de operación (proteinas o trade)
    std::string modo = argv[1];

    // OOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOO
    // ESCENARIO 1: RED DE PROTEINAS
    // OOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOO
    if (modo == "proteinas") {
        std::cout << "--- INICIANDO MODO: PROTEINAS (Grafo No Dirigido) ---\n";
        
        GraphList grafoProteinas(false); 
        std::string rutaDataset = "datasets/yeast.edgelist"; 
        
        MapeoGrafo traductor = cargarEdgeList(rutaDataset, grafoProteinas);

        if (grafoProteinas.getNumVertices() > 0) {
            std::cout << "\n--- Verificacion rapida ---\n";
            std::cout << "El nodo interno 0 corresponde a la proteina: " << traductor.id_a_nombre[0] << "\n";
            std::cout << "Grado (conexiones) del nodo 0: " << grafoProteinas.getNeighbors(0).size() << "\n";
        }
    } 
    // oooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooo
    // ESCENARIO 2: RED DE COMERCIO (TRADE)
    // oooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooo
    else if (modo == "trade") {
        std::cout << "--- INICIANDO MODO: TRADE NETWORK (Analisis Temporal Dirigido) ---\n";
        
        std::vector<std::string> archivosTrade = {
            "datasets/2000.net", "datasets/2005.net", "datasets/2010.net",
            "datasets/2015.net", "datasets/2018.net"
        };
        
        std::vector<GraphList> redesComerciales(archivosTrade.size(), GraphList(true));
        MapeoGrafo traductor = cargarTradeNetworks(archivosTrade, redesComerciales);

        if (redesComerciales[0].getNumVertices() > 0) {
            int idChina = traductor.nombre_a_id["CHN"];
            std::cout << "\n--- Evolucion de las exportaciones de CHINA ---\n";
            std::cout << "Paises a los que exportaba en 2000: " << redesComerciales[0].getNeighbors(idChina).size() << "\n";
            std::cout << "Paises a los que exportaba en 2018: " << redesComerciales[4].getNeighbors(idChina).size() << "\n";
        }
    } 
    // oooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooo
    // ESCENARIO 3: MODO DESCONOCIDO
    //ooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooo
    else {
        std::cerr << "Error: Modo '" << modo << "' no reconocido.\n";
        std::cerr << "Opciones validas: proteinas, trade\n";
        return 1;
    }

    return 0;
}