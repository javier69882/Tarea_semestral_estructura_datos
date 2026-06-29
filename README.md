# Tarea_semestral_estructura_datos

# Proyecto de Análisis de Grafos y Métricas de Centralidad

Este repositorio contiene la implementación en C++ de un sistema de análisis de redes complejas, enfocado en el cálculo de métricas de centralidad y la evaluación del comportamiento estructural ante perturbaciones dinámicas.



## Estructura del Repositorio

A continuación se describe la distribución y el propósito de cada componente del proyecto:

### 1. Código Fuente en C++
* **`Graph.cpp` / `Graph.hpp`**: Contienen la definición de la clase base abstracta y la implementación detallada de las estructuras de lista y matriz de adyacencia.
* **`Centrality.cpp` / `Centrality.hpp`**: Módulo que contiene la lógica matemática y algorítmica para el cálculo de las **7 métricas de centralidad** del proyecto.
* **`experimento.cpp` / `experimento.hpp`**: Gestionan el experimento secundario de perturbación de la red. Sus funciones principales incluyen:
  * Decidir y ejecutar la adición o eliminación dinámica de aristas.
  * Recopilar los datos de las métricas resultantes tras las modificaciones.
  * Reportar por consola el **Top 30** de nodos para las métricas vectoriales y el valor único para las métricas globales.
* **`main.cpp`**: Punto de entrada principal que coordina los flujos de lectura, parsing y ejecución de experimentos.

### 2. Parsers de Datos
* **`parser_proteinas.cpp` / `parser_proteinas.hpp`**: Módulo encargado de procesar y estructurar el dataset de interacciones proteína-proteína (PPI).
* **`parser_trade.cpp` / `parser_trade.hpp`**: Módulo especializado en la lectura y procesamiento de la red de comercio internacional (Trade Network).

### 3. Scripts de Visualización (Python)
Scripts encargados de procesar las métricas exportadas por C++ para transformarlas en gráficos y representaciones visuales:
* **`plot_memoria.py`**: Compara el rendimiento y consumo de memoria entre las representaciones de grafos.
* **`plot_perturbacion.py`**: Grafica los efectos del cambio estructural sobre el comportamiento global de la red.
* **`plot_proteinas.py`**: Genera los gráficos analíticos del dataset de proteínas.
* **`plot_trade.py`**: Despliega las representaciones visuales de la red comercial.
* **`plot.py`**: Script base de apoyo para la manipulación y graficado de datos.

### 4. Directorios del Proyecto
* **`datasets/`**: Almacena las redes originales de entrada (archivos temporales `.net` por año como `2000.net`, `2005.net`, etc., junto con listas de adyacencia como `yeast.edgelist`).
* **`csv/`**: Directorio donde el programa en C++ exporta de forma estructurada los resultados de las simulaciones y métricas en formato `.csv`.
* **`graficos/`**: Carpeta de destino donde se guardan las imágenes y figuras generadas por los scripts de Python.

### 5. Binarios y Documentación
* **`proyecto_grafos`**: Archivo ejecutable compilado para entornos basados en Linux/Unix.
* **`proyecto_grafos.exe`**: Archivo ejecutable para entornos Windows.
* **`README.md`**: Archivo de documentación del proyecto.

## Compilación y Ejecución (WSL / Linux)

### 1. Compilación
Para compilar el proyecto utilizando el Subsistema de Windows para Linux (WSL) o cualquier entorno Linux, ejecuta el siguiente comando en la raíz del repositorio. El flag `-O3` se utiliza para maximizar la optimización del código compilado:

```bash
g++ -O3 main.cpp Graph.cpp parser_proteinas.cpp parser_trade.cpp Centrality.cpp Experimento.cpp -o proyecto_grafos
```

### 2. Ejecuciones Rápidas
Estas opciones permiten ejecutar el programa y verificar los resultados de las métricas directamente por consola, sin exportar datos:

```bash
./proyecto_grafos proteinas
./proyecto_grafos trade
```

### 3. Estudio Experimental Principal
Estos comandos ejecutan el análisis completo y generan los archivos `.csv` correspondientes a las métricas de centralidad y uso de memoria:

```bash
./proyecto_grafos trade_test resultados_trade.csv
./proyecto_grafos proteinas_test resultados_prot.csv
```

### 4. Experimentos de Perturbación Estructural
Evalúan el comportamiento de la red al modificar dinámicamente sus conexiones.

**Quitar aristas (Perturbación):**
```bash
./proyecto_grafos perturbacion_trade
./proyecto_grafos perturbacion_proteinas
```

**Aumentar aristas:**
```bash
./proyecto_grafos aumento_trade
./proyecto_grafos aumento_proteinas
```

### 5. Generación de Gráficos (Python)
Una vez generados los archivos `.csv` mediante los experimentos, puedes usar los scripts de Python para visualizar los resultados. Asegúrate de tener las dependencias necesarias instaladas (como `matplotlib` o `pandas`).

**Gráficos de espacio en memoria RAM y tiempos de construcción:**
```bash
python3 plot_memoria.py csv/memoria_trade.csv
python3 plot_memoria.py csv/memoria_prot.csv
```

**Gráficos de rendimiento algorítmico**

```bash
python3 plot_trade.py csv/resultados_trade.csv
python3 plot_proteinas.py csv/resultados_prot.csv
```

**Gráficos de perturbación (eliminación y aumento de aristas):**
```bash
python3 plot_perturbacion.py csv/resultados_perturbacion_trade.csv
python3 plot_perturbacion.py csv/resultados_perturbacion_prot.csv
python3 plot_perturbacion.py csv/resultados_aumento_trade.csv
python3 plot_perturbacion.py csv/resultados_aumento_prot.csv
```