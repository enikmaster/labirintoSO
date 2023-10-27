#ifndef LABIRINTOSO_MOTOR_H
#define LABIRINTOSO_MOTOR_H

#include <stdio.h>
#include <stdlib.h>

#include <ncurses.h>

// ESTRUTURAS
//  - Posicao - dados sobre a posição de um elemento
typedef struct {
    int x;
    int y;
} Posicao;
//  - User - dados sobre um utilizador
typedef struct {
    char pid[50];
    char username[TAMANHO_NAMES];
    char identificador;
    Posicao posicao;
    struct User *next;
} User;
//  - Rock - dados sobre uma pedra
typedef struct {
    char identificador;
    Posicao posicao;
    int duracao;
    struct Rock *next;
} Rock;
//  - Block - dados sobre um bloco
typedef struct {
    char identificador;
    Posicao posicao;
    int duracao;
    struct Block *next;
} Block;
//  - Wall - dados sobre uma parede
typedef struct {
    char identificador;
    Posicao posicao;
    struct Wall *next;
} Wall;
//  - Mapa - dados sobre o mapa
typedef struct {
    Posicao meta;
    Posicao *inicioHeader;
    User *usersAtivosHeader; // pra fazer get das posições atuais dos users
    Rock *rocksHeader;
    Block *blocksHeader;
    Wall *wallsHeader;
} Mapa;
//  - Tempo - dados sobre o tempo e que é definido pelo administrador num ficheiro
typedef struct {
    int inscricao;
    int duracao;
    int decremento;
} Tempo;
//  - Mensagem - estrutura de dados a passar a cada utilizador
typedef struct {
    Mapa mapa; // para desenhar o mapa no cliente
    int tempoJogo;
    int nivel;
} Mensagem;
//  - GameSetup - estrutura de dados sobre a configuração do jogo
//  esta informação não passa toda para os clientes
typedef struct {
    User *usersAtivosHeader;
    User *usersEsperaHeader;
    Mapa mapa;
    Tempo tempo;
    int usersAtivos;
    int usersEspera;
    int tempoJogo;
    int nivel;
} GameSetup;

void definirTempoInicial(Tempo *);

int verificaComando(char *);

#endif
