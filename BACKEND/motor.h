#ifndef LABIRINTOSO_MOTOR_H
#define LABIRINTOSO_MOTOR_H

#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <string.h>
#include <ctype.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <ncurses.h>
#include <time.h>

extern pid_t pidBot;

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

int comandoBots(GameSetup *);

int comandoBmov(GameSetup *);

int comandoRbm();

int comandoBegin();

int comandoEnd();

int comandoKick(GameSetup *, char *);

// comandos do frontend


#endif
