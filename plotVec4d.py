import matplotlib.pyplot as plt
from adjustText import adjust_text
import os

# Caminho para o novo arquivo com 4 dimensões + rótulo
filename = "vecs_dim4_S_toV_toT.dat"

# Listas para armazenar dados
x = []
y = []
colors = []
sizes = []
labels = []
wtypes = []

# Leitura do arquivo
with open(filename, 'r') as f:
    for line in f:
        parts = line.strip().split()
        if len(parts) == 6:
            xi, yi, ci, si, label, wtype = parts
            x.append(float(ci))
            y.append(float(si))
            colors.append(float(xi))           # Pode representar magnitude ou outra info
            sizes.append(float(yi) * 20)       # Escala de tamanho
            labels.append(label)
            wtypes.append(wtype)

# Plot
plt.figure(figsize=(10, 6))
sc = plt.scatter(x, y, c=colors, s=sizes, cmap='viridis', alpha=0.7)

# Adiciona os rótulos com ajuste automático
texts = []
for i in range(len(labels)):
    texts.append(plt.text(x[i], y[i], labels[i], fontsize=8))

adjust_text(texts, x=x, y=y, arrowprops=dict(arrowstyle='-', color='gray', lw=0.5))

# Barra de cor
cbar = plt.colorbar(sc)
cbar.set_label('Valor codificado por cor')

plt.title("Projeção de Vetores de Palavras em 4D (2D + Cor + Tamanho)")
plt.xlabel("Dimensão 1")
plt.ylabel("Dimensão 2")
plt.grid(True)
plt.tight_layout()

# Nome do arquivo de saída com extensão .png
output_filename = os.path.splitext(filename)[0] + ".png"
plt.savefig(output_filename, dpi=300)

plt.show()

plt.close()