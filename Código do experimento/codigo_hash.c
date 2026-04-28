#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <sys/resource.h>

#define N_EXECUCOES 13
int TAMANHO;

typedef struct No {
    int chave;
    struct No* proximo;
} No;

typedef struct {
    No** buckets;
} TabelaHash;

typedef struct {
    double tempo;
    long long comparacoes;
    long memoria_pico;
} Metrics;

long get_peak_memory() {
    struct rusage usage;
    getrusage(RUSAGE_SELF, &usage);
    return usage.ru_maxrss;
}

int funcaoHash(int chave) {
    return abs(chave % TAMANHO);
}

void inicializar(TabelaHash* t) {
    t->buckets = (No**) calloc(TAMANHO,sizeof(No*));
}

long long inserir(TabelaHash* t, int chave) {
    long long comps = 0;
    int idx = funcaoHash(chave);
    No* cur = t->buckets[idx];
    while (cur != NULL) { comps++; cur = cur->proximo; }
    No* novo = (No*) malloc(sizeof(No));
    novo->chave   = chave;
    novo->proximo = t->buckets[idx];
    t->buckets[idx] = novo;
    return comps; //Gambiarra para contar o numero de comparações, o retorno é só para isso
}

void liberar(TabelaHash* t) {
    for (int i = 0; i < TAMANHO; i++) {
        No* cur = t->buckets[i];
        while (cur) { No* tmp = cur; cur = cur->proximo; free(tmp); }
        t->buckets[i] = NULL;
    }
}

long long metodosdeordenacao(TabelaHash* tab, int t, int tipo, unsigned int seed) {
    long long comps = 0;

    switch (tipo) {
        case 1:
            for (int i = 0; i < t; i++) comps += inserir(tab, i + 1);
            break;
        case 2:
            for (int i = t; i > 0; i--) comps += inserir(tab, i);
            break;
        case 3:
            srand(seed); // mesma seed -> mesmo embaralhamento nas 13 execucoes
            for (int i = 0; i < t; i++) {
                int j = rand();
                comps += inserir(tab,j);
            }
            break;
    }
    return comps;
}

Metrics executar_teste(TabelaHash* tab, int t, int tipo, unsigned int seed) {
    struct timespec start, end;
    clock_gettime(CLOCK_MONOTONIC, &start);
    long long comps = metodosdeordenacao(tab, t, tipo, seed);
    clock_gettime(CLOCK_MONOTONIC, &end);
    double tempo = (end.tv_sec - start.tv_sec) + (end.tv_nsec - start.tv_nsec) / 1e9;
    Metrics m = {tempo, comps, get_peak_memory()};
    return m;
}

void salvar_csv(FILE* f, int tipo, int n, int exec, double tempo, long long comps, long mem) {
    fprintf(f, "%d,%d,%d,%.9f,%lld,%ld\n", tipo, n, exec, tempo, comps, mem);
}

int main(int argc, char* argv[]) {

    if (argc < 2) {
        printf("Uso:\n");
        printf("  %s 1              (Crescente)\n",   argv[0]);
        printf("  %s 2              (Decrescente)\n", argv[0]);
        printf("  %s 3 <seed>       (Aleatorio - seed obrigatorio)\n", argv[0]);
        printf("\nExemplos:\n");
        printf("  ./codigo_hash 1\n");
        printf("  ./codigo_hash 2\n");
        printf("  ./codigo_hash 3 42\n");
        return 1;
    }

    int tipo = atoi(argv[1]);
    if (tipo < 1 || tipo > 3) {
        printf("Erro: tipo invalido '%s'.\n", argv[1]);
        printf("Use 1 (Crescente), 2 (Decrescente) ou 3 (Aleatorio).\n");
        return 1;
    }

    unsigned int seed = 0;
    if (tipo == 3) {
        if (argc < 3) {
            printf("Erro: o tipo Aleatorio (3) requer uma seed.\n");
            printf("Exemplo: ./codigo_hash 3 42\n");
            return 1;
        }
        seed = (unsigned int) atoi(argv[2]);
        printf("Seed utilizada: %u\n", seed);
    }

    const char* nomes_tipo[] = {"", "Crescente", "Decrescente", "Aleatorio"};
    int entradas[] = {1000, 10000, 100000, 1000000 , 10000000};
    int n_entradas = 5;

    char nome_csv[64];
    snprintf(nome_csv, sizeof(nome_csv), "resultados_hash_%s.csv",
             tipo == 1 ? "crescente" : tipo == 2 ? "decrescente" : "aleatorio");

    FILE* csv = fopen(nome_csv, "w");
    if (!csv) { fprintf(stderr, "Erro ao criar CSV!\n"); return 1; }
    fprintf(csv, "tipo,n_elementos,execucao,tempo_s,comparacoes,memoria_kb\n");

    printf("\n=== Tipo: %s ===\n", nomes_tipo[tipo]);
    printf("%-10s  %-4s  %-14s  %-14s  %-12s\n",
           "Elementos", "Exec", "Tempo (s)", "Comparacoes", "Memoria (KB)");
    printf("----------  ----  --------------  --------------  ------------\n");

    for (int e = 0; e < n_entradas; e++) {
        int tam = entradas[e];
        double    soma_tempo = 0;
        long long soma_comps = 0;
        long      soma_mem   = 0;
        TAMANHO = entradas[e];

        for (int exec = 1; exec <= N_EXECUCOES; exec++) {
            TabelaHash tab;
            inicializar(&tab);
            Metrics m = executar_teste(&tab, tam, tipo, seed);
            printf("%-10d  %-4d  %-14.9f  %-14lld  %-12ld\n",
                   tam, exec, m.tempo, m.comparacoes, m.memoria_pico);
            salvar_csv(csv, tipo, tam, exec, m.tempo, m.comparacoes, m.memoria_pico);
            soma_tempo += m.tempo;
            soma_comps += m.comparacoes;
            soma_mem   += m.memoria_pico;
            liberar(&tab);
        }

        fprintf(csv, "%d,%d,MEDIA,%.9f,%.2f,%.2f\n",
                tipo, tam,
                soma_tempo / N_EXECUCOES,
                (double)soma_comps / N_EXECUCOES,
                (double)soma_mem   / N_EXECUCOES);

        printf("%-10d  %-4s  %-14.9f  %-14.2f  %-12.2f  << MEDIA\n\n",
               tam, "MED",
               soma_tempo / N_EXECUCOES,
               (double)soma_comps / N_EXECUCOES,
               (double)soma_mem   / N_EXECUCOES);
    }

    fclose(csv);
    printf("Resultados salvos em: %s\n", nome_csv);
    return 0;
}