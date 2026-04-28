// Tabela Hash com Encadeamento Exteriorr


#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define TAMANHO 10

typedef struct No {
    int chave;
    struct No* proximo;
} No;

typedef struct {
    No* buckets[TAMANHO];
} TabelaHash;

int funcaoHash(int chave) {
    return abs(chave % TAMANHO);
}

void inicializar(TabelaHash* t) {
    for (int i = 0; i < TAMANHO; i++) {
        t->buckets[i] = NULL;
    }
}

void inserir_silencioso(TabelaHash* t, int chave) {
    int idx = funcaoHash(chave);
    No* novo = (No*) malloc(sizeof(No));
    novo->chave   = chave;
    novo->proximo = t->buckets[idx];
    t->buckets[idx] = novo;
}

void inserir(TabelaHash* t, int chave) {
    int idx  = funcaoHash(chave);
    No* atual = t->buckets[idx];

    while (atual) {
        if (atual->chave == chave) {
            printf("Chave %d ja existe na tabela.\n", chave);
            return;
        }
        atual = atual->proximo;
    }

    No* novo = (No*) malloc(sizeof(No));
    novo->chave   = chave;
    novo->proximo = t->buckets[idx];
    t->buckets[idx] = novo;
    printf("Inserido: %d --> bucket[%d]\n", chave, idx);
}

No* buscar(TabelaHash* t, int chave) {
    int idx  = funcaoHash(chave);
    No* atual = t->buckets[idx];

    while (atual) {
        if (atual->chave == chave) return atual;
        atual = atual->proximo;
    }
    return NULL;
}

void remover(TabelaHash* t, int chave) {
    int idx   = funcaoHash(chave);
    No* atual = t->buckets[idx];
    No* ant   = NULL;

    while (atual) {
        if (atual->chave == chave) {
            if (ant) ant->proximo = atual->proximo;
            else     t->buckets[idx] = atual->proximo;
            free(atual);
            printf("Chave %d removida com sucesso.\n", chave);
            return;
        }
        ant   = atual;
        atual = atual->proximo;
    }
    printf("Chave %d nao encontrada para remocao.\n", chave);
}

void exibir(TabelaHash* t) {
    printf("\n=== Estado da Tabela Hash ===\n");
    for (int i = 0; i < TAMANHO; i++) {
        printf("bucket[%2d]: ", i);
        No* cur = t->buckets[i];
        if (!cur) {
            printf("(vazio)");
        }
        while (cur) {
            printf("%d", cur->chave);
            if (cur->proximo) printf(" -> ");
            cur = cur->proximo;
        }
        printf("\n");
    }
    printf("=============================\n\n");
}

void liberar(TabelaHash* t) {
    for (int i = 0; i < TAMANHO; i++) {
        No* cur = t->buckets[i];
        while (cur) {
            No* tmp = cur;
            cur = cur->proximo;
            free(tmp);
        }
        t->buckets[i] = NULL;
    }
}

void experimento() {
    int entradas[] = {1000, 5000, 10000, 50000, 100000};
    int n = 5;

    printf("\n=== Experimento: Tempo de Insercao ===\n");
    printf("%-12s | %-16s\n", "Entradas (n)", "Tempo (segundos)");
    printf("-------------|------------------\n");

    for (int t = 0; t < n; t++) {
        TabelaHash tab;
        inicializar(&tab);

        clock_t inicio = clock();
        for (int i = 0; i < entradas[t]; i++) {
            inserir_silencioso(&tab, i);
        }
        double tempo = (double)(clock() - inicio) / CLOCKS_PER_SEC;

        printf("%-12d | %.6f\n", entradas[t], tempo);
        liberar(&tab);
    }
    printf("======================================\n");
}

int main() {
    TabelaHash t;
    inicializar(&t);

    printf("=== Tabela Hash com Encadeamento Separado ===\n\n");

    inserir(&t, 15);
    inserir(&t, 25);
    inserir(&t, 35);
    inserir(&t, 7);
    inserir(&t, 42);
    inserir(&t, 100);
    inserir(&t, 25);

    exibir(&t);

    int alvo = 25;
    No* resultado = buscar(&t, alvo);
    printf("Busca pela chave %d: %s\n\n", alvo, resultado ? "ENCONTRADA" : "NAO encontrada");

    remover(&t, 25);
    remover(&t, 99);

    exibir(&t);

    experimento();

    liberar(&t);
    return 0;
}
