#ifndef LABIRINTOSO_JOGOUI_H
#define LABIRINTOSO_JOGOUI_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include <ncurses.h>

void desenhaMapa(WINDOW *, int);

void trataTeclado(WINDOW *, WINDOW *);

int verificaComandoUI(char *, WINDOW *);

bool checkIfUserAtivo(char *);

#endif