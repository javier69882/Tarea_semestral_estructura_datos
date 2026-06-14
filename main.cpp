#include <iostream>
#include <chrono>
#include <iomanip>
#include <string>
#include <vector>

#include "Centrality.hpp"
#ifdef _WIN32
#include <windows.h>
#include <psapi.h>
#endif
#include "Graph.hpp"
#include "Centrality.hpp"
#include "parser_proteinas.hpp" 
#include "parser_trade.hpp" 
//COMPILADO
//g++ main.cpp Graph.cpp parser_proteinas.cpp parser_trade.cpp Centrality.cpp -o proyecto_grafos
//./proyecto_grafos proteinas
// O 
//./proyecto_grafos trade

#ifdef _WIN32
static std::size_t getCurrentProcessMemoryBytes() {
    PROCESS_MEMORY_COUNTERS_EX counters{};
    if (GetProcessMemoryInfo(GetCurrentProcess(), reinterpret_cast<PROCESS_MEMORY_COUNTERS*>(&counters), sizeof(counters))) {
        return static_cast<std::size_t>(counters.WorkingSetSize);
    }
    return 0;
}
#else
static std::size_t getCurrentProcessMemoryBytes() {
    return 0;
}
#endif

// benchmark de la metrica 5. Average Shortest Path, con medicion de tiempo
static void printAverageShortestPathBenchmark(const Graph& graph, const std::string& datasetLabel) {
    const auto start = std::chrono::steady_clock::now();

    const double averageShortestPath = Centrality::calculateAverageShortestPath(graph);

    const auto end = std::chrono::steady_clock::now();
    const auto elapsedMs = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();

    std::cout << "\n--- Average Shortest Path: " << datasetLabel << " ---\n";
    std::cout << std::fixed << std::setprecision(6);
    std::cout << "Resultado: " << averageShortestPath << "\n";
    std::cout << "Tiempo: " << elapsedMs << " ms\n";
    std::cout.unsetf(std::ios::floatfield);
}

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

            // benchmark de metrica 5. Average Shortest Path
            printAverageShortestPathBenchmark(grafoProteinas, "proteinas / yeast.edgelist");
        }
         // ==========================================
        // ANÁLISIS DE CENTRALIDAD: PAGERANK
        // ==========================================
        auto resultadoPR_Prot = Centrality::calculatePageRank(grafoProteinas); 

        std::vector<std::pair<int, double>> topProteinas = resultadoPR_Prot.first;
        double tiempoMs_Prot = resultadoPR_Prot.second;

        std::cout << "\nPageRank calculado en " << tiempoMs_Prot << " ms.\n";
        std::cout << "--- TOP 5 PROTEINAS MAS CRITICAS (Nodos Hub) ---\n";

        for (int i = 0; i < 5 && i < topProteinas.size(); i++) {
            int idNodo = topProteinas[i].first;
            double puntaje = topProteinas[i].second;
            
            std::cout << i + 1 << ". " << traductor.id_a_nombre[idNodo] 
                      << " (Puntaje: " << puntaje << ")\n";
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

            for (std::size_t i = 0; i < archivosTrade.size(); ++i) {
                // benchmark de metrica 5. Average Shortest Path
                printAverageShortestPathBenchmark(redesComerciales[i], archivosTrade[i]);
            }
        }
         auto resultadoPR = Centrality::calculatePageRank(redesComerciales[4]); // Año 2018

std::vector<std::pair<int, double>> topPaises = resultadoPR.first;
double tiempoMs = resultadoPR.second;

std::cout << "PageRank calculado en " << tiempoMs << " ms.\n";
std::cout << "--- TOP 5 POTENCIAS COMERCIALES (2018) ---\n";

for (int i = 0; i < 5 && i < topPaises.size(); i++) {
    int idNodo = topPaises[i].first;
    double puntaje = topPaises[i].second;
    
    std::cout << i + 1 << ". " << traductor.id_a_nombre[idNodo] 
              << " (Puntaje: " << puntaje << ")\n";
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