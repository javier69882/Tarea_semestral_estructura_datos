import pandas as pd
import matplotlib.pyplot as plt
import numpy as np
import sys
import os

if len(sys.argv) < 2:
    print("Error: Falta el archivo CSV.")
    sys.exit(1)

archivo_csv = sys.argv[1]

try:
    df = pd.read_csv(archivo_csv)
    os.makedirs('graficos', exist_ok=True)
    
    if df['Dataset'].dtype == object:
        df['Dataset'] = df['Dataset'].str.replace('datasets/', '').str.replace('.net', '')

    # Diccionario para buscar rápidamente el nombre de la arista
    dict_aristas = {(str(row['Dataset']), row['Estado']): row['Arista'] for _, row in df.iterrows()}

    metricas = [
        ('Deg_Val', 'Degree Centrality (Top 1)'),
        ('Bet_Val', 'Betweenness Centrality (Top 1)'),
        ('Clo_Val', 'Closeness Centrality (Top 1)'),
        ('PR_Val',  'PageRank (Top 1)'),
        ('ASP_Val', 'Average Shortest Path (Global)'),
        ('Dia_Val', 'Network Diameter (Global)'),
        ('Eig_Val', 'Eigenvector Centrality (Top 1)')
    ]

    fig, axes = plt.subplots(nrows=4, ncols=2, figsize=(18, 24))
    axes = axes.flatten()

    for i, (col_val, titulo) in enumerate(metricas):
        df_pivot = df.pivot(index='Dataset', columns='Estado', values=col_val)
        
        # Graficamos SIN leyenda en los subgráficos
        df_pivot.plot(kind='bar', ax=axes[i], edgecolor='black', legend=False)
        
        # --- CÁLCULO DE ESCALAS Y MÁRGENES ---
        min_val = df_pivot.min().min()
        max_val = df_pivot.max().max()
        rango = max_val - min_val
        if pd.isna(rango) or rango == 0:
            rango = max_val if max_val > 0 else 1.0
        
        # Espacio extra arriba para el número, y abajo (negativo) para la arista
        margen_sup = rango * 0.4
        margen_inf = rango * 0.4
        
        # Hundimos el límite inferior para crear una "zona de texto" bajo las barras
        axes[i].set_ylim(min_val - margen_inf, max_val + margen_sup)
        
        # Trazamos una línea negra fuerte en el 0 para separar las barras de la zona de texto
        axes[i].axhline(y=min_val if min_val > 0 else 0, color='black', linewidth=1.2)

        # --- AÑADIR TEXTOS MANUALMENTE ---
        for container, estado in zip(axes[i].containers, df_pivot.columns):
            for bar, dataset in zip(container, df_pivot.index):
                val = bar.get_height()
                x_pos = bar.get_x() + bar.get_width() / 2
                
                # Verificamos si realmente existía esta prueba en el CSV
                fila_existe = (str(dataset), estado) in dict_aristas
                
                # Si el valor es nulo, o es 0 falso generado por Pandas en un espacio vacío, nos lo saltamos
                if np.isnan(val) or (val == 0.0 and not fila_existe):
                    continue
                
                # 1. Poner el número exacto arriba de la barra
                axes[i].text(x_pos, val + (rango * 0.02), f'{val:.9f}',
                             ha='center', va='bottom', rotation=90, fontsize=9)
                
                # 2. Poner la etiqueta de la arista cortada DEBAJO de las barras
                if estado != 'Original':
                    arista_nombre = dict_aristas.get((str(dataset), estado), "")
                    if arista_nombre and arista_nombre != "Ninguna":
                        axes[i].text(x_pos, min_val - (rango * 0.02), arista_nombre,
                                     ha='center', va='top', rotation=90, fontsize=8,
                                     color='#2c3e50', fontweight='bold')

        # --- FORMATO ESTÉTICO ---
        axes[i].set_title(f'{titulo}', fontsize=14, fontweight='bold')
        axes[i].set_xlabel('Año (Dataset)', fontsize=12)
        
        if "Shortest Path" in titulo or "Diameter" in titulo:
            axes[i].set_ylabel('Distancia Acumulada', fontsize=12)
        else:
            axes[i].set_ylabel('Puntaje Normalizado', fontsize=12)

        axes[i].tick_params(axis='x', rotation=0)
        axes[i].grid(axis='y', linestyle='--', alpha=0.5)

    # --- LEYENDA GLOBAL ---
    axes[7].axis('off')
    handles, labels = axes[0].get_legend_handles_labels()
    axes[7].legend(handles, labels, title='Escenario de Perturbación', 
                   loc='center', fontsize=14, title_fontsize=16, edgecolor='black', shadow=True)

    plt.tight_layout()
    
    nombre_archivo = os.path.join('graficos', 'panel_perturbacion_numeros.png')
    plt.savefig(nombre_archivo, dpi=300, bbox_inches='tight')
    print(f"\n-> ¡Listo! El panel final arreglado fue guardado como '{nombre_archivo}'")

except Exception as e:
    print(f"Ocurrió un error al procesar los datos: {e}")