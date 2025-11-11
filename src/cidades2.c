#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "cidades.h"

// Função auxiliar para comparar posições
static int cmpPos(const void *a, const void *b) {
    const Cidade *c1 = (const Cidade *)a;
    const Cidade *c2 = (const Cidade *)b;
    return c1->Posicao - c2->Posicao;
}

static void trim_crlf(char *s) {
    if (!s) return;
    size_t n = strlen(s);
    while (n > 0 && (s[n-1] == '\n' || s[n-1] == '\r')) {
        s[--n] = '\0';
    }
}

static void freeLista(Cidade *inicio) {
    Cidade *cur = inicio;
    while (cur) {
        Cidade *nxt = cur->Proximo;
        free(cur);
        cur = nxt;
    }
}

// Lê arquivo e cria Estrada
Estrada *getEstrada(const char *nomeArquivo) {
    FILE *f = fopen(nomeArquivo, "r");

    Estrada *E = malloc(sizeof(Estrada));
    if (!E) {
        if (f) fclose(f);
        return NULL;
    }
    // Inicializa padrão: se ocorrer qualquer erro retornamos E com N=0,T=0,Inicio=NULL
    E->T = 0;
    E->N = 0;
    E->Inicio = NULL;

    if (!f) {
        // arquivo não abriu -> retorna estrutura vazia (para evitar segfault em main)
        return E;
    }

    int T, N;
    if (fscanf(f, "%d", &T) != 1) { fclose(f); return E; }
    if (fscanf(f, "%d", &N) != 1) { fclose(f); return E; }

    if (T < 3 || T > 1000000 || N < 2 || N > 10000) {
        fclose(f);
        return E;
    }

    E->T = T;
    E->N = N;
    E->Inicio = NULL;

    Cidade *ultimo = NULL;
    for (int i = 0; i < N; i++) {
        Cidade *c = malloc(sizeof(Cidade));
        if (!c) {
            // falha alocação: limpa e retorna estado inválido
            freeLista(E->Inicio);
            E->Inicio = NULL;
            E->T = 0;
            E->N = 0;
            fclose(f);
            return E;
        }
        // ler posicao
        if (fscanf(f, "%d", &c->Posicao) != 1) {
            free(c);
            freeLista(E->Inicio);
            E->Inicio = NULL;
            E->T = 0;
            E->N = 0;
            fclose(f);
            return E;
        }
        // ler nome (consome espaços em branco antes)
        if (fscanf(f, " %[^\n]", c->Nome) != 1) {
            free(c);
            freeLista(E->Inicio);
            E->Inicio = NULL;
            E->T = 0;
            E->N = 0;
            fclose(f);
            return E;
        }
        trim_crlf(c->Nome);

        if (c->Posicao <= 0 || c->Posicao >= T) {
            free(c);
            freeLista(E->Inicio);
            E->Inicio = NULL;
            E->T = 0;
            E->N = 0;
            fclose(f);
            return E;
        }
        c->Proximo = NULL;
        if (!E->Inicio) E->Inicio = c;
        else ultimo->Proximo = c;
        ultimo = c;
    }

    fclose(f);
    return E;
}

// Calcula menor vizinhança
double calcularMenorVizinhanca(const char *nomeArquivo) {
    FILE *f = fopen(nomeArquivo, "r");
    if (!f) return -1.0;

    int T, N;
    if (fscanf(f, "%d", &T) != 1) { fclose(f); return -1.0; }
    if (fscanf(f, "%d", &N) != 1) { fclose(f); return -1.0; }

    if (N <= 0) { fclose(f); return -1.0; }

    Cidade *arr = malloc(N * sizeof(Cidade));
    if (!arr) { fclose(f); return -1.0; }

    for (int i = 0; i < N; i++) {
        if (fscanf(f, "%d", &arr[i].Posicao) != 1) { free(arr); fclose(f); return -1.0; }
        if (fscanf(f, " %[^\n]", arr[i].Nome) != 1) { free(arr); fclose(f); return -1.0; }
        trim_crlf(arr[i].Nome);
    }
    fclose(f);

    qsort(arr, N, sizeof(Cidade), cmpPos);

    double menor = (double)T;
    for (int i = 0; i < N; i++) {
        double viz;
        if (i == 0) viz = (arr[0].Posicao + arr[1].Posicao)/2.0;
        else if (i == N-1) viz = T - (arr[N-2].Posicao + arr[N-1].Posicao)/2.0;
        else viz = (arr[i+1].Posicao - arr[i-1].Posicao)/2.0;
        if (viz < menor) menor = viz;
    }

    free(arr);
    return menor;
}

// Retorna nome da cidade com menor vizinhança
char *cidadeMenorVizinhanca(const char *nomeArquivo) {
    FILE *f = fopen(nomeArquivo, "r");
    if (!f) {
        // retorna string alocada vazia para evitar strcmp(NULL,...)
        char *v = malloc(1);
        if (v) v[0] = '\0';
        return v;
    }

    int T, N;
    if (fscanf(f, "%d", &T) != 1) { fclose(f); char *v = malloc(1); if (v) v[0] = '\0'; return v; }
    if (fscanf(f, "%d", &N) != 1) { fclose(f); char *v = malloc(1); if (v) v[0] = '\0'; return v; }

    if (N <= 0) { fclose(f); char *v = malloc(1); if (v) v[0] = '\0'; return v; }

    Cidade *arr = malloc(N * sizeof(Cidade));
    if (!arr) { fclose(f); char *v = malloc(1); if (v) v[0] = '\0'; return v; }

    for (int i = 0; i < N; i++) {
        if (fscanf(f, "%d", &arr[i].Posicao) != 1) { free(arr); fclose(f); char *v = malloc(1); if (v) v[0] = '\0'; return v; }
        if (fscanf(f, " %[^\n]", arr[i].Nome) != 1) { free(arr); fclose(f); char *v = malloc(1); if (v) v[0] = '\0'; return v; }
        trim_crlf(arr[i].Nome);
    }
    fclose(f);

    qsort(arr, N, sizeof(Cidade), cmpPos);

    double menor = (double)T;
    int idx = 0;
    for (int i = 0; i < N; i++) {
        double viz;
        if (i == 0) viz = (arr[0].Posicao + arr[1].Posicao)/2.0;
        else if (i == N-1) viz = T - (arr[N-2].Posicao + arr[N-1].Posicao)/2.0;
        else viz = (arr[i+1].Posicao - arr[i-1].Posicao)/2.0;
        if (viz < menor) { menor = viz; idx = i; }
    }

    char *nome = malloc(strlen(arr[idx].Nome) + 1);
    if (nome) strcpy(nome, arr[idx].Nome);
    else {
        // fallback para string vazia em caso de falha de alocação
        nome = malloc(1);
        if (nome) nome[0] = '\0';
    }

    free(arr);
    return nome;
}