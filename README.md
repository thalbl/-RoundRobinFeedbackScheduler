# Simulador de Escalonamento Round Robin com Feedback

## Visão Geral
Este projeto implementa um simulador de escalonamento de processos baseado no algoritmo Round Robin com Feedback, desenvolvido em linguagem C. Oferecemos **duas versões complementares** do simulador para diferentes necessidades:

1. **Versão com entrada aleatória**: Gera processos automaticamente com tempos de execução aleatórios
2. **Versão com entrada por arquivo**: Permite carregar configurações personalizadas de processos via arquivo texto

Ambas as versões compartilham o mesmo núcleo de escalonamento e oferecem análise detalhada do desempenho do sistema.

## Características Comuns

### ⚙️ Mecanismo de Escalonamento
- **Filas Multi-Nível**:
  - Q0 (Alta Prioridade): Quantum de 2 unidades
  - Q1 (Baixa Prioridade): Quantum de 4 unidades
- **Políticas de Feedback**:
  - Processos preemptados → Q1
  - Retorno de I/O:
    - Disco → Q1
    - Fita magnética/Impressora → Q0
- Processos novos sempre entram em Q0

### 🔄 Sistema de Entrada/Saída (I/O)
- Solicitação aleatória durante execução (10% de probabilidade)
- Três tipos de dispositivos:
  - **Disco**: 5 unidades de tempo
  - **Fita Magnética**: 3 unidades de tempo
  - **Impressora**: 4 unidades de tempo

### 📊 Análise de Desempenho
- Log detalhado de eventos em tempo real
- Estatísticas completas ao final:
  - Tempo de espera e turnaround por processo
  - Médias gerais de desempenho
  - Utilização da CPU
  - Tempo total de simulação

## Versão 1: Entrada Aleatória

### Características
- Gera automaticamente processos com:
  - Tempo de execução aleatório (5-30 unidades)
  - Todos os processos chegam no tempo 0
- Ideal para testes rápidos e análise de comportamento geral

### Como Usar
```bash
# Compilar
gcc -o scheduler_rand scheduler_rand.c

# Executar (5 processos por padrão)
./scheduler_rand
```

### Exemplo de Saída
```
Tempo    0: Processo  1 iniciou execucao (Q0, quantum: 2)
Tempo    2: Processo  1 preemptado -> Fila Q1
Tempo    2: Processo  2 iniciou execucao (Q0, quantum: 2)
...
Tempo   25: Processo  3 terminou. Espera:  8, Turnaround: 20

======== ESTATISTICAS DA SIMULACAO ========
PID  Execucao  Espera  Turnaround
----------------------------------
 1      12        5        17
 2       8        3        11
 3      15        8        23
...
```

## Versão 2: Entrada por Arquivo

### Características
- Permite definir processos personalizados via arquivo texto
- Especifica tempo de chegada e tempo de execução para cada processo
- Ideal para testar cenários específicos e reproduzíveis

### Formato do Arquivo
Crie um arquivo de texto com o formato:
```
[tempo_chegada] [tempo_execucao]
[tempo_chegada] [tempo_execucao]
...
```

Exemplo (`processos.txt`):
```
0 10
1 5
2 8
3 12
4 7
```

### Como Usar
```bash
# Compilar
gcc -o scheduler_file scheduler_file.c

# Executar com arquivo personalizado
./scheduler_file processos.txt

# Ajuda
./scheduler_file --help
```

### Exemplo de Saída
```
Tempo    0: Processo  1 chegou (burst: 10) -> Fila Q0
Tempo    0: Processo  1 iniciou execucao (Q0, quantum: 2)
Tempo    0: Processo  1 solicitou I/O (IMPRESSORA)
Tempo    1: Processo  2 chegou (burst: 5) -> Fila Q0
...
Tempo   30: Processo  5 terminou. Espera:  5, Turnaround: 12

======== ESTATISTICAS DA SIMULACAO ========
PID  Chegada  Execucao  Espera  Turnaround
------------------------------------------
 1       0        10        5        15
 2       1         5        3         8
 3       2         8        6        14
...
```

## Comportamento Esperado e Validação

### Cenários Normais
1. **Processos solicitando I/O imediatamente**:
   ```bash
   Tempo 0: Processo 1 iniciou execução
   Tempo 0: Processo 1 solicitou I/O
   ```
   - **Por que ocorre**: Probabilidade de 10% por unidade de tempo
   - **É normal**: Reflete situações reais onde processos tem operações de I/O intensivas

2. **Múltiplos eventos simultâneos**:
   ```bash
   Tempo 4: Processo 5 chegou -> Fila Q0
   Tempo 4: Processo 1 completou I/O -> Fila Q0
   Tempo 4: Processo 2 completou I/O -> Fila Q0
   Tempo 4: Processo 4 iniciou execução
   ```
   - **Ordem de processamento**: 
     1. Chegada de novos processos
     2. Conclusão de operações de I/O
     3. Escalonamento de processos prontos

3. **Preempção por quantum**:
   ```bash
   Tempo 5: Processo 4 preemptado -> Fila Q1
   ```
   - Ocorre quando um processo completa seu tempo de quantum sem terminar

### Vantagens das Duas Versões
| Característica               | Versão Aleatória         | Versão com Arquivo       |
|------------------------------|--------------------------|--------------------------|
| Configuração inicial         | Automática               | Personalizada           |
| Tempos de chegada            | Todos tempo 0            | Definidos por linha     |
| Tempos de execução           | Aleatórios (5-30)        | Especificados           |
| Casos de uso                 | Testes rápidos           | Cenários específicos    |
| Reprodutibilidade            | Baixa (randômica)        | Alta (determinística)   |
| Análise de casos extremos    | Limitada                 | Completa                |

## Estrutura do Projeto
```
RoundRobinFeedbackScheduler/
├── src/
│   ├── processos.txt
│   ├── scheduler_rand.c     # Versão com processos aleatórios
│   └── scheduler_file.c     # Versão com entrada por arquivo
├── Makefile                 # Script de compilação
└── README.md                # Este documento
```

## Análise de Casos Especiais

### Caso 1: Processo solicita I/O imediatamente
**Arquivo de entrada**:
```
0 10
```

**Comportamento esperado**:
```
Tempo 0: Processo 1 chegou -> Fila Q0
Tempo 0: Processo 1 iniciou execução (Q0, quantum: 2)
Tempo 0: Processo 1 solicitou I/O (DISCO)  # Caso ocorra a probabilidade
Tempo 5: Processo 1 completou I/O (DISCO) -> Fila Q1
...
```

### Caso 2: Múltiplos processos chegando simultaneamente
**Arquivo de entrada**:
```
0 5
0 8
0 6
```

**Comportamento esperado**:
```
Tempo 0: Processo 1 chegou -> Fila Q0
Tempo 0: Processo 2 chegou -> Fila Q0
Tempo 0: Processo 3 chegou -> Fila Q0
Tempo 0: Processo 1 iniciou execução (Q0, quantum: 2)
...
```