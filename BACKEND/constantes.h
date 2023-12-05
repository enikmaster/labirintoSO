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

#define CHAR_ROCK 'R'
#define CHAR_BLOCK 'B'
#define CHAR_WALL 'X'

extern char Inscricao[TAMANHO_PATH];
extern char NPlayers[TAMANHO_PATH];
extern char Duracao[TAMANHO_PATH];
extern char Decremento[TAMANHO_PATH];
extern char PathMapaUm[TAMANHO_PATH];
extern char PathMapaDois[TAMANHO_PATH];
extern char PathMapaTres[TAMANHO_PATH];

#endif