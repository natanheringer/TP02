#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "cidades.h"


// Função auxiliar para comparar cidades pela posição (qsort)
int compararCidades(const void *a, const void *b) {
    
    const Cidade *ca = (const Cidade *)a;
    const Cidade *cb = (const Cidade *)b;
    return ca->Posicao - cb->Posicao;

}

// Função auxiliar para limpar string (remover espaços iniciais e finais)
void limparString(char *str) {

    // remove espaços iniciais
    char *inicio = str;
    while (*inicio == ' ' || *inicio == '\t') {

        inicio++;

    }

    // Move string para o início se necessário
    if (inicio != str) {

        char *dest = str;
        while (*inicio != '\0') {

            *dest++ = *inicio++;

        }
        *dest = '\0';

    }
    
    // Remove quebra de linha e espaços finais 
    int len = strlen(str);
    while (len > 0 && (str[len-1] == '\n' || str[len-1] == '\r' || str[len-1] == ' ' || str[len-1] == '\t')) {

        str[len-1] = '\0';
        len--;

    }

}



// Inicializa cidades no TAD Cidade
Estrada *getEstrada(const char *nomeArquivo) {

    FILE *arquivo = fopen(nomeArquivo, "r");

    if (arquivo == NULL) {

        return NULL;

    }

    int T, N; 

    // lê T (comprimento da estrada)
    if (fscanf(arquivo, "%d", &T) != 1) {

        fclose(arquivo);
        return NULL;

    }

    // Lê N (número de cidades)
    if (fscanf(arquivo, "%d", &N) != 1) {

        fclose(arquivo);
        return NULL;

    } 

    // Verifica restrições: 3 ≤ T ≤ 10⁶ e 2 ≤ N ≤ 10⁴
    if (T < 3 || T > 1000000 || N < 2 || N > 10000) {

        fclose(arquivo);
        return NULL;

    }

    // Array temporário para armazenar cidades durante a leitura 
    Cidade *temp = (Cidade *)malloc(N * sizeof(Cidade));
    if (temp == NULL) {
        
        fclose(arquivo);
        return NULL;

    }

    // Le as N cidades 
    for (int i = 0; i < N; i++) {

        int Xi;
        char linha[512];

        // lê a posição 
        if (fscanf(arquivo, "%d", &Xi) != 1) {

            free(temp);
            fclose(arquivo);
            return NULL;

        }

        // lê o resto da linha (nome da cidade, pode ter espaços)
        if (fgets(linha, sizeof(linha), arquivo) == NULL) {

            free(temp);
            fclose(arquivo);
            return NULL;

        }

        // limpa a string (remove espaços iniciais e finais)
        limparString(linha);

        // validar restrição: 0 < Xi < T 
        if (Xi <= 0 || Xi >= T) {

            free(temp);
            fclose(arquivo);
            return NULL;

        }


        // validar restrição: Xi != Xj (verificar duplicatas)
        for (int j = 0; j < i; j++) {

            if (temp[j].Posicao == Xi) {
                
                free(temp);
                fclose(arquivo);
                return NULL;

            }

        }

        // Armazena os dados 
        temp[i].Posicao = Xi; 
        strncpy(temp[i].Nome, linha, 255);
        temp[i].Nome[255] = '\0';

    }

    fclose(arquivo);

    // Criar estrutura Estrada 
    Estrada *estrada = (Estrada *)malloc(sizeof(Estrada));
    if (estrada == NULL) {

        free(temp);
        return NULL;

    }

    estrada->T = T;
    estrada->N = N; 

    // Criar Lista encadeada na ordem de leitura
    Cidade *inicio = NULL;
    Cidade *final = NULL;

    for (int i = 0; i < N; i++) {

        Cidade *nova = (Cidade *)malloc(sizeof(Cidade));

        if (nova == NULL) {

            // Liberar memoria ja alocada 
            Cidade *atual = inicio; 
            while (atual != NULL) {

                Cidade *prox = atual->Proximo;
                free(atual);
                atual = prox;

            }

            free(temp);
            free(estrada);
            return NULL;
             
        }

        strncpy(nova->Nome, temp[i].Nome, 255);
        nova->Nome[255] = '\0';
        nova->Posicao = temp[i].Posicao;
        nova->Proximo = NULL;

        if (inicio == NULL) {

            inicio = nova;
            final = nova;

        } else {

            final->Proximo = nova;
            final = nova;

        }

    }

    estrada->Inicio = inicio;
    free(temp);
    return estrada;

}


// Retorna a menor vizinhança de estrada entre as cidades 
double calcularMenorVizinhanca(const char *nomeArquivo) {

    FILE *arquivo = fopen(nomeArquivo, "r");
    if (arquivo == NULL) {
    
        return 0.0;
    
    }

    int T, N;
    
    // Lê T (comprimento da estrada)
    if (fscanf(arquivo, "%d", &T) != 1) {
    
        fclose(arquivo);
        return 0.0;
    
    }
    
    // Lê N (número de cidades)
    if (fscanf(arquivo, "%d", &N) != 1) {
    
        fclose(arquivo);
        return 0.0;
    
    }

    // Array para armazenar cidades
    Cidade *cidades = (Cidade *)malloc(N * sizeof(Cidade));
    if (cidades == NULL) {
    
        fclose(arquivo);
        return 0.0;
    
    }

    // Lê as cidades
    for (int i = 0; i < N; i++) {
    
        int Xi;
        char linha[512];
        
        if (fscanf(arquivo, "%d", &Xi) != 1) {
    
            free(cidades);
            fclose(arquivo);
            return 0.0;
    
        }
        
        if (fgets(linha, sizeof(linha), arquivo) == NULL) {
    
            free(cidades);
            fclose(arquivo);
            return 0.0;
    
        }
        
        limparString(linha);
        
        cidades[i].Posicao = Xi;
        strncpy(cidades[i].Nome, linha, 255);
        cidades[i].Nome[255] = '\0';
    
    }
    
    fclose(arquivo);

    // Ordena cidades por posição
    qsort(cidades, N, sizeof(Cidade), compararCidades);

    // Calcula vizinhança de cada cidade e encontra a menor
    double menorVizinhanca = 1e9;
    
    for (int i = 0; i < N; i++) {
        double limite_esquerda, limite_direita;
        
        // Primeira cidade: de 0 até ponto médio com próxima
        if (i == 0) {
        
            limite_esquerda = 0.0;
            limite_direita = (cidades[i].Posicao + cidades[i+1].Posicao) / 2.0;
        
        }
        
        // Última cidade: do ponto médio com anterior até T
        else if (i == N - 1) {
        
            limite_esquerda = (cidades[i-1].Posicao + cidades[i].Posicao) / 2.0;
            limite_direita = (double)T;
        
        }
        
        // Cidades do meio: entre pontos médios
        else {
        
            limite_esquerda = (cidades[i-1].Posicao + cidades[i].Posicao) / 2.0;
            limite_direita = (cidades[i].Posicao + cidades[i+1].Posicao) / 2.0;
        
        }
        
        double vizinhanca = limite_direita - limite_esquerda;
        
        if (vizinhanca < menorVizinhanca) {

            menorVizinhanca = vizinhanca;
        
        }
    }

    free(cidades);
    return menorVizinhanca;


}

// retorna a cidade que tem menor vizinhança
char *cidadeMenorVizinhanca(const char *nomeArquivo) {

    FILE *arquivo = fopen(nomeArquivo, "r");
    if (arquivo == NULL) {

        return NULL;

    }

    int T, N;

    // lê T (comprimento da estrada)
    if (fscanf(arquivo, "%d", &T) != 1) {

        fclose(arquivo);
        return NULL;
    
    }
    
    // lê N (número de cidades)
    if (fscanf(arquivo, "%d", &N) != 1) {

        fclose(arquivo);
        return NULL;
    
    }

    // Array para armazenar cidades
    Cidade *cidades = (Cidade *)malloc(N * sizeof(Cidade));
    if (cidades == NULL) {

        fclose(arquivo);
        return NULL;

    }

    // Lê as cidades 
    for (int i = 0; i < N; i++) {

        int Xi;
        char linha[512];

        // lê a posição 
        if (fscanf(arquivo, "%d", &Xi) != 1) {

            free(cidades);
            fclose(arquivo);
            return NULL;

        }

        if (fgets(linha, sizeof(linha), arquivo) == NULL) {

            free(cidades);
            fclose(arquivo);
            return NULL;

        }

        limparString(linha);

        cidades[i].Posicao = Xi;
        strncpy(cidades[i].Nome, linha, 255); 
        cidades[i].Nome[255] = '\0';

    }

    fclose(arquivo);

    // Ordena cidades por posição 
    qsort(cidades, N, sizeof(Cidade), compararCidades);

    // Calcula a vizinhança de cada cidade e encontra a menor 
    double menorVizinhanca = 1e9; 
    int indice_menor = 0; 

    for (int i = 0; i < N; i++) {

        double limite_esquerda, limite_direita;

        // Primeira cidade: de 0 até ponto médio com próxima
        if (i == 0) {
       
            limite_esquerda = 0.0;
            limite_direita = (cidades[i].Posicao + cidades[i+1].Posicao) / 2.0;
       
        }
       
        // Última cidade: do ponto médio com anterior até T
        else if (i == N - 1) {
       
            limite_esquerda = (cidades[i-1].Posicao + cidades[i].Posicao) / 2.0;
            limite_direita = (double)T;
       
        }

        // Cidades do meio: entre pontos médios
        else {

            limite_esquerda = (cidades[i-1].Posicao + cidades[i].Posicao) / 2.0;
            limite_direita = (cidades[i].Posicao + cidades[i+1].Posicao) / 2.0;

        }

        double vizinhanca = limite_direita - limite_esquerda;
        if (vizinhanca < menorVizinhanca) {

            menorVizinhanca = vizinhanca;
            indice_menor = i;

        }


    }

    // Aloca memória para o nome da cidade com menor vizinhança 
    int tamanho_nome = strlen(cidades[indice_menor].Nome);
    char *resultado = (char *)malloc((tamanho_nome + 1) * sizeof(char));
    if (resultado == NULL) {

        free(cidades);
        return NULL;

    }
    
    strcpy(resultado, cidades[indice_menor].Nome);
    free(cidades);
    return resultado;

}