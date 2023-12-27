#include "../constantes.h"
#include "motor.h"
#include <pthread.h>

void *threadCountdownToStart(void *arg) {
    //ThreadData *tData = (ThreadData *) arg;
    int countdown = atoi(Inscricao);
    do {
        if (countdown <= 5) {
            printf("[INFO] O jogo vai começar em %d segundos\n", countdown--);
            fflush(stdout);
        }
        sleep(1); // ver função clock_nanosleep() para ter mais pontos
    } while (countdown > 0);
    pthread_exit(NULL);
}

void *threadGerirFrontend(void *arg) {
    ThreadData *tData = (ThreadData *) arg;
    bool startTimer = false;
    int serverPipe = open(SRV_FIFO, O_RDONLY);// pipe do servidor
    if (serverPipe == -1) {
        perror("[ERRO] Erro ao abrir o pipe do servidor.\n");
        sigqueue(getpid(), SIGUSR2, (const union sigval) 0);
        unlink(SRV_FIFO);
        pthread_exit(NULL);
    }
    MsgFrontEnd msgFrontEnd;
    do {
        memset(&msgFrontEnd, 0, sizeof(msgFrontEnd));
        ssize_t bytesRead = read(serverPipe, &msgFrontEnd, sizeof(msgFrontEnd));
        if (bytesRead == 0) continue;
        if (bytesRead == -1) {
            perror("[ERRO] Erro ao ler do pipe do servidor.\n");
            continue;
        }
        switch (msgFrontEnd.tipoMensagem) {
            case tipo_inscricao:
                if (tData->ptrGameSetup->usersAtivos < MAX_USERS) { // ainda há espaço para inscrições
                    char pipeName[TAMANHO_NAMES];
                    sprintf(pipeName, "cli%d", msgFrontEnd.informacao.inscricao.pid);
                    int pipeJogador = open(pipeName, O_WRONLY);
                    if (pipeJogador == -1) {
                        perror("[ERRO] Erro ao abrir o pipe do jogador.\n");
                        continue;
                    }
                    pUser thisUser = tData->ptrGameSetup->ptrUsersAtivosHeader;
                    for (int i = 0; i < tData->ptrGameSetup->usersAtivos; ++i) {
                        thisUser = thisUser->next;
                    }
                    pUser newUser = malloc(sizeof(User));
                    if (newUser == NULL) {
                        perror("[ERRO] Erro ao alocar memória para o novo user.\n");
                        free(newUser);
                        continue;
                    }
                    newUser->pid = msgFrontEnd.informacao.inscricao.pid;
                    strcpy(newUser->username, msgFrontEnd.informacao.inscricao.username);
                    newUser->position = malloc(sizeof(Position));
                    if (newUser->position == NULL) {
                        perror("[ERRO] Erro ao alocar memória para a posição do novo user.\n");
                        free(newUser->position);
                        free(newUser);
                        continue;
                    }
                    pthread_mutex_lock(&tData->ptrGameSetup->mutexJogadores);
                    newUser->identificador = toupper(newUser->username[0]);
                    newUser->position->x = 0;
                    newUser->position->y = 0;
                    newUser->next = NULL;
                    thisUser->next = newUser;
                    tData->ptrGameSetup->usersAtivos++;
                    pthread_mutex_unlock(&tData->ptrGameSetup->mutexJogadores);
                    MsgBackEnd msgBackEnd;
                    msgBackEnd.tipoMensagem = tipo_retorno_inscricao;
                    strcpy(msgBackEnd.informacao.retornoInscricao.origem, "Servidor");
                    strcpy(msgBackEnd.informacao.retornoInscricao.mensagem, "Inscrição aceite.");
                    if (write(pipeJogador, &msgBackEnd, sizeof(msgBackEnd)) == -1) {
                        perror("[ERRO] Erro ao escrever no pipe do jogador.\n");
                        close(pipeJogador);
                        continue;
                    }
                    close(pipeJogador);
                } else { // não há espaço para inscrições, verifica lista de espera
                    if (tData->ptrGameSetup->usersEspera < MAX_USERS) {
                        char pipeName[TAMANHO_NAMES];
                        sprintf(pipeName, "cli%d", msgFrontEnd.informacao.inscricao.pid);
                        int pipeJogador = open(pipeName, O_WRONLY);
                        if (pipeJogador == -1) {
                            perror("[ERRO] Erro ao abrir o pipe do jogador.\n");
                            continue;
                        }
                        pUser thisUser = tData->ptrGameSetup->ptrUsersEsperaHeader;
                        for (int i = 0; i < tData->ptrGameSetup->usersEspera; ++i) {
                            thisUser = thisUser->next;
                        }
                        pUser newUser = malloc(sizeof(User));
                        if (newUser == NULL) {
                            perror("[ERRO] Erro ao alocar memória para o novo user.\n");
                            free(newUser);
                            continue;
                        }
                        newUser->pid = msgFrontEnd.informacao.inscricao.pid;
                        strcpy(newUser->username, msgFrontEnd.informacao.inscricao.username);
                        newUser->position = malloc(sizeof(Position));
                        if (newUser->position == NULL) {
                            perror("[ERRO] Erro ao alocar memória para a posição do novo user.\n");
                            free(newUser->position);
                            free(newUser);
                            continue;
                        }
                        pthread_mutex_lock(&tData->ptrGameSetup->mutexJogadores);
                        newUser->identificador = toupper(newUser->username[0]);
                        newUser->position->x = 0;
                        newUser->position->y = 0;
                        newUser->next = NULL;
                        thisUser->next = newUser;
                        tData->ptrGameSetup->usersEspera++;
                        pthread_mutex_unlock(&tData->ptrGameSetup->mutexJogadores);
                        MsgBackEnd msgBackEnd;
                        msgBackEnd.tipoMensagem = tipo_retorno_inscricao;
                        strcpy(msgBackEnd.informacao.retornoInscricao.origem, "Servidor");
                        strcpy(msgBackEnd.informacao.retornoInscricao.mensagem, "Inscrição em lista de espera.");
                        if (write(pipeJogador, &msgBackEnd, sizeof(msgBackEnd)) == -1) {
                            perror("[ERRO] Erro ao escrever no pipe do jogador.\n");
                            close(pipeJogador);
                            continue;
                        }
                        close(pipeJogador);
                    } else { // não há espaço em nenhuma lista
                        char pipeName[TAMANHO_NAMES];
                        sprintf(pipeName, "cli%d", msgFrontEnd.informacao.inscricao.pid);
                        int pipeJogador = open(pipeName, O_WRONLY);
                        if (pipeJogador == -1) {
                            perror("[ERRO] Erro ao abrir o pipe do jogador.\n");
                            continue;
                        }
                        MsgBackEnd msgBackEnd;
                        msgBackEnd.tipoMensagem = tipo_retorno_inscricao;
                        strcpy(msgBackEnd.informacao.retornoInscricao.origem, "Servidor");
                        strcpy(msgBackEnd.informacao.retornoInscricao.mensagem,
                               "Inscrição recusada. Listas cheias");
                        if (write(pipeJogador, &msgBackEnd, sizeof(msgBackEnd)) == -1) {
                            perror("[ERRO] Erro ao escrever no pipe do jogador.\n");
                            close(pipeJogador);
                            continue;
                        }
                        close(pipeJogador);
                    }
                }
                break;
            case tipo_movimento:
                break;
            case tipo_informacao:
                break;
            case tipo_mensagem:
                break;
            case tipo_terminar:
                break;
        }
    } while (!tData->continua);
    close(serverPipe);
    pthread_exit(NULL);
}

void *threadTimers(void *arg) {
    ThreadData *tData = (ThreadData *) arg;
    do {

        //decrementaUmSegundo();
        sleep(1);
    } while (tData->continua == false);
    pthread_exit(NULL);
}

// TODO: uma função para enviar uma mensagem a todos os clientes com as informações do jogo

// TODO: uma função para enviar uma mendagem a todos os clientes com o mapa atualizado

// TODO: 3 Threads
//  1 - thread para lidar com coisas temporais (tempo de jogo, tempo de inscrição, tempo de decremento, etc)
//  2 - thread para lidar com os comandos dos clientes (jogadores): quando um user faz um movimento, a posição do user é atualizada no mapa e é enviado a todos os clientes
//  3 - thread para lidar com os comandos do servidor (administração do jogo)