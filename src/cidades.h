#ifndef CIDADES_H
#define CIDADES_H

typedef struct Cidade {

    char Nome[256]; // Nome da cidade
    int Posicao; // Posicao (coordenada) da cidade
    struct Cidade * Proximo; // Ponteiro para a proxima cidade da estrada

} Cidade; 

typedef struct {

    int N; // Numero de cidades
    int T; // Comprimento da estrada 
    Cidade * Inicio; // Ponteiro para a primeira cidade da estrada 

} Estrada;

Estrada *getEstrada(const char *nomeArquivo);

// Lê, valida restrições e inicializa uma lista encadeada com os NOMES das cidades (ordem de leitura).
// Em caso de violação das restrições, retornar NULL. 

double calcularMenorVizinhanca(const char *nomeArquivo);

// Retorna o menor comprimento de vizinhança entre todas as cidades, como double.

char *cidadeMenorVizinhanca(const char *nomeArquivo);

// Retorna (char*) com o NOME da cidade que possui a menor vizinhança.
// A função deve alocar a string e devolvê-la ao chamador (que a libera).

#endif