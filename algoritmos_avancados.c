#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define HASH_SIZE 11

/* -------------------------------------------------------------------------- */
/* DEFINIÇÕES DE ESTRUTURAS                          */
/* -------------------------------------------------------------------------- */

// Estrutura para a Árvore Binária da Mansão
typedef struct RoomNode {
    char nome[50];
    char pista[100];
    struct RoomNode *esquerda;
    struct RoomNode *direita;
} RoomNode;

// Estrutura para a Árvore Binária de Busca (BST) de Pistas Coletadas
typedef struct ClueNode {
    char pista[100];
    struct ClueNode *esquerda;
    struct ClueNode *direita;
} ClueNode;

// Estrutura para os nós da Tabela Hash (Mapeamento Pista -> Suspeito)
typedef struct HashEntry {
    char pista[100];
    char suspeito[50];
    struct HashEntry *next;
} HashEntry;

// Estrutura principal da Tabela Hash
typedef struct {
    HashEntry *buckets[HASH_SIZE];
} HashTable;

/* -------------------------------------------------------------------------- */
/* PROTÓTIPOS DAS FUNÇÕES                            */
/* -------------------------------------------------------------------------- */
RoomNode* criarSala(const char *nome, const char *pista);
void explorarSalas(RoomNode *raizMansao, ClueNode **raizPistas, HashTable *tabela);
ClueNode* inserirPista(ClueNode *raiz, const char *pista);
void inicializarHash(HashTable *tabela);
unsigned int calcularHash(const char *str);
void inserirNaHash(HashTable *tabela, const char *pista, const char *suspeito);
const char* encontrarSuspeito(HashTable *tabela, const char *pista);
void exibirPistasEmOrdem(ClueNode *raiz);
void contarPistasPorSuspeito(ClueNode *raiz, HashTable *tabela, int *joao, int *jose, int *maria, int *cristiano);
void verificarSuspeitoFinal(ClueNode *raizPistas, HashTable *tabela);
void liberarMansao(RoomNode *raiz);
void liberarBST(ClueNode *raiz);
void liberarHash(HashTable *tabela);

/* -------------------------------------------------------------------------- */
/* IMPLEMENTAÇÃO DAS FUNÇÕES                         */
/* -------------------------------------------------------------------------- */

/**
 * Função: criarSala
 * Descrição: Cria dinamicamente um cômodo (nó da árvore da mansão) utilizando malloc.
 * Inicializa os ponteiros de caminhos (esquerda/direita) como NULL.
 * Parâmetros: nome - String identificadora do cômodo.
 * pista - String opcional com a pista estática daquele cômodo.
 * Retorno: Ponteiro para a nova sala criada.
 */
RoomNode* criarSala(const char *nome, const char *pista) {
    RoomNode *novaSala = (RoomNode*)malloc(sizeof(RoomNode));
    if (novaSala == NULL) {
        printf("Erro fatal: Falha na alocacao de memoria para a sala.\n");
        exit(1);
    }
    strcpy(novaSala->nome, nome);
    strcpy(novaSala->pista, pista);
    novaSala->esquerda = NULL;
    novaSala->direita = NULL;
    return novaSala;
}

/**
 * Função: inserirPista
 * Descrição: Insere uma pista coletada na Árvore Binária de Busca (BST) de forma
 * ordenada alfabeticamente (utilizando strcmp). Evita duplicatas.
 * Parâmetros: raiz - Ponteiro para a raiz atual da BST de pistas.
 * pista - String contendo o texto da pista a ser inserida.
 * Retorno: Ponteiro atualizado da raiz da BST.
 */
ClueNode* inserirPista(ClueNode *raiz, const char *pista) {
    if (raiz == NULL) {
        ClueNode *novoNo = (ClueNode*)malloc(sizeof(ClueNode));
        if (novoNo == NULL) {
            printf("Erro fatal: Falha na alocacao de memoria para a pista.\n");
            exit(1);
        }
        strcpy(novoNo->pista, pista);
        novoNo->esquerda = NULL;
        novoNo->direita = NULL;
        return novoNo;
    }

    int comparacao = strcmp(pista, raiz->pista);
    if (comparacao < 0) {
        raiz->esquerda = inserirPista(raiz->esquerda, pista);
    } else if (comparacao > 0) {
        raiz->direita = inserirPista(raiz->direita, pista);
    }
    return raiz;
}

/**
 * Função: inicializarHash
 * Descrição: Define todos os buckets da tabela hash como NULL.
 */
void inicializarHash(HashTable *tabela) {
    for (int i = 0; i < HASH_SIZE; i++) {
        tabela->buckets[i] = NULL;
    }
}

/**
 * Função: calcularHash
 * Descrição: Função de hash simples baseada na soma dos caracteres ASCII da string.
 */
unsigned int calcularHash(const char *str) {
    unsigned int hash = 0;
    while (*str) {
        hash += *str++;
    }
    return hash % HASH_SIZE;
}

/**
 * Função: inserirNaHash
 * Descrição: Associa uma pista a um suspeito inserindo o par chave/valor na tabela hash.
 * Trata colisões adicionando o elemento no início da lista encadeada (bucket).
 * Parâmetros: tabela - Ponteiro para a tabela hash do sistema.
 * pista - Chave identificadora (string).
 * suspeito - Valor associado à chave (string).
 */
void inserirNaHash(HashTable *tabela, const char *pista, const char *suspeito) {
    unsigned int indice = calcularHash(pista);
    HashEntry *novaEntrada = (HashEntry*)malloc(sizeof(HashEntry));
    if (novaEntrada == NULL) {
        printf("Erro fatal: Falha na alocacao de memoria para a tabela Hash.\n");
        exit(1);
    }
    strcpy(novaEntrada->pista, pista);
    strcpy(novaEntrada->suspeito, suspeito);
    
    // Tratamento de colisão por encadeamento no início
    novaEntrada->next = tabela->buckets[indice];
    tabela->buckets[indice] = novaEntrada;
}

/**
 * Função: encontrarSuspeito
 * Descrição: Consulta a tabela hash utilizando a string da pista como chave para 
 * encontrar qual suspeito está vinculado a ela.
 * Parâmetros: tabela - Ponteiro para a tabela hash.
 * pista - String contendo a chave de busca.
 * Retorno: String com o nome do suspeito correspondente, ou NULL se não encontrado.
 */
const char* encontrarSuspeito(HashTable *tabela, const char *pista) {
    unsigned int indice = calcularHash(pista);
    HashEntry *atual = tabela->buckets[indice];
    
    while (atual != NULL) {
        if (strcmp(atual->pista, pista) == 0) {
            return atual->suspeito;
        }
        atual = atual->next;
    }
    return NULL;
}

/**
 * Função: explorarSalas
 * Descrição: Controla a navegação interativa em tempo de execução pela árvore da mansão.
 * Exibe o cômodo atual, verifica e coleta pistas automaticamente enviando-as à BST,
 * trata comandos inválidos e encerra caso o jogador chegue a uma folha (sem saída) ou digite 's'.
 * Parâmetros: raizMansao - Ponteiro para o nó inicial (Hall de entrada).
 * raizPistas - Ponteiro duplo para gerenciar a raiz da BST de pistas coletadas.
 * tabela - Ponteiro para a tabela hash contendo os dados dos suspeitos.
 */
void explorarSalas(RoomNode *raizMansao, ClueNode **raizPistas, HashTable *tabela) {
    RoomNode *atual = raizMansao;
    char opcao;

    while (atual != NULL) {
        printf("\n==================================================\n");
        printf("VOCE ESTA NO(A): %s\n", atual->nome);
        printf("==================================================\n");

        // Mecânica de verificação e exibição de pistas na sala atual
        if (strlen(atual->pista) > 0) {
            printf("[!] PISTA ENCONTRADA: %s\n", atual->pista);
            
            // Procura o suspeito associado na Hash para exibição informativa
            const char *suspeitoVinculado = encontrarSuspeito(tabela, atual->pista);
            if (suspeitoVinculado != NULL) {
                printf("    (Esta pista aponta para o suspeito: %s)\n", suspeitoVinculado);
            }
            
            // Insere automaticamente na árvore BST de pistas ordenadas
            *raizPistas = inserirPista(*raizPistas, atual->pista);
        } else {
            printf("[.] Nao ha pistas evidentes neste comodo.\n");
        }

        // Condição de parada obrigatória: se a sala for um nó folha (Sem saídas adicionais)
        if (atual->esquerda == NULL && atual->direita == NULL) {
            printf("\n[FIM DA LINHA] Voce chegou a um comodo sem saidas adicionais. A exploracao acabou!\n");
            break;
        }

        // Menu de Navegação Interativa
        printf("\nCaminhos disponiveis:\n");
        if (atual->esquerda != NULL) printf(" - [e] Ir para a Esquerda: %s\n", atual->esquerda->nome);
        else printf(" - [e] Ir para a Esquerda: (Caminho Bloqueado/Parede)\n");

        if (atual->direita != NULL) printf(" - [d] Ir para a Direita: %s\n", atual->direita->nome);
        else printf(" - [d] Ir para a Direita: (Caminho Bloqueado/Parede)\n");

        printf(" - [s] Parar exploracao e ir para o Tribunal de Acusacao\n");
        
        printf("\nEscolha sua acao (e/d/s): ");
        scanf(" %c", &opcao);

        if (opcao == 's' || opcao == 'S') {
            printf("\nFinalizando buscas e preparando relatorio do detetive...\n");
            break;
        } else if (opcao == 'e' || opcao == 'E') {
            // Teste de caminho inexistente para a Esquerda
            if (atual->esquerda != NULL) {
                atual = atual->esquerda;
            } else {
                printf("\n[AVISO] BUM! Voce bateu de cara na parede. Nao ha caminho a esquerda!\n");
            }
        } else if (opcao == 'd' || opcao == 'D') {
            // Teste de caminho inexistente para a Direita
            if (atual->direita != NULL) {
                atual = atual->direita;
            } else {
                printf("\n[AVISO] BUM! Voce bateu de cara na parede. Nao ha caminho a direita!\n");
            }
        } else {
            printf("\n[ERRO] Comando invalido! Use apenas 'e' para esquerda, 'd' para direita ou 's' para sair.\n");
        }
    }
}

/**
 * Função Auxiliar: exibirPistasEmOrdem
 * Descrição: Exibe as strings da BST realizando um percurso em-ordem (In-Order Traversal),
 * garantindo a listagem em ordem alfabética.
 */
void exibirPistasEmOrdem(ClueNode *raiz) {
    if (raiz != NULL) {
        exibirPistasEmOrdem(raiz->esquerda);
        printf("  -> %s\n", raiz->pista);
        exibirPistasEmOrdem(raiz->direita);
    }
}

/**
 * Função Auxiliar: contarPistasPorSuspeito
 * Descrição: Percorre a BST de pistas e incrementa o contador de cada suspeito com base
 * nas consultas feitas dinamicamente na tabela Hash.
 */
void contarPistasPorSuspeito(ClueNode *raiz, HashTable *tabela, int *joao, int *jose, int *maria, int *cristiano) {
    if (raiz != NULL) {
        contarPistasPorSuspeito(raiz->esquerda, tabela, joao, jose, maria, cristiano);
        
        const char *suspeito = encontrarSuspeito(tabela, raiz->pista);
        if (suspeito != NULL) {
            if (strcmp(suspeito, "Joao") == 0) (*joao)++;
            else if (strcmp(suspeito, "Jose") == 0) (*jose)++;
            else if (strcmp(suspeito, "Maria") == 0) (*maria)++;
            else if (strcmp(suspeito, "Cristiano") == 0) (*cristiano)++;
        }
        
        contarPistasPorSuspeito(raiz->direita, tabela, joao, jose, maria, cristiano);
    }
}

/**
 * Função: verificarSuspeitoFinal
 * Descrição: Conduz o encerramento do jogo, lista todas as pistas coletadas em ordem alfabética,
 * recebe a acusação final do jogador e valida automaticamente se o suspeito escolhido
 * possui ao menos duas pistas válidas contra si armazenadas na BST.
 * Parâmetros: raizPistas - Raiz da árvore BST contendo as pistas coletadas.
 * tabela - Tabela hash contendo as relações pista/suspeito originais.
 */
void verificarSuspeitoFinal(ClueNode *raizPistas, HashTable *tabela) {
    printf("\n==================================================\n");
    printf("               FASE FINAL: ACUSACAO               \n");
    printf("==================================================\n");
    printf("Pistas coletadas (em ordem alfabetica):\n");
    
    if (raizPistas == NULL) {
        printf("  (Nenhuma pista coletada durante a investigacao!)\n");
    } else {
        exibirPistasEmOrdem(raizPistas);
    }

    char acusado[50];
    printf("\nInsira o nome do seu principal suspeito (Joao, Jose, Maria, Cristiano): ");
    scanf("%s", acusado);

    // Contadores para checagem de provas
    int contJoao = 0, contJose = 0, contMaria = 0, contCristiano = 0;
    contarPistasPorSuspeito(raizPistas, tabela, &contJoao, &contJose, &contMaria, &contCristiano);

    int totalProvasDoAcusado = 0;
    if (strcmp(acusado, "Joao") == 0) totalProvasDoAcusado = contJoao;
    else if (strcmp(acusado, "Jose") == 0) totalProvasDoAcusado = contJose;
    else if (strcmp(acusado, "Maria") == 0) totalProvasDoAcusado = contMaria;
    else if (strcmp(acusado, "Cristiano") == 0) totalProvasDoAcusado = contCristiano;

    printf("\n==================================================\n");
    printf("                 VEREDITO FINAL                   \n");
    printf("==================================================\n");
    
    // Regra de validação automática: Mínimo de duas pistas corretas
    if (totalProvasDoAcusado >= 2) {
        printf("EXCELENTE TRABALHO, DETETIVE! O caso foi encerrado com sucesso.\n");
        printf("Voce reuniu %d provas contundentes contra %s.\n", totalProvasDoAcusado, acusado);
        printf("O suspeito confessou o crime perante o júri!\n");
    } else {
        printf("PROVAS INSUFICIENTES! O caso colapsou.\n");
        printf("Voce apresentou apenas %d prova(s) contra %s.\n", totalProvasDoAcusado, acusado);
        printf("O suspeito foi liberado por falta de evidencias ou voce acusou a pessoa errada.\n");
    }
    printf("==================================================\n");
}

/* -------------------------------------------------------------------------- */
/* FUNÇÕES DE DESALOCAÇÃO DE MEMÓRIA                      */
/* -------------------------------------------------------------------------- */

void liberarMansao(RoomNode *raiz) {
    if (raiz != NULL) {
        liberarMansao(raiz->esquerda);
        liberarMansao(raiz->direita);
        free(raiz);
    }
}

void liberarBST(ClueNode *raiz) {
    if (raiz != NULL) {
        liberarBST(raiz->esquerda);
        liberarBST(raiz->direita);
        free(raiz);
    }
}

void liberarHash(HashTable *tabela) {
    for (int i = 0; i < HASH_SIZE; i++) {
        HashEntry *atual = tabela->buckets[i];
        while (atual != NULL) {
            HashEntry *aux = atual;
            atual = atual->next;
            free(aux);
        }
    }
}

/* -------------------------------------------------------------------------- */
/* FLUXO PRINCIPAL                               */
/* -------------------------------------------------------------------------- */

int main() {
    // 1. Inicialização da Tabela Hash com o banco de dados de pistas estáticas
    HashTable basePistas;
    inicializarHash(&basePistas);

    // Cadastrando pistas vinculadas a cada suspeito na tabela hash
    inserirNaHash(&basePistas, "Relogio de ouro com iniciais CR", "Cristiano");
    inserirNaHash(&basePistas, "Poema inacabado com a assinatura de Cristiano", "Cristiano");
    inserirNaHash(&basePistas, "Bilhete com a caligrafia de Cristiano", "Cristiano");
    inserirNaHash(&basePistas, "Fio de cabelo longo e perfume doce de Maria", "Maria");
    inserirNaHash(&basePistas, "Isqueiro de prata gravado com J.J.", "Joao");
    inserirNaHash(&basePistas, "Lenco bordado com o nome de Jose", "Jose");

    // 2. Construção Fixa da Árvore Binária representando o Mapa da Mansão
    // Cada nó recebe (Nome_do_Cômodo, Pista_Estática_ou_Vazia)
    RoomNode *hall = criarSala("Hall de Entrada", "");
    RoomNode *salaEstar = criarSala("Sala de Estar", "Relogio de ouro com iniciais CR");
    RoomNode *cozinha = criarSala("Cozinha", "Isqueiro de prata gravado com J.J.");
    RoomNode *escritorio = criarSala("Escritorio", "Bilhete com a caligrafia de Cristiano");
    RoomNode *biblioteca = criarSala("Biblioteca", "Poema inacabado com a assinatura de Cristiano");
    RoomNode *jardim = criarSala("Jardim", "");
    RoomNode *salaoReunioes = criarSala("Salao de Reunioes", "Fio de cabelo longo e perfume doce de Maria");
    RoomNode *quarto1 = criarSala("Quarto 1", "Lenco bordado com o nome de Jose");
    RoomNode *quarto2 = criarSala("Quarto 2", "");
    RoomNode *quarto3 = criarSala("Quarto 3", "");

    // Montando as conexões lógicas estruturais da Mansão (Esquerda / Direita)
    hall->esquerda = salaEstar;
    hall->direita = salaoReunioes;

    salaEstar->esquerda = cozinha; // cozinha é um nó folha (Sem saída)
    salaEstar->direita = escritorio;

    escritorio->esquerda = biblioteca; // biblioteca é um nó folha (Sem saída)
    escritorio->direita = jardim;      // jardim é um nó folha (Sem saída)

    salaoReunioes->esquerda = quarto1;
    salaoReunioes->direita = NULL; // Caminho vazio/bloqueado no salão

    quarto1->esquerda = quarto2; // quarto2 é um nó folha (Sem saída)
    quarto1->direita = quarto3;  // quarto3 é um nó folha (Sem saída)

    // 3. Inicialização da raiz nula para a árvore BST de controle do inventário do detetive
    ClueNode *raizPistasColetadas = NULL;

    // 4. Execução do Loop de Exploração e Fase Final do Jogo
    printf("==================================================\n");
    printf("         BEM-VINDO AO DETECTIVE QUEST v1.0        \n");
    printf("==================================================\n");
    printf("Sua missao: Explore a mansao, colete evidencias e\n");
    printf("encontre provas suficientes para condenar o culpado.\n");
    
    explorarSalas(hall, &raizPistasColetadas, &basePistas);
    verificarSuspeitoFinal(raizPistasColetadas, &basePistas);

    // 5. Liberação adequada de toda a memória alocada dinamicamente
    liberarMansao(hall);
    liberarBST(raizPistasColetadas);
    liberarHash(&basePistas);

    return 0;
}