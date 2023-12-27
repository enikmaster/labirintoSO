#ifndef LABIRINTOSO_MOTOR_H
#define LABIRINTOSO_MOTOR_H

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <signal.h>
#include <string.h>
#include <ctype.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <ncurses.h>

extern pid_t pidBot;

// ESTRUTURAS
typedef enum {
    tipo_inscricao,
    tipo_movimento,
    tipo_informacao,
    tipo_mensagem,
    tipo_terminar,
} TipoFrontEnd;

typedef struct TipoInscricao TipoInscricao;
struct TipoInscricao {
    int pid; // pid do jogador
    char username[TAMANHO_NAMES]; // username do jogador
};

typedef struct TipoMovimento TipoMovimento;
struct TipoMovimento {
    int pid; // pid do jogador
    char movimento; // movimento do jogador (char ou int?)
};

typedef struct TipoInformacao TipoInformacao; // quando o user pede a lista de jogadores ativos
struct TipoInformacao {
    int pid; // pid do jogador
    char lista[500]; // lista de jogadores
};

typedef struct TipoMensagem TipoMensagem;
struct TipoMensagem {
    int pidOrigem; // pid do jogador
    char usernameDestino[TAMANHO_NAMES]; // username do jogador
    char mensagem[TAMANHO_CONTEUDO]; // mensagem do jogador
};

typedef struct TipoTerminar TipoTerminar;
struct TipoTerminar {
    int pid; // pid do jogador
};

typedef struct MsgFrontEnd MsgFrontEnd;
struct MsgFrontEnd {
    TipoFrontEnd tipoMensagem;
    union {
        TipoInscricao inscricao;
        TipoMovimento movimento;
        TipoInformacao informacao;
        TipoMensagem mensagem;
        TipoTerminar terminar;
    } informacao;
};

typedef enum {
    tipo_a,
    tipo_b,
    tipo_c
} TipoBackEnd;

// definir aqui os tipos de mensagem de retorno


/*typedef struct MsgBackEnd MsgBackEnd;
struct MsgBackEnd {
    TipoBackEnd tipoMensagem;
    union {
        TipoA inscricao;
        TipoB movimento;
        TipoC informacao;
    } informacao;
};*/

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
    bool jogoAtivo;
    int usersAtivos;
    int usersEspera;
    int tempoJogo;
    int nivel;
} GameSetup;

typedef struct ThreadData ThreadData, *pThreadData;
struct ThreadData {
    bool continua;
    GameSetup *ptrGameSetup;
    //pthread_mutex_t *pTrinco;
};

// funções de sistema
void pathParaVariaveisAmbiente();

void setGameSetup(GameSetup *);

void loadMapa(GameSetup *, int);

void sinalizaBot(int, siginfo_t *, void *);

int verificaComando(char *);

void fecharJogo(GameSetup *);

void sigHandler(int);

// meta 1
void desenhaMapa(char[MAPA_LINHAS][MAPA_COLUNAS]);

void testarBot();

// threads
void *threadGerirFrontend(void *);

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
