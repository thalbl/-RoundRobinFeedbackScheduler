# Simulador de Escalonamento Round Robin com Feedback para Windows

## Visão Geral
Este projeto implementa um simulador de escalonamento de processos baseado no algoritmo Round Robin com Feedback, desenvolvido inteiramente em linguagem C e otimizado para execução em sistemas Windows. O simulador gerencia processos em múltiplas filas de prioridade com diferentes tempos de quantum e inclui suporte para operações de I/O com três tipos de dispositivos.

## Funcionalidades Principais

### 🚀 Gerenciamento de Processos no Windows
- Criação de processos com PID único e PCB (Process Control Block)
- Estados de processo: Pronto Q0, Pronto Q1, Executando, EM_IO, Terminado
- Tempos de serviço aleatórios (5-30 unidades)
- Sistema de prioridades com múltiplas filas

### ⚙️ Sistema Multi-Filas
- **Fila Q0 (Alta Prioridade)**: Quantum de 2 unidades
- **Fila Q1 (Baixa Prioridade)**: Quantum de 4 unidades
- **Fila de I/O**: Gerencia operações de entrada/saída
- Políticas de retorno diferenciadas por tipo de I/O

### 🔄 Operações de I/O
- Solicitação aleatória de I/O durante execução (10% de probabilidade)
- Dispositivos suportados:
  - **Disco**: 5 unidades → retorna para Q1
  - **Fita Magnética**: 3 unidades → retorna para Q0
  - **Impressora**: 4 unidades → retorna para Q0

### 📊 Análise de Desempenho
- Log detalhado de eventos em tempo real
- Cálculo de métricas de desempenho:
  - Tempo de espera por processo
  - Turnaround time
  - Utilização da CPU
  - Tempos médios de espera e turnaround
- Saída formatada para prompt de comando

## Pré-requisitos para Windows
- [MinGW-w64](https://sourceforge.net/projects/mingw-w64/) (compilador GCC para Windows)
- [Git para Windows](https://git-scm.com/download/win) (opcional, mas recomendado)
- Terminal (CMD, PowerShell ou Git Bash)

## Configuração do Ambiente Windows

### 1. Instalar o MinGW-w64
1. Baixe o instalador do [MinGW-w64](https://sourceforge.net/projects/mingw-w64/)
2. Durante a instalação, selecione:
   - Architecture: `x86_64`
   - Threads: `posix`
   - Exception: `seh`
3. Adicione o MinGW ao PATH do sistema:
   - Pressione `Win + R`, digite `sysdm.cpl` e clique em OK
   - Abra "Variáveis de Ambiente"
   - Em "Variáveis do sistema", edite `Path`
   - Adicione o caminho da pasta `bin` do MinGW (ex: `C:\mingw64\bin`)

### 2. Clonar o repositório (com Git)
```powershell
git clone https://github.com/seu-usuario/RoundRobinFeedbackScheduler.git
cd RoundRobinFeedbackScheduler
```

### 3. Compilar manualmente (sem Makefile)
```powershell
gcc -o bin/scheduler src/scheduler.c
```

### 4. Executar o simulador
```powershell
bin\scheduler.exe
```

## Script de Compilação Automática (Makefile para Windows)
1. Instale o [Make para Windows](https://sourceforge.net/projects/gnuwin32/files/make/3.81/)
2. Adicione o caminho do Make ao PATH (ex: `C:\Program Files (x86)\GnuWin32\bin`)
3. Execute:

```powershell
make        # Compila o projeto
make run    # Executa o simulador
make clean  # Limpa os arquivos compilados
```

## Exemplo de Saída no Prompt de Comando
```
======== INICIO DA SIMULACAO ========
Configuracao:
- Processos: 5
- Quantum Q0 (alta prioridade): 2
- Quantum Q1 (baixa prioridade): 4
- Probabilidade de I/O: 10%
====================================

Tempo    0: Processo  1 iniciou execucao (Q0, quantum: 2)
Tempo    2: Processo  1 preemptado -> Fila Q1
Tempo    2: Processo  2 iniciou execucao (Q0, quantum: 2)
Tempo    4: Processo  2 solicitou I/O (DISCO)
Tempo    4: Processo  3 iniciou execucao (Q0, quantum: 2)
...
Tempo   25: Processo  3 terminou. Espera:  8, Turnaround: 20

======== ESTATISTICAS DA SIMULACAO ========
PID  Execucao  Espera  Turnaround
----------------------------------
 1      12        5        17
 2       8        3        11
 3      15        8        23
 4      10        6        16
 5       7        4        11

Metricas Gerais:
- Tempo total de simulacao: 35 unidades
- Processos concluidos: 5/5
- Tempo medio de espera: 5.20
- Tempo medio de turnaround: 15.60
- Utilizacao da CPU: 92.50%
==========================================
```

## Parâmetros de Configuração
Ajuste no arquivo `src/scheduler.c`:

```c
#define MAX_PROCESSOS 100       // Número máximo de processos
#define QUANTUM_Q0 2            // Tempo quantum para fila de alta prioridade
#define QUANTUM_Q1 4            // Tempo quantum para fila de baixa prioridade
#define PROBABILIDADE_IO 0.1    // Probabilidade de I/O (10%)
#define TEMPO_MAXIMO 1000       // Tempo máximo de simulação

// Tempos de I/O
#define DISCO_IO 5
#define FITA_IO 3
#define IMPRESSORA_IO 4
```

## Solução de Problemas Comuns no Windows

### Erro "gcc não é reconhecido"
- Verifique a instalação do MinGW-w64
- Confira se o caminho do `bin` do MinGW está no PATH do sistema
- Reinicie o terminal após alterações no PATH

### Erro ao executar o executável
```powershell
.\bin\scheduler.exe
```
Se o erro persistir, compile manualmente:
```powershell
gcc src/scheduler.c -o scheduler
.\scheduler.exe
```

### Problemas com acentuação
O projeto usa configuração de locale para suportar caracteres especiais:
```c
setlocale(LC_ALL, "Portuguese");
```
Se persistirem problemas, remova os acentos do código.

## Premissas do Simulador
1. **Limite de processos**: Máximo de 100 processos
2. **Tempos de quantum**:
   - Fila Q0: 2 unidades
   - Fila Q1: 4 unidades
3. **Geração aleatória**:
   - Tempo de serviço: 5-30 unidades
   - Probabilidade de I/O: 10% por unidade de tempo
4. **Duração de I/O**:
   - Disco: 5 unidades
   - Fita magnética: 3 unidades
   - Impressora: 4 unidades
5. **Entrada nas filas**:
   - Novos processos: Q0
   - Retorno de I/O: Disco → Q1, Fita/Impressora → Q0
   - Processos preemptados: Q1
