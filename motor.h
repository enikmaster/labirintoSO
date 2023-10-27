#ifndef LABIRINTOSO_MOTOR_H
#define LABIRINTOSO_MOTOR_H

#include <stdio.h>
#include <stdlib.h>

#include <ncurses.h>

// estruturas
typedef struct {
    int x;
    int y;
} Posicao;
typedef struct {
    char pid[50];
    char username[TAMANHO_NAMES];
    char id;
    Posicao posicao;
    struct User *next;
} User;
typedef struct {
    char id;
    Posicao posicao;
    struct Rock *next;
} Rock;
typedef struct {
    char id;
    Posicao posicao;
    struct Block *next;
} Block;
typedef struct {
    char id;
    Posicao posicao;
    struct Wall *next;
} Wall;
typedef struct {
    Posicao meta;
    Posicao *inicioHeader;
    User *usersHeader; // pra fazer get das posições atuais dos users
    Rock *rocksHeader;
    Block *blocksHeader;
    Wall *wallsHeader;
} Mapa;
typedef struct {
    int inscricao;
    int duracao;
    int decremento;
} Tempo;
typedef struct {
    User *userHeader;
    Mapa mapa;
    Tempo tempo;
    int usersLogados;
    int usersEspera;
    int nivel;
    int tempoJogo;
} Estruturas;

int verificaComando(char *comando);

#endif
