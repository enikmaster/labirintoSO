#ifndef LABIRINTOSO_JOGOUI_H
#define LABIRINTOSO_JOGOUI_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include <ncurses.h>

void desenhaMoldura(int alt, int comp, int linha, int coluna);
void desenhaMapa(WINDOW *janela, int tipo);
void trataTeclado(WINDOW *janelaTopo, WINDOW *janelaBaixo);

int verificaComandoUI(char *comando, WINDOW *janela);
bool checkIfUserAtivo(char *comandoTemp);




#endif
