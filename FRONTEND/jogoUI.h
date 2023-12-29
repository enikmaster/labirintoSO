#ifndef LABIRINTOSO_JOGOUI_H
#define LABIRINTOSO_JOGOUI_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <unistd.h>
#include <pthread.h>
#include <ncurses.h>

void setGameInfoFrontend(GameInfoFrontend *);

void desenhaMapa(WINDOW *, int);

void trataTeclado(WINDOW *, WINDOW *);

int verificaComandoUI(char *, WINDOW *);

bool checkIfUserAtivo(char *);

void *threadGerirBackend(void *);

void fecharCliente(GameInfoFrontend *);

#endif