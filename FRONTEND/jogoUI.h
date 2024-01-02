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

void *threadGerirBackend(void *);

void fecharCliente(GameInfoFrontend *);

int comandoPlayers(ThreadDataFrontend *);

int comandoExit(ThreadDataFrontend *);

int comandoMensagem(ThreadDataFrontend *, char *);

#endif