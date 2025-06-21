#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <stdbool.h>
#include <locale.h>

#define MAX_PROCESSOS 100
#define TAMANHO_FILA 100
#define QUANTUM_Q0 2
#define QUANTUM_Q1 4
#define PROBABILIDADE_IO 0.1
#define TEMPO_MAXIMO 1000

// Estados do processo
typedef enum {
    PRONTO_Q0,
    PRONTO_Q1,
    EXECUTANDO,
    EM_IO,
    TERMINADO
} EstadoProcesso;

// Tipos de dispositivos de I/O
typedef enum {
    DISCO,
    FITA,
    IMPRESSORA
} TipoIO;

// Bloco de Controle de Processo (PCB)
typedef struct {
    int pid;
    int ppid;
    EstadoProcesso estado;
    int tempo_chegada;
    int tempo_execucao;
    int tempo_restante;
    int tempo_espera;
    int inicio_espera;
    TipoIO tipo_io;
    int tempo_io_restante;
} PCB;

// Estrutura de fila
typedef struct {
    int itens[TAMANHO_FILA];
    int frente;
    int tras;
    int tamanho;
} Fila;

// Variáveis globais
PCB processos[MAX_PROCESSOS];
Fila fila0;   // Fila de alta prioridade
Fila fila1;   // Fila de baixa prioridade
Fila fila_io; // Fila de I/O
int processo_atual = -1;
int quantum_atual = 0;
int tempo_relogio = 0;
int processos_terminados = 0;
int total_processos = 0;
int cpu_ociosa = 0;  // Contador de tempo ocioso da CPU

// Funções para manipulação de filas
void inicializar_fila(Fila *f) {
    f->frente = -1;
    f->tras = -1;
    f->tamanho = 0;
}

bool fila_vazia(Fila *f) {
    return f->tamanho == 0;
}

void enfileirar(Fila *f, int item) {
    if (f->tamanho == TAMANHO_FILA) {
        printf("Fila cheia!\n");
        return;
    }
    if (f->frente == -1) {
        f->frente = 0;
    }
    f->tras = (f->tras + 1) % TAMANHO_FILA;
    f->itens[f->tras] = item;
    f->tamanho++;
}

int desenfileirar(Fila *f) {
    if (fila_vazia(f)) {
        return -1;
    }
    int item = f->itens[f->frente];
    if (f->frente == f->tras) {
        f->frente = f->tras = -1;
    } else {
        f->frente = (f->frente + 1) % TAMANHO_FILA;
    }
    f->tamanho--;
    return item;
}

// Inicialização dos processos
void inicializar_processos(int n) {
    srand(time(NULL));
    total_processos = n;
    for (int i = 0; i < n; i++) {
        processos[i].pid = i + 1;
        processos[i].ppid = 0;
        processos[i].estado = PRONTO_Q0;
        processos[i].tempo_chegada = 0;
        processos[i].tempo_execucao = (rand() % 26) + 5; // 5-30 unidades
        processos[i].tempo_restante = processos[i].tempo_execucao;
        processos[i].tempo_espera = 0;
        processos[i].inicio_espera = 0;
        enfileirar(&fila0, i);
    }
}

// Atualização da fila de I/O
void atualizar_fila_io() {
    int i = 0;
    int count = fila_io.tamanho;
    
    while (count > 0) {
        int idx_proc = desenfileirar(&fila_io);
        processos[idx_proc].tempo_io_restante--;

        if (processos[idx_proc].tempo_io_restante > 0) {
            enfileirar(&fila_io, idx_proc);
        } else {
            if (processos[idx_proc].tipo_io == DISCO) {
                processos[idx_proc].estado = PRONTO_Q1;
                processos[idx_proc].inicio_espera = tempo_relogio;
                enfileirar(&fila1, idx_proc);
                printf("Tempo %4d: Processo %2d completou I/O (DISCO) -> Fila Q1\n", 
                       tempo_relogio, processos[idx_proc].pid);
            } else {
                processos[idx_proc].estado = PRONTO_Q0;
                processos[idx_proc].inicio_espera = tempo_relogio;
                enfileirar(&fila0, idx_proc);
                const char *tipo = (processos[idx_proc].tipo_io == FITA) ? "FITA" : "IMPRESSORA";
                printf("Tempo %4d: Processo %2d completou I/O (%s) -> Fila Q0\n", 
                       tempo_relogio, processos[idx_proc].pid, tipo);
            }
        }
        count--;
    }
}

// Escalonamento de processo
void escalonar() {
    if (!fila_vazia(&fila0)) {
        processo_atual = desenfileirar(&fila0);
        processos[processo_atual].estado = EXECUTANDO;
        quantum_atual = (QUANTUM_Q0 < processos[processo_atual].tempo_restante) ? 
                        QUANTUM_Q0 : processos[processo_atual].tempo_restante;
        processos[processo_atual].tempo_espera += (tempo_relogio - processos[processo_atual].inicio_espera);
        printf("Tempo %4d: Processo %2d iniciou execucao (Q0, quantum: %d)\n",
               tempo_relogio, processos[processo_atual].pid, quantum_atual);
    } else if (!fila_vazia(&fila1)) {
        processo_atual = desenfileirar(&fila1);
        processos[processo_atual].estado = EXECUTANDO;
        quantum_atual = (QUANTUM_Q1 < processos[processo_atual].tempo_restante) ? 
                        QUANTUM_Q1 : processos[processo_atual].tempo_restante;
        processos[processo_atual].tempo_espera += (tempo_relogio - processos[processo_atual].inicio_espera);
        printf("Tempo %4d: Processo %2d iniciou execucao (Q1, quantum: %d)\n",
               tempo_relogio, processos[processo_atual].pid, quantum_atual);
    }
}

// Execução da CPU
void executar_cpu() {
    PCB *proc = &processos[processo_atual];
    proc->tempo_restante--;
    quantum_atual--;

    // Verificar se o processo terminou
    if (proc->tempo_restante == 0) {
        proc->estado = TERMINADO;
        int turnaround = tempo_relogio - proc->tempo_chegada;
        printf("Tempo %4d: Processo %2d terminou. Espera: %2d, Turnaround: %2d\n",
               tempo_relogio, proc->pid, proc->tempo_espera, turnaround);
        processos_terminados++;
        processo_atual = -1;
        return;
    }

    // Verificar solicitação de I/O
    if ((double)rand() / RAND_MAX < PROBABILIDADE_IO) {
        TipoIO tipo_io = rand() % 3;
        int duracao_io;
        const char *nome_io = "";
        
        switch (tipo_io) {
            case DISCO: 
                duracao_io = 5; 
                nome_io = "DISCO";
                break;
            case FITA: 
                duracao_io = 3; 
                nome_io = "FITA";
                break;
            case IMPRESSORA: 
                duracao_io = 4; 
                nome_io = "IMPRESSORA";
                break;
        }

        proc->estado = EM_IO;
        proc->tipo_io = tipo_io;
        proc->tempo_io_restante = duracao_io;
        enfileirar(&fila_io, processo_atual);
        printf("Tempo %4d: Processo %2d solicitou I/O (%s)\n", 
               tempo_relogio, proc->pid, nome_io);
        processo_atual = -1;
        return;
    }

    // Verificar se o quantum expirou
    if (quantum_atual == 0) {
        proc->estado = PRONTO_Q1;
        proc->inicio_espera = tempo_relogio;
        enfileirar(&fila1, processo_atual);
        printf("Tempo %4d: Processo %2d preemptado -> Fila Q1\n", tempo_relogio, proc->pid);
        processo_atual = -1;
    }
}

// Loop principal de simulação
void simular() {
    printf("\n======== INICIO DA SIMULACAO ========\n");
    printf("Configuracao:\n");
    printf("- Processos: %d\n", total_processos);
    printf("- Quantum Q0 (alta prioridade): %d\n", QUANTUM_Q0);
    printf("- Quantum Q1 (baixa prioridade): %d\n", QUANTUM_Q1);
    printf("- Probabilidade de I/O: %.0f%%\n", PROBABILIDADE_IO * 100);
    printf("====================================\n\n");
    
    while (tempo_relogio < TEMPO_MAXIMO && processos_terminados < total_processos) {
        // Atualizar fila de I/O no início de cada unidade de tempo
        if (!fila_vazia(&fila_io)) {
            atualizar_fila_io();
        }

        // Escalonar se não há processo em execução
        if (processo_atual == -1) {
            escalonar();
        }

        // Executar CPU se houver processo
        if (processo_atual != -1) {
            executar_cpu();
        } else {
            cpu_ociosa++;
            printf("Tempo %4d: CPU ociosa\n", tempo_relogio);
        }

        tempo_relogio++;
    }
}

// Cálculo e exibição de estatísticas
void mostrar_estatisticas() {
    float tempo_medio_espera = 0, tempo_medio_turnaround = 0;
    
    printf("\n======== ESTATISTICAS DA SIMULACAO ========\n");
    printf("PID  Execucao  Espera  Turnaround\n");
    printf("----------------------------------\n");
    for (int i = 0; i < total_processos; i++) {
        int turnaround = processos[i].tempo_espera + processos[i].tempo_execucao;
        printf("%2d      %3d       %3d      %3d\n", 
               processos[i].pid,
               processos[i].tempo_execucao,
               processos[i].tempo_espera,
               turnaround);
        
        tempo_medio_espera += processos[i].tempo_espera;
        tempo_medio_turnaround += turnaround;
    }
    
    tempo_medio_espera /= total_processos;
    tempo_medio_turnaround /= total_processos;
    
    // Calcular utilização da CPU
    float utilizacao_cpu = 100.0 * (tempo_relogio - cpu_ociosa) / tempo_relogio;
    
    printf("\nMetricas Gerais:\n");
    printf("- Tempo total de simulacao: %d unidades\n", tempo_relogio);
    printf("- Processos concluidos: %d/%d\n", processos_terminados, total_processos);
    printf("- Tempo medio de espera: %.2f\n", tempo_medio_espera);
    printf("- Tempo medio de turnaround: %.2f\n", tempo_medio_turnaround);
    printf("- Utilizacao da CPU: %.2f%%\n", utilizacao_cpu);
    printf("==========================================\n");
}

int main() {
    // Configurar locale para suportar acentuação
    setlocale(LC_ALL, "Portuguese");
    
    // Inicializar filas
    inicializar_fila(&fila0);
    inicializar_fila(&fila1);
    inicializar_fila(&fila_io);

    // Criar processos
    int num_processos = 5;
    inicializar_processos(num_processos);
    
    // Executar simulação
    simular();
    
    // Mostrar estatísticas
    mostrar_estatisticas();
    
    return 0;
}