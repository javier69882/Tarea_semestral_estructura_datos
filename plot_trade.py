import sys
import os
import pandas as pd
import matplotlib.pyplot as plt

if len(sys.argv) != 2:
    print(f"Uso: python3 {sys.argv[0]} <file.csv>")
    exit()

csv_file = sys.argv[1]
df = pd.read_csv(csv_file)
os.makedirs('graficos', exist_ok=True)

# Nombres limpios de las métricas evaluadas
metricas = ["Degree", "Betweenness", "Closeness", "PageRank", "ASP", "Diameter", "Eigenvector"]
anios = ['2000', '2005', '2010', '2015', '2018']

columnas_mean = [f"{metrica}_mean" for metrica in metricas]
columnas_var = [f"{metrica}_var" for metrica in metricas]
columnas_mem = [f"{metrica}_mem_mean" for metrica in metricas]

# Separamos las columnas por nombre para evitar depender de la posición
df_means = df[columnas_mean].copy()
df_vars = df[columnas_var].copy()
df_mem = df[columnas_mem].copy()

df_means.columns = metricas
df_vars.columns = metricas
df_mem.columns = metricas
df_means.index = anios
df_vars.index = anios
df_mem.index = anios

# Transponemos para agrupar por métrica en el eje X
df_means_trans = df_means.T

# Generación del lienzo gráfico (Escala Lineal para notar la diferencia de rendimiento)
plt.figure(figsize=(16, 9))
colormap_viridis = plt.get_cmap('viridis')
ax = df_means_trans.plot(kind='bar', figsize=(16, 9), width=0.85, colormap=colormap_viridis, edgecolor='black')

plt.title("Rendimiento por Año: Tiempo Promedio y Varianza (Trade Network)", fontsize=16, fontweight='bold')
plt.xlabel("Métricas de Centralidad", fontweight='bold', fontsize=12)
plt.ylabel("Tiempo de Ejecución (milisegundos)", fontweight='bold', fontsize=12)
plt.xticks(rotation=0)
plt.grid(axis='y', linestyle='--', alpha=0.5)
plt.legend(title="Año del Dataset", fontsize='11')

# Recorremos los contenedores de matplotlib para pintar los textos de promedio y varianza
for i, container in enumerate(ax.containers):
    anio = anios[i]
    for j, bar in enumerate(container):
        metrica = metricas[j]
        mean_val = bar.get_height()
        var_val = df_vars.loc[anio, metrica]
        mem_val = df_mem.loc[anio, metrica]
        
        if mean_val > 0:
            # Formateamos la varianza en formato científico si es muy pequeña, o decimal estándar
            str_var = f"{var_val:.1e}" if var_val < 0.01 else f"{var_val:.8f}"
            texto_etiqueta = f"m: {mean_val:.8f}\nv: {str_var}\nmem: {mem_val:.8f} KB"
            
            ax.annotate(texto_etiqueta, 
                        (bar.get_x() + bar.get_width() / 2., mean_val), 
                        ha='center', va='bottom', 
                        fontsize=7.0, rotation=90, xytext=(0, 6), 
                        textcoords='offset points')

# Brindamos espacio arriba para evitar que el texto choque con el borde superior
plt.ylim(0, df_means_trans.max().max() * 1.35)

plt.tight_layout()
csv_base = os.path.splitext(os.path.basename(csv_file))[0]
png_file = os.path.join('graficos', f"{csv_base}_lineal_completo.png")
plt.savefig(png_file, dpi=300)
print(f"Grafico guardado exitosamente como: {png_file}")
plt.show()