import matplotlib.pyplot as plt
import pandas as pd

# Carregar dados
csv_path = 'data/output.csv'
data = pd.read_csv(csv_path)

# Plotar posição e referência
plt.figure(figsize=(10, 5))
plt.plot(data['t'], data['y1'], label='y1 (posição x)')
plt.plot(data['t'], data['y2'], label='y2 (posição y)')
plt.plot(data['t'], data['x1'], label='x1 (centro x)', linestyle='dashed')
plt.plot(data['t'], data['x2'], label='x2 (centro y)', linestyle='dashed')
plt.xlabel('Tempo (s)')
plt.ylabel('Posição (m)')
plt.title('Posição do Robô vs Tempo')
plt.legend()
plt.grid(True)
plt.tight_layout()
plt.savefig('data/trajectoria.png')
plt.show()