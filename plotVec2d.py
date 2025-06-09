import matplotlib.pyplot as plt
from adjustText import adjust_text

# Caminho para o arquivo com os vetores
filename = "vecs_dim2_Sto_V_toT.dat"

# Listas para armazenar dados
x = []
y = []
labels = []

# Leitura do arquivo
with open(filename, 'r') as f:
    for line in f:
        parts = line.strip().split()
        if len(parts) == 3:
            xi, yi, label = parts
            x.append(float(xi))
            y.append(float(yi))
            labels.append(label)

# Plot
plt.figure(figsize=(10, 6))
plt.scatter(x, y, color='blue')

# Adiciona os rótulos com ajuste automático
texts = []
for i in range(len(labels)):
    texts.append(plt.text(x[i], y[i], labels[i], fontsize=9))

adjust_text(texts, x=x, y=y, arrowprops=dict(arrowstyle='-', color='gray', lw=0.5))

plt.title("Projeção de Vetores de Palavras")
plt.xlabel("Dimensão 1")
plt.ylabel("Dimensão 2")
plt.grid(True)
plt.tight_layout()
plt.show()
