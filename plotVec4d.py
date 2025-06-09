import matplotlib.pyplot as plt

# Caminho para o novo arquivo com 4 dimensões + rótulo
filename = "vecs_dim4_S_V_T_seq.dat"

# Listas para armazenar dados
x = []
y = []
colors = []
sizes = []
labels = []

# Leitura do arquivo
with open(filename, 'r') as f:
    for line in f:
        parts = line.strip().split()
        if len(parts) == 5:
            xi, yi, ci, si, label = parts
            x.append(float(xi))
            y.append(float(yi))
            colors.append(float(ci))   # Ex: magnitude ou qualquer valor
            sizes.append(float(si) * 20)  # Ajuste do tamanho dos pontos
            labels.append(label)

# Plot
plt.figure(figsize=(10, 6))
sc = plt.scatter(x, y, c=colors, s=sizes, cmap='viridis', alpha=0.7)

# Adiciona os rótulos
for i in range(len(labels)):
    plt.text(x[i], y[i], labels[i], fontsize=8, ha='right', va='bottom')

# Barra de cor
cbar = plt.colorbar(sc)
cbar.set_label('Valor codificado por cor')

plt.title("Projeção de Vetores de Palavras em 4D (2D + Cor + Tamanho)")
plt.xlabel("Dimensão 1")
plt.ylabel("Dimensão 2")
plt.grid(True)
plt.tight_layout()
plt.show()
