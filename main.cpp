#include <iostream>
#include <string>
#include "Graph.hpp"
#include "parser_proteinas.hpp" // 

int main() {
    //Creamos el grafo usando Lista de Adyacencia 
    GraphList grafoProteinas(false); 

    //Ruta dataset
    std::string rutaDataset = "datasets/yeast.edgelist"; 
    
    //Usamos el parser para rellenar el grafo
    std::cout << "Iniciando lectura de datos...\n";
    MapeoGrafo traductor = cargarEdgeList(rutaDataset, grafoProteinas);

    // 4. Pequeña prueba de cordura para confirmar que los datos están ahí
    if (grafoProteinas.getNumVertices() > 0) {
        std::cout << "\n--- Verificacion rapida ---\n";
        std::cout << "El nodo interno 0 corresponde a la proteina: " << traductor.id_a_nombre[0] << "\n";
        std::cout << "Grado (conexiones) del nodo 0: " << grafoProteinas.getNeighbors(0).size() << "\n";
        std::cout << "\n--- Imprimiendo estructura del ADT Grafo ---\n";
        grafoProteinas.print();   
    }

    return 0;
}