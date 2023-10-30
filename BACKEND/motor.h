#ifndef LABIRINTOSO_MOTOR_H
#define LABIRINTOSO_MOTOR_H

#include <stdio.h>
#include <stdlib.h>

#include <ncurses.h>

// ESTRUTURAS
//  - Posicao - dados sobre a posição de um elemento
typedef struct Posicao Posicao, *pPosicao;
struct Posicao {
    int x;
    int y;
    pPosicao next;
};
//  - User - dados sobre um utilizador
typedef struct User User, *pUser;
struct User {
    char pid[50];
    char username[TAMANHO_NAMES];
    char identificador;
    pPosicao posicao;
    pUser next;
};
//  - Rock - dados sobre uma pedra
typedef struct Rock Rock, *pRock;
struct Rock {
    char identificador;
    pPosicao posicao;
    int duracao;
    pRock next;
};
//  - Block - dados sobre um bloco
typedef struct Block Block, *pBlock;
struct Block {
    char identificador;
    pPosicao posicao;
    int duracao;
    pBlock next;
};
//  - Wall - dados sobre uma parede
typedef struct Wall Wall, *pWall;
struct Wall {
    char identificador;
    pPosicao posicao;
    pWall next;
};
//  - Mapa - dados sobre o mapa
typedef struct Mapa Mapa, *pMapa;
struct Mapa {
    pPosicao ptrMeta;
    pPosicao ptrInicioHeader;
    //pUser ptrUsersAtivosHeader; // pra fazer get das posições atuais dos users
    //pRock ptrRocksHeader;
    //pBlock ptrBlocksHeader;
    //pWall ptrWallsHeader;
    char mapa[MAPA_LINHAS][MAPA_COLUNAS];
    pMapa next;
};
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
    pMapa ptrMapa;
    int usersAtivos;
    int usersEspera;
    int tempoJogo;
    int nivel;
} GameSetup;

// funções de sistema
void pathParaVariaveisAmbiente();

void setGameSetup(GameSetup *);

void loadMapa(Mapa *, int);

int verificaComando(char *);

#endif
