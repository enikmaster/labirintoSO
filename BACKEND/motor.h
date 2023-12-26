#ifndef LABIRINTOSO_MOTOR_H
#define LABIRINTOSO_MOTOR_H

#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <ncurses.h>
#include <string.h>
#include <ctype.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>

extern pid_t pidBot;

// ESTRUTURAS
typedef struct ThreadData ThreadData, *pThreadData;
struct ThreadData {
    int continua;
    pthread_mutex_t *pTrinco;
};
//  - Mensagem - estrutura de dados a passar a cada utilizador
typedef struct {
    char mapa[MAPA_LINHAS][MAPA_COLUNAS]; // para desenhar o mapa no cliente
    int tempoJogo;
    int nivel;
} Mensagem;
//  - Position - dados sobre a posição de um elemento
typedef struct Position Position, *pPosition;
struct Position {
    int x;
    int y;
    pPosition next;
};
//  - User - dados sobre um utilizador
typedef struct User User, *pUser;
struct User {
    int pid;
    char username[TAMANHO_NAMES];
    char identificador;
    pPosition position;
    pUser next;
};
//  - Rock - dados sobre uma pedra
typedef struct Rock Rock, *pRock;
struct Rock {
    char identificador;
    pPosition position;
    int duracao;
    pRock next;
};
//  - Block - dados sobre um bloco
typedef struct Block Block, *pBlock;
struct Block {
    char identificador;
    pPosition position;
    int duracao;
    pBlock next;
};
//  - Mapa - dados sobre o mapa
typedef struct Map Map, *pMap;
struct Map {
    pPosition ptrMeta;
    pPosition ptrInicioHeader;
    //pRock ptrRocksHeader;
    //pBlock ptrBlocksHeader;
    char mapa[MAPA_LINHAS][MAPA_COLUNAS];
    pMap next;
};
//  - Bot - dados sobre um bot
typedef struct Bot Bot, *pBot;
struct Bot {
    int pid;
    pBot next;
};
//  - Setup - dados sobre a configuração inicial do jogo
typedef struct Setup Setup, *pSetup;
struct Setup {
    long int inscricao;
    long int duracao;
    long int decremento;
    long int minJogadores;
};
//  - GameSetup - estrutura de dados sobre a configuração do jogo
//  esta informação não passa toda para os clientes
typedef struct {
    pSetup ptrSetup;
    pUser ptrUsersAtivosHeader;
    pUser ptrUsersEsperaHeader;
    //pBot ptrBotsHeader;
    pMap ptrMapa;
    int usersAtivos;
    int usersEspera;
    int tempoJogo;
    int nivel;
} GameSetup;

// funções de sistema
void pathParaVariaveisAmbiente();

void setGameSetup(GameSetup *);

void loadMapa(GameSetup *, int);

void sinalizaBot(int, siginfo_t *, void *);

int verificaComando(char *);

void fecharJogo(GameSetup *);

// meta 1
void desenhaMapa(char[MAPA_LINHAS][MAPA_COLUNAS]);

void testarBot();

// threads
void *threadTimers(void *);

// comandos do backend
int comandoUsers(GameSetup *);

int comandoBots();

int comandoBmov();

int comandoRbm();

int comandoBegin();

int comandoEnd();

int comandoKick(GameSetup *, char *);

// comandos do frontend


#endif
