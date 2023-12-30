#ifndef LABIRINTOSO_JOGOUI_H
#define LABIRINTOSO_JOGOUI_H

#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <string.h>
#include <ctype.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <sys/stat.h>

void setThisUser(pUser *, char *);

void setGameInfoFrontend(GameInfoFrontend *);

void desenhaMapa(WINDOW *, int);

void trataTeclado(ThreadDataFrontend *);

int verificaComandoUI(char *, WINDOW *);

bool checkIfUserAtivo(char *);

void *threadGerirBackend(void *);

void fecharCliente(GameInfoFrontend *);

#endif