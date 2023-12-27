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
    do {
        int fd = open(SRV_FIFO, O_RDONLY);// pipe do servidor
        if (fd == -1) {
            perror("[ERRO] Erro ao abrir o pipe do servidor.\n");
            sigqueue(getpid(), SIGUSR2, (const union sigval) 0);
            unlink(SRV_FIFO);
            pthread_exit(NULL);
        }
        MsgFrontEnd msgFrontEnd;
        do {
            memset(&msgFrontEnd, 0, sizeof(msgFrontEnd));
            ssize_t bytesRead = read(fd, &msgFrontEnd, sizeof(msgFrontEnd));
            if (bytesRead == 0) continue;
            if (bytesRead == -1) {
                perror("[ERRO] Erro ao ler do pipe do servidor.\n");
                continue;
            }
            int nAux = 0; // a descobrir para que serve
            switch (msgFrontEnd.tipoMensagem) {
                case tipo_inscricao:
                    if (tData->ptrGameSetup->usersAtivos < MAX_USERS) {
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
                        newUser->identificador = toupper(newUser->username[0]);
                        newUser->position->x = 0;
                        newUser->position->y = 0;
                        newUser->next = NULL;
                        thisUser->next = newUser;
                        tData->ptrGameSetup->usersAtivos++;
                        // TODO: falta terminar isto depois de ter uma mensagem de retorno
                        write(pipeJogador, &msgFrontEnd, sizeof(msgFrontEnd));
                        close(pipeJogador);

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
        // verificar se a lista permite mais inscritos
        if (tData->ptrGameSetup->usersAtivos < MAX_USERS) {
            pUser thisUser = tData->ptrGameSetup->ptrUsersAtivosHeader;
            for (int i = 0; i < tData->ptrGameSetup->usersAtivos; ++i) {
                thisUser = thisUser->next;
            }
            pUser newUser = malloc(sizeof(User));

        }
        if (tData->jogadoresAtivos[MAX_USERS - 1] == -1) { // não está cheio
            // adiciona o pid ao array jogadoresAtivos na primeira posição que encontrar com o -1 (vazia)
            for (int i = 0; i < MAX_USERS; ++i) {
                if (tData->jogadoresAtivos[i] == -1) {
                    tData->jogadoresAtivos[i] = pid; // lidar com isto
                    tData->usernamesAtivos[i] = username; // lidar com isto
                    if (++nJogadoresAtivos >= atoi(NPlayers) && startTimer == false) {
                        startTimer = true;
                        pthread_t threadCountdownToStartId;
                        if (pthread_create(&threadCountdownToStartId, NULL, threadCountdownToStart, (void *) &tData) !=
                            0) {
                            perror("[ERRO] Erro ao criar a thread do countdown.\n");
                            exit(-1);
                        }
                    }
                    break;
                }
            }
        } else if (tData->jogadoresEspera[MAX_USERS - 1] == -1) { // não está cheio
            // adiciona o pid ao array jogadoresEspera na primeira posição que encontrar com o -1 (vazia)
            printf("[INFO] O jogo está cheio.\n");
            fflush(stdout);
            for (int i = 0; i < MAX_USERS; ++i) {
                if (tData->jogadoresEspera[i] == -1) {
                    //tData->jogadoresEspera[i] = pid; // lidar com isto
                    //tData->usernamesEspera[i] = username; // lidar com isto
                    break;
                }
            }
        } else {
            printf("[INFO] O jogo e a lista de espera estão cheias.\n");
            fflush(stdout);
            // enviar uma mensagem ao user?
        }

        // TODO: quando receber o sinal para encerrar, mudar o tData->continua para true
    } while (tData->continua == false);
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