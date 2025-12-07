import pandas as pd
import matplotlib.pyplot as plt
import numpy as np

df = pd.read_csv('data/times.csv')

summary = df.groupby('thread_id')['time_ms'].agg([
    ('count', 'count'),
    ('mean', 'mean'),
    ('std', 'std'),
    ('min', 'min'),
    ('max', 'max')
])

summary['jitter'] = summary['max'] - summary['min']

print("Resumo estatístico dos tempos por thread (ms):")
print(summary)

summary.to_csv('data/tempo_estatisticas.csv')

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