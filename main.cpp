#include <iostream>
#include <chrono>
#include <iomanip>
#include <string>
#include <vector>
#include <fstream> 
#include <cmath>   
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



// OOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOO
// GUÍA DE COMPILACIÓN Y EJECUCIÓN (WSL / Linux)
// OOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOO

// 1. COMPILAR 
// g++ -O3 main.cpp Graph.cpp parser_proteinas.cpp parser_trade.cpp Centrality.cpp -o proyecto_grafos

// 2. EJECUCIONES RAPIDAS (Verificación de resultados por consola):
// ./proyecto_grafos proteinas
// ./proyecto_grafos trade

// 3. ESTUDIO EXPERIMENTAL (Genera archivos .csv de Centralidad y Memoria):
// ./proyecto_grafos trade_test resultados_trade.csv
// ./proyecto_grafos proteinas_test resultados_prot.csv

// 4. GENERAR GRÁFICOS CON PYTHON (Usar python3 en WSL):
 //en wsl hice 
//sudo apt update
//sudo apt install python3-pandas python3-matplotlib
//  Gráficos de rendimiento y tiempos de Centralidad:
// python3 plot_trade.py resultados_trade.csv
//
//  Gráficos de espacio en memoria RAM y tiempos de construcción:
// python3 plot_memoria.py memoria_trade.csv
// python3 plot_memoria.py memoria_prot.csv
// OOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOO




// oooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooo
//CALCULO DE MEMORIA DEL ADT 
// oooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooo
static double calcularMemoriaGrafo(const Graph& g) {
    int V = g.getNumVertices();

    // 1. Intentamos castear a Lista de Adyacencia
    const GraphList* listGraph = dynamic_cast<const GraphList*>(&g);
    if (listGraph != nullptr) {
        size_t bytesBase = sizeof(*listGraph); 
        size_t bytesVectores = V * sizeof(std::vector<std::pair<int, double>>); 
        size_t totalAristas = 0;
        for (int i = 0; i < V; ++i) {
            totalAristas += listGraph->getNeighbors(i).size();
        }
        size_t bytesAristas = totalAristas * sizeof(std::pair<int, double>); 
        return (bytesBase + bytesVectores + bytesAristas) / 1024.0; // A Kilobytes
    }

    // 2. Intentamos castear a Matriz de Adyacencia
    const GraphMatrix* matrixGraph = dynamic_cast<const GraphMatrix*>(&g);
    if (matrixGraph != nullptr) {
        size_t bytesBase = sizeof(*matrixGraph);
        // El vector externo contiene V vectores internos
        size_t bytesVectores = V * sizeof(std::vector<double>);
        // La matriz real ocupa V * V celdas de tipo double (8 bytes cada una)
        size_t bytesCeldas = (size_t)V * V * sizeof(double);
        return (bytesBase + bytesVectores + bytesCeldas) / 1024.0; // A Kilobytes
    }

    return 0.0; 
}
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

// OOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOO
// FUNCIONES MODO TEST / 10 ITERACIONES
// OOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOO

// Helper que corre 10 iteraciones y retorna el tiempo promedio y la varianza 
template <typename Func>
static std::pair<double, double> runTop5Experiment(const std::string& metricName, const MapeoGrafo& traductor, Func centralityFunc) {
    const int numRuns = 10;
    std::vector<double> executionTimes(numRuns);

    std::cout << "  -> Evaluando " << metricName << " (" << numRuns << " iteraciones)...\n";

    for (int i = 0; i < numRuns; ++i) {
        auto start = std::chrono::high_resolution_clock::now();
        centralityFunc(); 
        auto end = std::chrono::high_resolution_clock::now();
        
        std::chrono::duration<double, std::milli> duration = end - start;
        executionTimes[i] = duration.count(); 
    }

    double sum = 0.0;
    for (double t : executionTimes) sum += t;
    double mean = sum / numRuns;

    double varSum = 0.0;
    for (double t : executionTimes) varSum += (t - mean) * (t - mean);
    double variance = varSum / numRuns;

    return std::make_pair(mean, variance); 
}

template <typename Func>
static std::pair<double, double> runScalarExperiment(const std::string& metricName, Func centralityFunc) {
    const int numRuns = 10;
    std::vector<double> executionTimes(numRuns);

    std::cout << "  -> Evaluando " << metricName << " (" << numRuns << " iteraciones)...\n";

    for (int i = 0; i < numRuns; ++i) {
        auto start = std::chrono::high_resolution_clock::now(); 
        centralityFunc();
        auto end = std::chrono::high_resolution_clock::now(); 
        
        std::chrono::duration<double, std::milli> duration = end - start; 
        executionTimes[i] = duration.count(); 
    }

    double sum = 0.0;
    for (double t : executionTimes) sum += t;
    double mean = sum / numRuns;

    double varSum = 0.0;
    for (double t : executionTimes) varSum += (t - mean) * (t - mean);
    double variance = varSum / numRuns;

    return std::make_pair(mean, variance);
}

int main(int argc, char* argv[]) {
    // Validar el argumento
    if (argc < 2) {
        std::cerr << "Error: Faltan parametros.\n";
        std::cerr << "Uso correcto:\n";
        std::cerr << "  ./proyecto_grafos proteinas\n";
        std::cerr << "  ./proyecto_grafos trade\n";
        std::cerr << "  ./proyecto_grafos proteinas_test <archivo.csv>\n";
        std::cerr << "  ./proyecto_grafos trade_test <archivo.csv>\n";
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

// OOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOO
    // ESCENARIO 3: PROTEINAS TEST 
    // OOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOO
   else if (modo == "proteinas_test") {
       if (argc < 3) return 1;
        
        std::ofstream csvCentralidad(argv[2]);
        std::ofstream csvMemoria("memoria_prot.csv");
        
        auto startConst = std::chrono::high_resolution_clock::now();
        GraphList grafoProteinas(false); 
        MapeoGrafo traductor = cargarEdgeList("datasets/yeast.edgelist", grafoProteinas);
        auto endConst = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double, std::milli> t_const = endConst - startConst;

        int n = grafoProteinas.getNumVertices();
        if (n > 0) {
            size_t e = 0;
            for (int i = 0; i < n; ++i) e += grafoProteinas.getNeighbors(i).size();

            csvMemoria << "Dataset,Nodos,Aristas,Memoria_KB,TiempoConstruccion_ms\n";
            csvMemoria << "yeast.edgelist," << n << "," << e << "," << calcularMemoriaGrafo(grafoProteinas) << "," << t_const.count() << "\n";
            
            // Estructura duplicada para soportar columnas de varianza
            csvCentralidad << "n,Degree_mean,Degree_var,Betweenness_mean,Betweenness_var,Closeness_mean,Closeness_var,PageRank_mean,PageRank_var,ASP_mean,ASP_var,Diameter_mean,Diameter_var,Eigenvector_mean,Eigenvector_var\n";
            std::cout << "Procesando dataset: yeast.edgelist (Nodos: " << n << ")\n";

            auto t_deg = runTop5Experiment("Degree", traductor, [&]() { return Centrality::calculateDegreeCentrality(grafoProteinas, false); });
            auto t_bet = runTop5Experiment("Betweenness", traductor, [&]() { return Centrality::calculateBetweennessCentrality(grafoProteinas); });
            auto t_clo = runTop5Experiment("Closeness", traductor, [&]() { return Centrality::calculateClosenessCentrality(grafoProteinas); });
            auto t_pr  = runTop5Experiment("PageRank", traductor, [&]() { return Centrality::calculatePageRank(grafoProteinas); });
            auto t_asp = runScalarExperiment("Average Path", [&]() { return Centrality::calculateAverageShortestPath(grafoProteinas); });
            auto t_dia = runScalarExperiment("Diameter", [&]() { return Centrality::calculateNetworkDiameter(grafoProteinas); });
            auto t_eig = runTop5Experiment("Eigenvector", traductor, [&]() { return Centrality::calculateEigenvectorCentrality(grafoProteinas); });

            csvCentralidad << n << ',' 
                           << t_deg.first << ',' << t_deg.second << ','
                           << t_bet.first << ',' << t_bet.second << ','
                           << t_clo.first << ',' << t_clo.second << ','
                           << t_pr.first  << ',' << t_pr.second  << ','
                           << t_asp.first << ',' << t_asp.second << ','
                           << t_dia.first << ',' << t_dia.second << ','
                           << t_eig.first << ',' << t_eig.second << '\n';
            std::cout << "-> Datos guardados exitosamente!\n";
        }
    }
    // OOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOO
    // ESCENARIO 4: TRADE TEST (NUEVO EXPERIMENTO CSV)
    // OOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOO
    else if (modo == "trade_test") {
        if (argc < 3) return 1;

        std::ofstream csvCentralidad(argv[2]);
        std::ofstream csvMemoria("memoria_trade.csv"); 
        
        std::vector<std::string> archivosTrade = {"datasets/2000.net", "datasets/2005.net", "datasets/2010.net", "datasets/2015.net", "datasets/2018.net"};
        std::vector<GraphList> redesComerciales(archivosTrade.size(), GraphList(true));
        MapeoGrafo traductor = cargarTradeNetworks(archivosTrade, redesComerciales);

        csvCentralidad << "n,Degree_mean,Degree_var,Betweenness_mean,Betweenness_var,Closeness_mean,Closeness_var,PageRank_mean,PageRank_var,ASP_mean,ASP_var,Diameter_mean,Diameter_var,Eigenvector_mean,Eigenvector_var\n";
        csvMemoria << "Dataset,Nodos,Aristas,Memoria_KB,TiempoConstruccion_ms\n";

        for (size_t i = 0; i < archivosTrade.size(); ++i) {
            int n = redesComerciales[i].getNumVertices();
            if (n == 0) continue;

            auto startConst = std::chrono::high_resolution_clock::now();
            std::vector<std::string> unArchivo = { archivosTrade[i] };
            std::vector<GraphList> redUnica(1, GraphList(true));
            cargarTradeNetworks(unArchivo, redUnica);
            auto endConst = std::chrono::high_resolution_clock::now();
            std::chrono::duration<double, std::milli> t_const = endConst - startConst;

            size_t e = 0;
            for (int v = 0; v < n; ++v) e += redesComerciales[i].getNeighbors(v).size();

            csvMemoria << archivosTrade[i] << "," << n << "," << e << "," << calcularMemoriaGrafo(redesComerciales[i]) << "," << t_const.count() << "\n";
            std::cout << "Procesando dataset: " << archivosTrade[i] << " (Nodos: " << n << ")\n";

            auto t_deg = runTop5Experiment("Degree", traductor, [&]() { return Centrality::calculateDegreeCentrality(redesComerciales[i], true); });
            auto t_bet = runTop5Experiment("Betweenness", traductor, [&]() { return Centrality::calculateBetweennessCentrality(redesComerciales[i]); });
            auto t_clo = runTop5Experiment("Closeness", traductor, [&]() { return Centrality::calculateClosenessCentrality(redesComerciales[i]); });
            auto t_pr  = runTop5Experiment("PageRank", traductor, [&]() { return Centrality::calculatePageRank(redesComerciales[i]); });
            auto t_asp = runScalarExperiment("Average Path", [&]() { return Centrality::calculateAverageShortestPath(redesComerciales[i]); });
            auto t_dia = runScalarExperiment("Diameter", [&]() { return Centrality::calculateNetworkDiameter(redesComerciales[i]); });
            auto t_eig = runTop5Experiment("Eigenvector", traductor, [&]() { return Centrality::calculateEigenvectorCentrality(redesComerciales[i]); });

            csvCentralidad << n << ',' 
                           << t_deg.first << ',' << t_deg.second << ','
                           << t_bet.first << ',' << t_bet.second << ','
                           << t_clo.first << ',' << t_clo.second << ','
                           << t_pr.first  << ',' << t_pr.second  << ','
                           << t_asp.first << ',' << t_asp.second << ','
                           << t_dia.first << ',' << t_dia.second << ','
                           << t_eig.first << ',' << t_eig.second << '\n';
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