import sys
import os
import pandas as pd
import matplotlib.pyplot as plt

if len(sys.argv) != 2:
    print(f"Uso: python3 {sys.argv[0]} <file.csv>")
    sys.exit(1)

csv_file = sys.argv[1]
df = pd.read_csv(csv_file)
os.makedirs('graficos', exist_ok=True)

# Nombres limpios de las métricas evaluadas (deben coincidir con los encabezados del CSV)
metricas = ["Degree", "Betweenness", "Closeness", "PageRank", "ASP", "Diameter", "Eigenvector"]
col_mean = [f"{metrica}_mean" for metrica in metricas]
col_var = [f"{metrica}_var" for metrica in metricas]
col_mem = [f"{metrica}_mem_mean" for metrica in metricas]

df_means = df[col_mean].copy()
df_vars = df[col_var].copy()
df_mem = df[col_mem].copy()

df_means.columns = metricas
df_vars.columns = metricas
df_mem.columns = metricas

# Como el dataset de proteínas tiene solo 1 red, indexamos con su nombre
df_means.index = ['Yeast (Levadura)']
df_vars.index = ['Yeast (Levadura)']
df_mem.index = ['Yeast (Levadura)']

# Convertimos la fila única en una serie para graficar por métrica
serie_means = df_means.iloc[0]
serie_mem = df_mem.iloc[0]

# Generación del lienzo gráfico
plt.figure(figsize=(16, 9))
colormap_viridis = plt.get_cmap('viridis')

# Al ser una sola serie, mapeamos los colores a lo largo de la paleta viridis para las 7 barras
colores = colormap_viridis([i / len(metricas) for i in range(len(metricas))])
ax = serie_means.plot(kind='bar', figsize=(16, 9), width=0.85, color=colores, edgecolor='black', legend=False)

# APLICAMOS ESCALA LOGARÍTMICA POR LA DIFERENCIA EXTREMA DE RENDIMIENTO
plt.yscale('log')

plt.title("Rendimiento por Métrica: Tiempo Promedio y Varianza (Proteínas)", fontsize=16, fontweight='bold')
plt.xlabel("Métricas de Centralidad", fontweight='bold', fontsize=12)
plt.ylabel("Tiempo de Ejecución (milisegundos) - Escala Logarítmica", fontweight='bold', fontsize=12)
plt.xticks(rotation=0)
plt.grid(axis='y', linestyle='--', alpha=0.5)

# Recorremos los contenedores de matplotlib para pintar los textos de promedio y varianza
for j, bar in enumerate(ax.patches):
    mean_val = bar.get_height()
    var_val = df_vars.iloc[0, j]
    mem_val = serie_mem.iloc[j]

    if mean_val > 0:
        str_var = f"{var_val:.1e}" if var_val < 0.01 else f"{var_val:.2f}"
        texto_etiqueta = f"m: {mean_val:.2f}\nv: {str_var}\nmem: {mem_val:.4f} KB"

        ax.annotate(texto_etiqueta,
                    (bar.get_x() + bar.get_width() / 2., mean_val),
                    ha='center', va='bottom',
                    fontsize=8.0, rotation=90, xytext=(0, 6),
                    textcoords='offset points')

# En escala logarítmica, para dar espacio arriba se multiplica en lugar de sumar
y_max = serie_means.max()
plt.ylim(ax.get_ylim()[0], y_max * 10)

plt.tight_layout()
csv_base = os.path.splitext(os.path.basename(csv_file))[0]
png_file = os.path.join('graficos', f"{csv_base}_log_completo.png")
plt.savefig(png_file, dpi=300)
print(f"Grafico guardado exitosamente como: {png_file}")
plt.show()