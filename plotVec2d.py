import matplotlib.pyplot as plt

# Caminho para o arquivo com os vetores
filename = "vecs_dim2_S_V_T_bow.dat"

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

# Adiciona os rótulos
for i in range(len(labels)):
    plt.text(x[i], y[i], labels[i], fontsize=9, ha='right', va='bottom')

plt.title("Projeção de Vetores de Palavras")
plt.xlabel("Dimensão 1")
plt.ylabel("Dimensão 2")
plt.grid(True)
plt.tight_layout()
plt.show()
