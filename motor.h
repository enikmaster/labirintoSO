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
    struct Posicao *next;
} Posicao, *pPosicao;
//  - User - dados sobre um utilizador
typedef struct {
    char pid[50];
    char username[NAMES];
    char identificador;
    Posicao posicao;
    struct User *next;
} User, *pUser;
//  - Rock - dados sobre uma pedra
typedef struct {
    char identificador;
    Posicao posicao;
    int duracao;
    struct Rock *next;
} Rock, *pRock;
//  - Block - dados sobre um bloco
typedef struct {
    char identificador;
    Posicao posicao;
    int duracao;
    struct Block *next;
} Block, *pBlock;
//  - Wall - dados sobre uma parede
typedef struct Wall Wall, *pWall;
struct Wall {
    char identificador;
    Posicao posicao;
    pWall next;
};
//  - Mapa - dados sobre o mapa
typedef struct {
    pPosicao ptrMeta;
    pPosicao ptrInicioHeader;
    //pUser ptrUsersAtivosHeader; // pra fazer get das posições atuais dos users
    pRock ptrRocksHeader;
    pBlock ptrBlocksHeader;
    pWall ptrWallsHeader;
} Mapa, *pMapa;
//  - Setup - dados sobre a configuração inicial do jogo
typedef struct {
    int inscricao;
    int duracao;
    int decremento;
    int minJogadores;
} Setup, *pSetup;
//  - Mensagem - estrutura de dados a passar a cada utilizador
typedef struct {
    pMapa ptrMapa; // para desenhar o mapa no cliente
    int tempoJogo;
    int nivel;
} Mensagem;
//  - GameSetup - estrutura de dados sobre a configuração do jogo
//  esta informação não passa toda para os clientes
typedef struct {
    //pSetup ptrSetup;
    //pUser ptrUsersAtivosHeader;
    //pUser ptrUsersEsperaHeader;
    //pMapa ptrMapa;
    int usersAtivos;
    int usersEspera;
    int tempoJogo;
    int nivel;
} GameSetup;

// funções de sistema
void setGameSetup(GameSetup *);

void loadMapa(Mapa *, int);

int verificaComando(char *);

#endif
