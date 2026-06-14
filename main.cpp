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
// benchmark de la metrica 3. Closeness Centrality, con medicion de tiempo
static void printClosenessCentralityBenchmark(const Graph& graph, const MapeoGrafo& traductor, const std::string& datasetLabel, const std::string& topLabel) {
    const auto start = std::chrono::steady_clock::now();

    const auto resultadoCloseness = Centrality::calculateClosenessCentrality(graph);

    const auto end = std::chrono::steady_clock::now();
    const auto elapsedMs = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();

    const std::vector<std::pair<int, double>>& topNodos = resultadoCloseness.first;
    const double tiempoMs = resultadoCloseness.second;

    std::cout << "\n--- Closeness Centrality: " << datasetLabel << " ---\n";
    std::cout << "Closeness calculado en " << tiempoMs << " ms.\n";
    std::cout << "Tiempo total del benchmark: " << elapsedMs << " ms\n";
    std::cout << "--- TOP 5 " << topLabel << " ---\n";

    std::cout << std::fixed << std::setprecision(6);
    for (int i = 0; i < 5 && i < static_cast<int>(topNodos.size()); i++) {
        const int idNodo = topNodos[i].first;
        const double puntaje = topNodos[i].second;

        std::cout << i + 1 << ". " << traductor.id_a_nombre[idNodo]
                  << " (Puntaje: " << puntaje << ")\n";
    }
    std::cout.unsetf(std::ios::floatfield);
}

// benchmark de la metrica 4. PageRank, con medicion de tiempo
static void printPageRankBenchmark(const Graph& graph, const MapeoGrafo& traductor, const std::string& datasetLabel, const std::string& topLabel) {
    const auto start = std::chrono::steady_clock::now();

    const auto resultadoPR = Centrality::calculatePageRank(graph);

    const auto end = std::chrono::steady_clock::now();
    const auto elapsedMs = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();

    const std::vector<std::pair<int, double>>& topNodos = resultadoPR.first;
    const double tiempoMs = resultadoPR.second;

    std::cout << "\n--- PageRank: " << datasetLabel << " ---\n";
    std::cout << "PageRank calculado en " << tiempoMs << " ms.\n";
    std::cout << "Tiempo total del benchmark: " << elapsedMs << " ms\n";
    std::cout << "--- TOP 5 " << topLabel << " ---\n";

    std::cout << std::fixed << std::setprecision(6);
    for (int i = 0; i < 5 && i < static_cast<int>(topNodos.size()); i++) {
        const int idNodo = topNodos[i].first;
        const double puntaje = topNodos[i].second;

        std::cout << i + 1 << ". " << traductor.id_a_nombre[idNodo]
                  << " (Puntaje: " << puntaje << ")\n";
    }
    std::cout.unsetf(std::ios::floatfield);
}

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
        if (grafoProteinas.getNumVertices() > 0) {
            printPageRankBenchmark(grafoProteinas, traductor, "proteinas / yeast.edgelist", "TOP 5 PROTEINAS MAS CRITICAS (Nodos Hub)");
        }
        // benchmark de metrica 3. Closeness Centrality
         if (grafoProteinas.getNumVertices() > 0) {
            printClosenessCentralityBenchmark(grafoProteinas, traductor, "proteinas / yeast.edgelist", "PROTEINAS CON MAYOR CERCANIA (Acceso rapido)");
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
        if (redesComerciales[4].getNumVertices() > 0) {
            printPageRankBenchmark(redesComerciales[4], traductor, "trade / 2018.net", "TOP 5 POTENCIAS COMERCIALES (2018)");
        }
        // benchmark de metrica 3. Closeness Centrality
        if (redesComerciales[4].getNumVertices() > 0) {
            printClosenessCentralityBenchmark(redesComerciales[4], traductor, "trade / 2018.net", "POTENCIAS COMERCIALES CON MAYOR CERCANIA (Acceso rapido)");
        }
    } 
    
    // escenario no reconocido
    else {
        std::cerr << "Error: Modo '" << modo << "' no reconocido.\n";
        std::cerr << "Opciones validas: proteinas, trade\n";
        return 1;
    }

    return 0;
}