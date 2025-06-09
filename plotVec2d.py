import matplotlib.pyplot as plt
from adjustText import adjust_text
from matplotlib.lines import Line2D
import os

# Caminho para o arquivo com os vetores
filename = "vecs_dim2_S_toV_toT.dat"

# Listas para armazenar dados
x = []
y = []
labels = []
wtypes = []

# Leitura do arquivo
with open(filename, 'r') as f:
    for line in f:
        parts = line.strip().split()
        if len(parts) == 4:
            xi, yi, label, wtype = parts
            x.append(float(xi))
            y.append(float(yi))
            labels.append(label)
            wtypes.append(wtype)

# Define cores: azul para 'V', laranja para 'S/T'
colors = ['blue' if wt == 'V' else 'orange' for wt in wtypes]

# Plot
plt.figure(figsize=(10, 6))
plt.scatter(x, y, c=colors, alpha=0.7)

# Adiciona os rótulos com ajuste automático
texts = []
for i in range(len(labels)):
    texts.append(plt.text(x[i], y[i], labels[i], fontsize=9))

adjust_text(texts, x=x, y=y, arrowprops=dict(arrowstyle='-', color='gray', lw=0.5))

# Legenda
legend_elements = [
    Line2D([0], [0], marker='o', color='w', label='Verbo (V)', markerfacecolor='blue', markersize=8),
    Line2D([0], [0], marker='o', color='w', label='Substantivos/Objetos (S/T)', markerfacecolor='orange', markersize=8)
]
plt.legend(handles=legend_elements)

plt.title("Projeção de Vetores de Palavras")
plt.xlabel("Dimensão 1")
plt.ylabel("Dimensão 2")
plt.grid(True)
plt.tight_layout()

# Nome do arquivo de saída com extensão .png
output_filename = os.path.splitext(filename)[0] + ".png"
plt.savefig(output_filename, dpi=300)

plt.show()

plt.close()
