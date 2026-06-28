# Script python que grafica resultados CSV
# *Instalar librerias necesarias via pip
# $ pip install pandas matplotlib

# en wsl hice 
#sudo apt update
#sudo apt install python3-pandas python3-matplotlib

import sys
import os
import pandas as pd
import matplotlib.pyplot as plt

# Se pasa por argumento el nombre del archivo
if len(sys.argv) != 2:
    print(f"Uso: {sys.argv[0]} <file1.csv>") 
    exit()

# Convertir archivo CSV a DataFrame (tabla de datos)
csv_file = sys.argv[1]
df = pd.read_csv(csv_file) # Da error si no existe el archivo
os.makedirs('graficos', exist_ok=True)

# Graficar curvas de cada algoritmo (seleccionar las columnas por índice)
sizes = df.iloc[:,0]      # Columna 0: n (Nodos)
t_deg = df.iloc[:,1]      # Columna 1: Degree
t_bet = df.iloc[:,2]      # Columna 2: Betweenness
t_clo = df.iloc[:,3]      # Columna 3: Closeness
t_pr  = df.iloc[:,4]      # Columna 4: PageRank
t_asp = df.iloc[:,5]      # Columna 5: ASP
t_dia = df.iloc[:,6]      # Columna 6: Diameter
t_eig = df.iloc[:,7]      # Columna 7: Eigenvector

plt.figure(figsize=(10, 6))

# Trazar todas las líneas con sus etiquetas
plt.plot(sizes, t_deg, label="Degree", marker='o')
plt.plot(sizes, t_bet, label="Betweenness", marker='o')
plt.plot(sizes, t_clo, label="Closeness", marker='o')
plt.plot(sizes, t_pr, label="PageRank", marker='o')
plt.plot(sizes, t_asp, label="Avg Shortest Path", marker='o')
plt.plot(sizes, t_dia, label="Diameter", marker='o')
plt.plot(sizes, t_eig, label="Eigenvector", marker='o')

# Configurar gráfico
plt.xlabel("Tamaño (n)") 
plt.ylabel("Tiempo (ms)") 
plt.title("Rendimiento de Métricas de Centralidad")
plt.legend() 
plt.grid() 

# Guardar automáticamente la imagen
csv_base = os.path.splitext(os.path.basename(csv_file))[0]
png_file = os.path.join('graficos', f"{csv_base}.png")
plt.savefig(png_file) # Opcionalmente guardar gráfico
print(f"Grafico guardado como: {png_file}")

plt.show() 