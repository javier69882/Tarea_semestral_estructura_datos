#include "parser_proteinas.hpp"
#include <iostream>
#include <fstream>

MapeoGrafo cargarEdgeList(const std::string& rutaArchivo, Graph& grafo) {
    std::ifstream archivo(rutaArchivo);
    MapeoGrafo mapa;
    
    if (!archivo.is_open()) {
        std::cerr << "Error: No se pudo abrir el archivo " << rutaArchivo << "\n";
        return mapa;
    }

    std::string nodoOrigen, nodoDestino;
    int idActual = 0;
 
    //Leer el archivo linea por linea 
    while (archivo >> nodoOrigen >> nodoDestino) {
        
        //Ignorar aristas con datos faltantes ("-") ---
        if (nodoOrigen == "-" || nodoDestino == "-") {
            continue; // Salta a la siguiente línea del archivo
        }

        //Si no conocemos el nodo origen, lo registramos y creamos el vertice
        if (mapa.nombre_a_id.find(nodoOrigen) == mapa.nombre_a_id.end()) {
            mapa.nombre_a_id[nodoOrigen] = idActual++;
            mapa.id_a_nombre.push_back(nodoOrigen);
            grafo.addVertex(); 
        }

        //Si no conocemos el nodo destino, lo registramos y creamos el vértice
        if (mapa.nombre_a_id.find(nodoDestino) == mapa.nombre_a_id.end()) {
            mapa.nombre_a_id[nodoDestino] = idActual++;
            mapa.id_a_nombre.push_back(nodoDestino);
            grafo.addVertex(); 
        }

        // Obtenemos los IDs numericos
        int u = mapa.nombre_a_id[nodoOrigen];
        int v = mapa.nombre_a_id[nodoDestino];

        // Evitamos los nodos conectados a si mismos
        if (u != v) {
            grafo.addEdge(u, v);
        }
    }

    archivo.close();
    std::cout << "-> Dataset [" << rutaArchivo << "] cargado exitosamente.\n";
    std::cout << "-> Total de nodos unicos: " << grafo.getNumVertices() << "\n";
    
    return mapa;
}