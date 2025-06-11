import matplotlib.pyplot as plt
from adjustText import adjust_text
from matplotlib.lines import Line2D
from sklearn.decomposition import PCA
import os

# Caminho para o arquivo com os vetores 8D
filename = "vecs_dim8_S_toVto_T.dat"

# Listas para armazenar os dados
vectors = []
labels = []
wtypes = []

# Leitura do arquivo
with open(filename, 'r') as f:
    for line in f:
        parts = line.strip().split()
        if len(parts) == 10:  # 8 coordenadas + label + tipo
            *coords, label, wtype = parts
            vectors.append([float(c) for c in coords])
            labels.append(label)
            wtypes.append(wtype)

# Aplica PCA para reduzir de 8D para 2D
pca = PCA(n_components=2)
reduced = pca.fit_transform(vectors)
x = reduced[:, 0]
y = reduced[:, 1]

# Define cores
colors = ['blue' if wt == 'V' else 'orange' for wt in wtypes]

# Plot
plt.figure(figsize=(10, 6))
plt.scatter(x, y, c=colors, alpha=0.7)

# Adiciona rótulos com ajuste automático
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

# Títulos e layout
plt.title("Projeção PCA (8D → 2D) de Vetores de Palavras")
plt.xlabel("Componente Principal 1")
plt.ylabel("Componente Principal 2")
plt.grid(True)
plt.tight_layout()

# Salvar imagem
output_filename = os.path.splitext(filename)[0] + "_pca.png"
plt.savefig(output_filename, dpi=300)

plt.show()
plt.close()
