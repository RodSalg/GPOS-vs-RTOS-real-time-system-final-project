import pandas as pd
import matplotlib.pyplot as plt
import numpy as np

# Carregar dados de tempos
df = pd.read_csv('data/times.csv')

# Analisar estatísticas por thread
summary = df.groupby('thread_id')['time_ms'].agg([
    ('count', 'count'),
    ('mean', 'mean'),
    ('std', 'std'),
    ('min', 'min'),
    ('max', 'max')
])

summary['jitter'] = summary['max'] - summary['min']

# Mostrar
print("Resumo estatístico dos tempos por thread (ms):")
print(summary)

# Salvar como CSV
summary.to_csv('data/tempo_estatisticas.csv')

# Plotar histograma por thread
for tid in df['thread_id'].unique():
    subset = df[df['thread_id'] == tid]['time_ms']
    plt.figure()
    plt.hist(subset, bins=30, color='skyblue', edgecolor='black')
    plt.title(f'Thread {tid} - Distribuição do Tempo de Execução')
    plt.xlabel('Tempo (ms)')
    plt.ylabel('Frequência')
    plt.grid(True)
    plt.tight_layout()
    plt.savefig(f'data/thread_{tid}_hist.png')
    plt.close()


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