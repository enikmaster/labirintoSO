#ifndef LABIRINTOSO_CONSTANTES_H
#define LABIRINTOSO_CONSTANTES_H

#define TAMANHO_COMANDO 100
#define TAMANHO_NAMES 50
#define TAMANHO_PATH 8000
#define TAMANHO_CONTEUDO 1000

#define MAX_USERS 5
#define MAX_BOTS 10
#define MAX_ROCKS 50
#define MAX_BLOCKS 5
#define MAX_LEVELS 3

#define MAPA_COLUNAS 40
#define MAPA_LINHAS 16

#define SRV_FIFO "canal"

#include <pthread.h>
#include <stdbool.h>
#include <ncurses.h>

extern char Inscricao[TAMANHO_PATH];
extern char NPlayers[TAMANHO_PATH];
extern char Duracao[TAMANHO_PATH];
extern char Decremento[TAMANHO_PATH];
extern char PathMapaUm[TAMANHO_PATH];
extern char PathMapaDois[TAMANHO_PATH];
extern char PathMapaTres[TAMANHO_PATH];

// Estruturas de dados
typedef enum {
    tipo_inscricao,
    tipo_movimento,
    tipo_informacao,
    tipo_mensagem,
    //tipo_terminar,
    tipo_terminar_programa
} TipoFrontEnd;

typedef struct TipoInscricao TipoInscricao;
struct TipoInscricao {
    pid_t pid; // pid do jogador
    char username[TAMANHO_NAMES]; // username do jogador
};

typedef struct TipoMovimento TipoMovimento;
struct TipoMovimento {
    char username[TAMANHO_NAMES];
    int movimento; // movimento do jogador (int)
};

typedef struct TipoInformacao TipoInformacao; // quando o user pede a lista de jogadores ativos
struct TipoInformacao {
    char username[TAMANHO_NAMES];
    //char lista[500]; // lista de jogadores
};

typedef struct TipoMensagem TipoMensagem;
struct TipoMensagem {
    char username[TAMANHO_NAMES];
    char usernameDestino[TAMANHO_NAMES]; // username do jogador
    char mensagem[TAMANHO_CONTEUDO]; // mensagem do jogador
};

typedef struct TipoTerminar TipoTerminar;
struct TipoTerminar {
    char username[TAMANHO_NAMES];
};

typedef struct TipoTerminarPrograma TipoTerminarPrograma;
struct TipoTerminarPrograma {
    char username[TAMANHO_NAMES]; // nome da origem (neste caso servidor)
};

typedef struct MsgFrontEnd MsgFrontEnd;
struct MsgFrontEnd {
    TipoFrontEnd tipoMensagem;
    union {
        TipoInscricao inscricao;
        TipoMovimento movimento;
        TipoInformacao informacao;
        TipoMensagem mensagem;
        //TipoTerminar terminar;
        TipoTerminarPrograma terminarPrograma;
    } informacao;
};

typedef enum {
    tipo_retorno_inscricao,
    tipo_retorno_players,
    tipo_retorno_chat,
    tipo_retorno_logout,
    tipo_block,
    tipo_atualizar
} TipoBackEnd;

// definir aqui os tipos de mensagem de retorno
typedef struct TipoRetornoInscricao TipoRetornoInscricao;
struct TipoRetornoInscricao {
    char origem[TAMANHO_NAMES];
    char mensagem[TAMANHO_CONTEUDO]; // mensagem do jogador
    //char mapa[MAPA_LINHAS][MAPA_COLUNAS]; // mapa do jogo
};

typedef struct TipoRetornoPlayers TipoRetornoPlayers;
struct TipoRetornoPlayers {
    char origem[TAMANHO_NAMES]; // nome da origem (neste caso servidor)
    char listaJogadores[MAX_USERS][TAMANHO_NAMES]; // lista de jogadores ativos
    char mensagem[TAMANHO_CONTEUDO]; // mensagem do jogador
};

typedef struct TipoRetornoChat TipoRetornoChat;
struct TipoRetornoChat {
    char origem[TAMANHO_NAMES]; // nome da origem (neste caso servidor)
    char destino[TAMANHO_NAMES]; // mensagem do jogador
    char mensagem[TAMANHO_CONTEUDO]; // mensagem do jogador
};

typedef struct TipoRetornoLogout TipoRetornoLogout;
struct TipoRetornoLogout {
    char username[TAMANHO_NAMES];
};


typedef struct TipoBlock TipoBlock;
struct TipoBlock {
    char origem[TAMANHO_NAMES]; // nome da origem (neste caso servidor)
    int x;
    int y;
};

typedef struct TipoAtualizar TipoAtualizar;
struct TipoAtualizar {
    char origem[TAMANHO_NAMES]; // nome da origem (neste caso servidor)
    char mapa[MAPA_LINHAS][MAPA_COLUNAS]; // mapa do jogo
    char mensagem[TAMANHO_CONTEUDO]; // mensagem do jogador
};

typedef struct MsgBackEnd MsgBackEnd;
struct MsgBackEnd {
    TipoBackEnd tipoMensagem;
    union {
        TipoRetornoInscricao retornoInscricao;
        TipoRetornoPlayers retornoPlayers;
        TipoRetornoChat retornoChat;
        TipoRetornoLogout retornoLogout;
        TipoBlock block;
        TipoTerminarPrograma terminarPrograma;
    } informacao;
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
typedef struct UserInfo UserInfo, *pUserInfo;
struct UserInfo {
    char identificador;
    pPosition position;
    pUserInfo next;
};

typedef struct User User, *pUser;
struct User {
    pid_t pid;
    char username[TAMANHO_NAMES];
    pUserInfo ptrUserInfo;
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
    pBlock next;
};
//  - Mapa - dados sobre o mapa
typedef struct Map Map, *pMap;
struct Map {
    pPosition ptrMeta;
    pPosition ptrInicioHeader;
    pRock ptrRocksHeader;
    pBlock ptrBlocksHeader;
    char mapa[MAPA_LINHAS][MAPA_COLUNAS];
    pMap next;
};
//  - Bot - dados sobre um bot
typedef struct Bot Bot, *pBot;
struct Bot {
    pid_t pid;
    pPosition ptrPosition;
    int duracao;
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
    pMap ptrMapa;
    pBot ptrBotsHeader; // TODO: ainda falta ver isto
    bool jogoAtivo;
    int usersAtivos;
    int usersEspera;
    int tempoJogo;
    int nivel;
    pthread_mutex_t mutexJogadores;
    pthread_mutex_t mutexMapa;
    pthread_mutex_t mutexBots;
    pthread_mutex_t mutexGeral;
} GameSetup;

typedef struct GameInfoFrontend GameInfoFrontend, *pGameInfoFrontend;
struct GameInfoFrontend {
    char mapa[MAPA_LINHAS][MAPA_COLUNAS];
    pRock ptrRocksHeader;
    pBlock ptrBlocksHeader;
    pUser ptrThisUser;
    pUserInfo ptrOtherUsersHeader;
    int tempoJogo;
    int nivel;
};

typedef struct ThreadDataFrontend ThreadDataFrontend, *pThreadDataFrontend;
struct ThreadDataFrontend {
    bool continua;
    WINDOW *janelaMapa;
    WINDOW *janelaChat;
    WINDOW *janelaLogs;
    WINDOW *janelaComandos;
    GameInfoFrontend *ptrGameInfo;
    pthread_mutex_t trinco;
};

typedef struct ThreadData ThreadData, *pThreadData;
struct ThreadData {
    bool continua;
    GameSetup *ptrGameSetup;
    pthread_mutex_t trinco;
};

#endif