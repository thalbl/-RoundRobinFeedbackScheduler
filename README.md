# Simulador de Escalonamento de Processos - Round Robin com Feedback

## Visão Geral
Este projeto implementa um simulador de escalonamento de processos baseado no algoritmo Round Robin com Feedback. O simulador gerencia processos em múltiplas filas de prioridade, com diferentes tempos de quantum, e inclui suporte para operações de I/O (entrada/saída) com três tipos de dispositivos: disco, fita magnética e impressora. O simulador é desenvolvido inteiramente em linguagem C, conforme os requisitos especificados.

## Funcionalidades Principais

### 1. Gerenciamento de Processos
- **Criação de processos** com PID único e informações de PCB (Process Control Block)
- Estados do processo: 
  - `READY_Q0` (alta prioridade)
  - `READY_Q1` (baixa prioridade)
  - `RUNNING` (em execução)
  - `IO` (em operação de entrada/saída)
  - `TERMINATED` (finalizado)
- Tempos de serviço (burst time) aleatórios para cada processo (entre 5 e 30 unidades de tempo)

### 2. Sistema de Filas Multi-Nível
- **Fila de Alta Prioridade (Q0)**: 
  - Tempo quantum: 2 unidades
  - Processos novos entram nesta fila
- **Fila de Baixa Prioridade (Q1)**:
  - Tempo quantum: 4 unidades
  - Processos preemptados da Q0 são movidos para esta fila
- **Fila de I/O**:
  - Gerencia processos em operações de entrada/saída
  - Diferentes tempos para cada dispositivo:
    - Disco: 5 unidades de tempo
    - Fita magnética: 3 unidades de tempo
    - Impressora: 4 unidades de tempo

### 3. Política de Escalonamento
- Prioridade para processos na fila Q0
- Processos que sofrem preempção são movidos para Q1
- Processos que retornam de I/O:
  - Após operação de disco → fila Q1 (baixa prioridade)
  - Após operação de fita magnética ou impressora → fila Q0 (alta prioridade)
- Algoritmo de seleção: Sempre executa processos de Q0 antes de Q1

### 4. Operações de Entrada/Saída (I/O)
- Processos podem solicitar operações de I/O durante a execução (probabilidade de 10% por unidade de tempo)
- Três tipos de dispositivos:
  - **Disco**: Retorna para fila de baixa prioridade (Q1)
  - **Fita Magnética**: Retorna para fila de alta prioridade (Q0)
  - **Impressora**: Retorna para fila de alta prioridade (Q0)
- Processos em I/O ficam bloqueados pelo tempo correspondente ao dispositivo

### 5. Simulação e Log de Eventos
- Execução em tempo discreto (unidades de tempo)
- Log detalhado de eventos:
  - Alocação da CPU a processos
  - Solicitações e conclusões de I/O
  - Preempção de processos
  - Término de processos
  - Tempos ociosos da CPU

### 6. Estatísticas de Desempenho
- Cálculo de métricas de desempenho:
  - Tempo de espera (waiting time) por processo
  - Turnaround time (tempo total do processo)
  - Médias de tempo de espera e turnaround
  - Porcentagem de utilização da CPU

## Premissas do Simulador

1. **Limite máximo de processos**: 100
2. **Tempo de quantum**:
   - Fila Q0 (alta prioridade): 2 unidades de tempo
   - Fila Q1 (baixa prioridade): 4 unidades de tempo
3. **Tempos de serviço e I/O aleatórios**:
   - Tempo de serviço (burst time): 5-30 unidades (aleatório)
   - Probabilidade de I/O: 10% por unidade de tempo de execução
4. **Duração de operações de I/O**:
   - Disco: 5 unidades de tempo
   - Fita magnética: 3 unidades de tempo
   - Impressora: 4 unidades de tempo
5. **Entrada nas filas de prontos**:
   - Processos novos: fila de alta prioridade (Q0)
   - Processos que retornam de I/O: 
     - Disco → fila de baixa prioridade (Q1)
     - Fita magnética/Impressora → fila de alta prioridade (Q0)
   - Processos preemptados: fila de baixa prioridade (Q1)

## Estrutura do Código

### Estruturas de Dados Principais
- **`PCB` (Process Control Block)**:
  - Armazena todas as informações do processo (PID, estado, tempos, etc.)
- **`Queue`**:
  - Implementação de fila circular para gerenciar as filas de processos
- **Processos**:
  - Array de estruturas PCB para gerenciar todos os processos

### Funções Principais
1. **`initialize_processes()`**:
   - Cria processos iniciais com valores aleatórios
   - Inicializa todas as estruturas de dados

2. **`update_io_queue()`**:
   - Gerencia a fila de I/O e atualiza os tempos restantes
   - Move processos de volta às filas de prontos ao completar I/O

3. **`schedule()`**:
   - Seleciona o próximo processo para execução
   - Implementa a política de prioridade entre filas

4. **`run_cpu()`**:
   - Executa a CPU por uma unidade de tempo
   - Gerencia eventos: término, I/O, preempção

5. **`simulate()`**:
   - Loop principal da simulação
   - Orquestra todas as operações por unidade de tempo

6. **`print_statistics()`**:
   - Calcula e exibe estatísticas finais de desempenho

## Compilação e Execução

### Requisitos
- Compilador C (gcc recomendado)
- Sistema operacional Linux/Unix (ou ambiente compatível)

### Compilação
```bash
gcc -o scheduler scheduler.c
```

### Execução
```bash
./scheduler
```

### Parâmetros de Configuração (no código)
- `MAX_PROCESSES`: Número máximo de processos (padrão: 100)
- `QUANTUM_Q0`: Tempo quantum para fila de alta prioridade (padrão: 2)
- `QUANTUM_Q1`: Tempo quantum para fila de baixa prioridade (padrão: 4)
- `IO_PROBABILITY`: Probabilidade de solicitação de I/O (padrão: 0.1)
- `MAX_TIME`: Tempo máximo de simulação (padrão: 1000 unidades)

## Saída do Simulador

### Durante a Execução
Log detalhado de eventos:
```
Time 0: Process 1 requests I/O (type: DISK)
Time 1: CPU idle
Time 2: Running process 2 (Q0, quantum: 2)
...
Time 25: Process 3 terminated. Waiting: 8, Turnaround: 20
```

### Ao Final da Simulação
Estatísticas completas:
```
Process Statistics:
PID     Burst   Waiting Turnaround
1       12      5       17
2       8       3       11
3       15      8       23
4       10      6       16
5       7       4       11

Average Waiting Time: 5.20
Average Turnaround Time: 15.60
CPU Utilization: 92.50%
```

## Exemplo de Execução

```bash
# Compilar
gcc -o scheduler scheduler.c

# Executar
./scheduler

# Saída esperada (exemplo)
Time 0: Process 1 started (Burst: 12)
Time 0: Running process 1 (Q0, quantum: 2)
Time 2: Process 1 preempted to Q1
Time 2: Running process 2 (Q0, quantum: 2)
...
Time 35: Simulation completed

Process Statistics:
PID     Burst   Waiting Turnaround
1       12      7       19
2       9       5       14
3       14      9       23
4       8       4       12
5       11      6       17

Average Waiting Time: 6.20
Average Turnaround Time: 17.00
CPU Utilization: 88.57%
```

## Considerações Finais

Este simulador implementa fielmente o algoritmo Round Robin com Feedback, atendendo a todos os requisitos especificados. A estrutura modular do código permite fácil expansão para:

- Adicionar mais filas de prioridade
- Implementar outras políticas de escalonamento
- Incluir mais tipos de dispositivos de I/O
- Adicionar gráficos de Gantt para visualização do escalonamento

O simulador serve como uma ferramenta educacional valiosa para compreender os mecanismos de escalonamento de processos em sistemas operacionais e os efeitos das políticas de prioridade no desempenho do sistema.