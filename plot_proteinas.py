import sys
import pandas as pd
import matplotlib.pyplot as plt

if len(sys.argv) != 2:
    print(f"Uso: python3 {sys.argv[0]} <file.csv>")
    sys.exit(1)

csv_file = sys.argv[1]
df = pd.read_csv(csv_file)

# Nombres limpios de las métricas evaluadas (asegurarse que coincidan con el CSV)
metricas = ["Degree", "Betweenness", "Closeness", "PageRank", "Avg Path", "Diameter", "Eigenvector"]

# Separamos las columnas de medias de las de varianzas basándonos en patrones de saltos
df_means = df.iloc[:, 1::2]
df_vars = df.iloc[:, 2::2]

df_means.columns = metricas
df_vars.columns = metricas

# Como el dataset de proteínas tiene solo 1 red, indexamos con su nombre
df_means.index = ['Yeast (Levadura)']
df_vars.index = ['Yeast (Levadura)']

# Transponemos para agrupar por métrica en el eje X
df_means_trans = df_means.T

# Generación del lienzo gráfico
plt.figure(figsize=(16, 9))
colormap_viridis = plt.cm.get_cmap('viridis')

# Al ser una sola serie, mapeamos los colores a lo largo de la paleta viridis para las 7 barras
colores = colormap_viridis([i / len(metricas) for i in range(len(metricas))])
ax = df_means_trans.plot(kind='bar', figsize=(16, 9), width=0.85, color=colores, edgecolor='black', legend=False)

# APLICAMOS ESCALA LOGARÍTMICA POR LA DIFERENCIA EXTREMA DE RENDIMIENTO
plt.yscale('log')

plt.title("Rendimiento por Métrica: Tiempo Promedio y Varianza (Proteínas)", fontsize=16, fontweight='bold')
plt.xlabel("Métricas de Centralidad", fontweight='bold', fontsize=12)
plt.ylabel("Tiempo de Ejecución (milisegundos) - Escala Logarítmica", fontweight='bold', fontsize=12)
plt.xticks(rotation=0)
plt.grid(axis='y', linestyle='--', alpha=0.5)

# Recorremos los contenedores de matplotlib para pintar los textos de promedio y varianza
for i, container in enumerate(ax.containers):
    # En este caso 'i' siempre será 0 porque hay una sola serie (Levadura)
    for j, bar in enumerate(container):
        metrica = metricas[j]
        mean_val = bar.get_height()
        var_val = df_vars.iloc[0, j] # Extraemos la varianza de la primera y única fila
        
        if mean_val > 0:
            # Formateamos la varianza en formato científico si es muy pequeña, o decimal estándar
            str_var = f"{var_val:.1e}" if var_val < 0.01 else f"{var_val:.2f}"
            texto_etiqueta = f"m: {mean_val:.2f}\nv: {str_var}"
            
            ax.annotate(texto_etiqueta, 
                        (bar.get_x() + bar.get_width() / 2., mean_val), 
                        ha='center', va='bottom', 
                        fontsize=8.5, rotation=90, xytext=(0, 6), 
                        textcoords='offset points')

# En escala logarítmica, para dar espacio arriba se multiplica en lugar de sumar
y_max = df_means_trans.max().max()
plt.ylim(ax.get_ylim()[0], y_max * 10)

plt.tight_layout()
png_file = csv_file.rsplit('.', 1)[0] + "_log_completo.png"
plt.savefig(png_file, dpi=300)
print(f"Grafico guardado exitosamente como: {png_file}")
plt.show()