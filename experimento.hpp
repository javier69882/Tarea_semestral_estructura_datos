#ifndef EXPERIMENTO_HPP
#define EXPERIMENTO_HPP

#include "Graph.hpp"
#include "parser_trade.hpp"
#include <string>
#include <fstream>

/**
 * @class Experimento
 * @brief Clase encargada de ejecutar las pruebas de resiliencia y topología sobre los grafos.
 */

class Experimento {
private:
    /**
     * @brief Método auxiliar que recalcula las métricas globales del grafo y registra los resultados.
     * @param g Referencia constante al grafo en su estado actual.
     * @param nombreDataset Nombre del dataset procesado (ej. "yeast", "2018").
     * @param estado Etiqueta que describe la fase del experimento.
     * @param arista Descripción en texto de los nodos involucrados.
     * @param csv Flujo de salida (ofstream) abierto donde se volcarán los datos.
     * @param traductor Estructura MapeoGrafo para traducir los IDs.
     */
    static void evaluarImpactoGlobal(const Graph& g, const std::string& nombreDataset, const std::string& estado, const std::string& arista, std::ofstream& csv, const MapeoGrafo& traductor);
public:
    /**
     * @brief Ejecuta el protocolo de Perturbación (Eliminación de Aristas).
     * @param g Referencia al grafo a perturbar.
     * @param nombreDataset Nombre de la red para el registro.
     * @param csv Flujo de salida para registrar el impacto.
     * @param traductor Estructura MapeoGrafo para traducir los IDs de los nodos.
     */
    static void ejecutarPerturbacion(Graph& g, const std::string& nombreDataset, std::ofstream& csv, const MapeoGrafo& traductor);
        /**
     * @brief Ejecuta el protocolo de Aumento (Creación de Atajos).
     * @param g Referencia al grafo a aumentar.
     * @param nombreDataset Nombre de la red para el registro.
     * @param csv Flujo de salida.
     * @param traductor Estructura MapeoGrafo para traducir los IDs de los nodos.
     */
    static void ejecutarAumento(Graph& g, const std::string& nombreDataset, std::ofstream& csv, const MapeoGrafo& traductor);
};

#endif // EXPERIMENTO_HPP