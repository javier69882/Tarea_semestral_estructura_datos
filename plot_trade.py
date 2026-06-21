import sys
import pandas as pd
import matplotlib.pyplot as plt

if len(sys.argv) != 2:
    print(f"Uso: python3 {sys.argv[0]} <file.csv>")
    exit()

csv_file = sys.argv[1]
df = pd.read_csv(csv_file)

# 1. Preparar datos
df_times = df.iloc[:, 1:] 
df_times.columns = ["Degree", "Betweenness", "Closeness", "PageRank", "Avg Path", "Diameter", "Eigenvector"]
anios = ['2000', '2005', '2010', '2015', '2018']
df_times.index = anios

# 2. Agrupar datos (Métricas en el Eje X, Años como barras)
df_agrupado = df_times.T

# 3. Dibujar el gráfico usando ESCALA LINEAL (normal)
# Aumentamos un poco el largo de la imagen (figsize) para que quepan los textos
ax = df_agrupado.plot(kind='bar', figsize=(15, 8), width=0.85, colormap='viridis')

# 4. Configuración visual
plt.title("Comparación de Rendimiento por Año (Trade Network)", fontsize=16, fontweight='bold')
plt.xlabel("Métricas de Centralidad", fontweight='bold', fontsize=12)
plt.ylabel("Tiempo de Ejecución (milisegundos)", fontweight='bold', fontsize=12)

plt.xticks(rotation=0) # Mantener los nombres de las métricas en horizontal
plt.legend(title="Año del Dataset", fontsize='11')
plt.grid(axis='y', linestyle='--', alpha=0.7)

# 5. EL TRUCO: Anotar el valor exacto sobre cada una de las barras
for p in ax.patches:
    valor = p.get_height()
    if valor > 0:
        # Colocamos el texto rotado a 90 grados para que no choquen entre sí
        ax.annotate(f'{valor:.2f}', 
                    (p.get_x() + p.get_width() / 2., valor), 
                    ha='center', va='bottom', 
                    fontsize=9, rotation=90, xytext=(0, 5), 
                    textcoords='offset points')

# Dar un 25% extra de espacio en la parte superior para que los textos no se corten
plt.ylim(0, df_agrupado.max().max() * 1.25)

# Guardar y mostrar
plt.tight_layout()
png_file = csv_file.rsplit('.', 1)[0] + "_lineal_completo.png"
plt.savefig(png_file, dpi=300) # dpi=300 asegura calidad HD para el informe
print(f"Grafico guardado exitosamente como: {png_file}")

plt.show()