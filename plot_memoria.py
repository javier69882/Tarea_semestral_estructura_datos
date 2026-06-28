import sys
import os
import pandas as pd
import matplotlib.pyplot as plt

if len(sys.argv) != 2:
    print(f"Uso: python3 {sys.argv[0]} <memoria.csv>")
    exit()

csv_file = sys.argv[1]
df = pd.read_csv(csv_file)
os.makedirs('graficos', exist_ok=True)

# Limpiar los nombres de los archivos para que se vean bien en el gráfico
df['Dataset'] = df['Dataset'].apply(lambda x: x.split('/')[-1].replace('.net', '').replace('.edgelist', ''))

fig, axes = plt.subplots(1, 2, figsize=(14, 6))

# Gráfico 1: Memoria RAM
bars1 = axes[0].bar(df['Dataset'], df['Memoria_KB'], color='#1f77b4', edgecolor='black')
axes[0].set_title("Espacio en Memoria RAM del Grafo", fontweight='bold', fontsize=14)
axes[0].set_ylabel("Kilobytes (KB)", fontweight='bold')
axes[0].grid(axis='y', linestyle='--', alpha=0.7)

# Agregar etiquetas sobre las barras
for bar in bars1:
    yval = bar.get_height()
    axes[0].text(bar.get_x() + bar.get_width()/2, yval, f'{yval:.1f} KB', ha='center', va='bottom', fontsize=11)

# Gráfico 2: Tiempo de Construcción (Parsing)
bars2 = axes[1].bar(df['Dataset'], df['TiempoConstruccion_ms'], color='#ff7f0e', edgecolor='black')
axes[1].set_title("Tiempo de Construcción desde Archivo", fontweight='bold', fontsize=14)
axes[1].set_ylabel("Milisegundos (ms)", fontweight='bold')
axes[1].grid(axis='y', linestyle='--', alpha=0.7)

for bar in bars2:
    yval = bar.get_height()
    axes[1].text(bar.get_x() + bar.get_width()/2, yval, f'{yval:.2f} ms', ha='center', va='bottom', fontsize=11)

plt.suptitle("Análisis Estructural y de Construcción", fontsize=16, fontweight='bold', y=1.05)
plt.tight_layout()

# Guardar imagen
csv_base = os.path.splitext(os.path.basename(csv_file))[0]
png_file = os.path.join('graficos', f"{csv_base}_grafico.png")
plt.savefig(png_file, bbox_inches='tight', dpi=300)
print(f"Grafico guardado exitosamente como: {png_file}")

plt.show()