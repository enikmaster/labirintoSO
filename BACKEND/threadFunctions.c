#include "../constantes.h"
#include "motor.h"

// cenas do timer

long int setTempoJogo(ThreadData *tData) {
    long int tempoJogo;
    if (tData->ptrGameSetup->nivel == 1) {
        tempoJogo = tData->ptrGameSetup->ptrSetup->duracao;
    } else {
        long int nivel = tData->ptrGameSetup->nivel;
        tempoJogo = tData->ptrGameSetup->ptrSetup->duracao - (tData->ptrGameSetup->ptrSetup->decremento * --nivel);
    }
    return tempoJogo;
}

void begin(ThreadData *tData) {
    pUser ptrUser = tData->ptrGameSetup->ptrUsersAtivosHeader;
    MsgBackEnd msgBackEnd;
    msgBackEnd.tipoMensagem = tipo_start_game;
    for (int y = 0; y < MAPA_LINHAS; ++y) {
        for (int x = 0; x < MAPA_COLUNAS; ++x) {
            msgBackEnd.informacao.startGame.mapa[y][x] = tData->ptrGameSetup->ptrMapa->mapa[y][x];
        }
    }
    msgBackEnd.informacao.startGame.tempoJogo = tData->ptrGameSetup->ptrSetup->duracao;
    msgBackEnd.informacao.startGame.nivel = tData->ptrGameSetup->nivel;
    while (ptrUser != NULL) {
        int pipeJogador = open(ptrUser->username, O_WRONLY);
        if (pipeJogador == -1) {
            perror("[ERRO] Erro ao abrir o pipe do jogador.\n");
            continue;
        }
        if (write(pipeJogador, &msgBackEnd, sizeof(msgBackEnd)) == -1) {
            perror("[ERRO] Erro ao escrever no pipe do jogador.\n");
            close(pipeJogador);
            continue;
        }
        close(pipeJogador);
        ptrUser = ptrUser->next;
    }
}

void setPosicaoStart(ThreadData *tData) {
    MsgBackEnd msgBackEnd;
    int i = 0;
    pUser ptrUser = tData->ptrGameSetup->ptrUsersAtivosHeader;
    pPosition ptrPosition = tData->ptrGameSetup->ptrMapa->ptrInicioHeader;
    while (ptrUser != NULL) {
        // atribuir posições ao user
        ptrUser->ptrUserInfo->position->x = ptrPosition->x;
        ptrUser->ptrUserInfo->position->y = ptrPosition->y;
        // preencher a mensagem
        msgBackEnd.tipoMensagem = tipo_posicoes_iniciais;
        strcpy(msgBackEnd.informacao.posicoesIniciais.username[i], ptrUser->username);
        msgBackEnd.informacao.posicoesIniciais.x[i] = ptrPosition->x;
        msgBackEnd.informacao.posicoesIniciais.y[i] = ptrPosition->y;
        // passar ao próximo user
        ptrUser = ptrUser->next;
        ptrPosition = ptrPosition->next;
        ++i;
    }
    ptrUser = tData->ptrGameSetup->ptrUsersAtivosHeader;
    while (ptrUser != NULL) {
        int pipeJogador = open(ptrUser->username, O_WRONLY);
        if (pipeJogador == -1) {
            perror("[ERRO] Erro ao abrir o pipe do jogador.\n");
            continue;
        }
        if (write(pipeJogador, &msgBackEnd, sizeof(msgBackEnd)) == -1) {
            perror("[ERRO] Erro ao escrever no pipe do jogador.\n");
            close(pipeJogador);
            continue;
        }
        close(pipeJogador);
    }
}

void *threadTimers(void *arg) {
    ThreadData *tData = (ThreadData *) arg;
    long int tempoInscricao = tData->ptrGameSetup->ptrSetup->inscricao;
    long int tempoJogo;

    do {
        // verificar se o jogo está ativo
        if (!tData->ptrGameSetup->jogoAtivo) { // jogo ainda não começou (inscrições)
            pthread_mutex_lock(&tData->ptrGameSetup->mutexJogadores);
            if (tData->ptrGameSetup->usersAtivos >= tData->ptrGameSetup->ptrSetup->minJogadores) {
                printf("[INFO] O jogo vai começar em %ld segundos.\n", tempoInscricao);
                fflush(stdout);
                if (--tempoInscricao <= 0 || tData->ptrGameSetup->usersAtivos >= MAX_USERS) {
                    tData->ptrGameSetup->jogoAtivo = true;
                    tempoJogo = setTempoJogo(tData);
                    begin(tData);
                    setPosicaoStart(tData);
                    if (tData->ptrGameSetup->ptrMapa->ptrBlocksHeader != NULL) {
                        // TODO: enviar mensagem com os blocks, para todos os users
                    }
                    if (tData->ptrGameSetup->ptrMapa->ptrRocksHeader != NULL) {
                        // TODO: enviar mensagem com os rocks, para todos os users
                    }
                }
            }
            pthread_mutex_unlock(&tData->ptrGameSetup->mutexJogadores);
        } else { // jogo a decorrer
            tempoJogo--;
            // verificar se o tempo de jogo chegou ao fim
            if (tempoJogo <= 0) { // é para terminar / passar de nível

                // verificar o nivel
                tData->ptrGameSetup->jogoAtivo = false;
                // enviar mensagem para os clientes
                // enviar mensagem para o servidor
                // enviar mensagem para o motor
            } else { // é paar rodar um turno

            }

            // verificar se existe um vencedor
            // mexer um block
            //decrementaUmSegundo();
        }


        sleep(1);
    } while (tData->continua == false);
    pthread_exit(NULL);
}

void *threadGerirFrontend(void *arg) {
    ThreadData *tData = (ThreadData *) arg;
    bool checkMsg = false;
    bool startTimer = false;
    int serverPipe = 0;
    MsgFrontEnd msgFrontEnd;
    do {
        serverPipe = open(SRV_FIFO, O_RDONLY);// pipe do servidor
        if (serverPipe == -1) {
            perror("[ERRO] Erro ao abrir o pipe do servidor.\n");
            sigqueue(getpid(), SIGUSR2, (const union sigval) 0);
            unlink(SRV_FIFO);
            pthread_exit(NULL);
        }
        memset(&msgFrontEnd, 0, sizeof(msgFrontEnd));
        ssize_t bytesRead = read(serverPipe, &msgFrontEnd, sizeof(msgFrontEnd));
        if (bytesRead == 0) continue;
        if (bytesRead == -1) {
            perror("[ERRO] Erro ao ler do pipe do servidor.\n");
            continue;
        }
        switch (msgFrontEnd.tipoMensagem) {
            case tipo_inscricao: // os jogadores inscrevem-se no jogo
                if (tData->ptrGameSetup->usersAtivos < MAX_USERS) { // ainda há espaço para inscrições
                    pUser thisUser = tData->ptrGameSetup->ptrUsersAtivosHeader;
                    while (thisUser != NULL && thisUser->next != NULL) {
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
                    pUserInfo newUserInfo = malloc(sizeof(UserInfo));
                    if (newUserInfo == NULL) {
                        perror("[ERRO] Erro ao alocar memória para o novo user info.\n");
                        free(newUser);
                        continue;
                    }
                    newUser->ptrUserInfo = newUserInfo;
                    pPosition newPosition = malloc(sizeof(Position));
                    if (newPosition == NULL) {
                        perror("[ERRO] Erro ao alocar memória para a nova posição.\n");
                        free(newUser);
                        free(newUserInfo);
                        continue;
                    }
                    newUser->ptrUserInfo->position = newPosition;
                    pthread_mutex_lock(&tData->ptrGameSetup->mutexJogadores);
                    newUser->ptrUserInfo->identificador = toupper(newUser->username[0]);
                    strcpy(newUser->ptrUserInfo->username, newUser->username);
                    newUser->ptrUserInfo->position->x = 0;
                    newUser->ptrUserInfo->position->y = 0;
                    newUser->ptrUserInfo->position->next = NULL;
                    newUser->ptrUserInfo->next = NULL;
                    newUser->next = NULL;
                    (thisUser == NULL) ? (tData->ptrGameSetup->ptrUsersAtivosHeader = newUser)
                                       : (thisUser->next = newUser);
                    tData->ptrGameSetup->usersAtivos++;
                    pthread_mutex_unlock(&tData->ptrGameSetup->mutexJogadores);
                    int pipeJogador = open(msgFrontEnd.informacao.inscricao.username, O_WRONLY);
                    if (pipeJogador == -1) {
                        perror("[ERRO] Erro ao abrir o pipe do jogador.\n");
                        continue;
                    }
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
                    printf("\n[INFO] %s inscreveu-se no jogo.\n", msgFrontEnd.informacao.inscricao.username);
                    fflush(stdout);
                } else { // não há espaço para inscrições, verifica lista de espera
                    if (tData->ptrGameSetup->usersEspera < MAX_USERS) {
                        pUser thisUser = tData->ptrGameSetup->ptrUsersEsperaHeader;
                        while (thisUser != NULL && thisUser->next != NULL) {
                            thisUser = thisUser->next;
                        }
                        pUser newUser = malloc(sizeof(User));
                        if (newUser == NULL) {
                            perror("[ERRO] Erro ao alocar memória para o novo user.\n");
                            continue;
                        }
                        newUser->pid = msgFrontEnd.informacao.inscricao.pid;
                        strcpy(newUser->username, msgFrontEnd.informacao.inscricao.username);
                        pUserInfo newUserInfo = malloc(sizeof(UserInfo));
                        if (newUserInfo == NULL) {
                            perror("[ERRO] Erro ao alocar memória para o novo user info.\n");
                            free(newUser);
                            continue;
                        }
                        newUser->ptrUserInfo = newUserInfo;
                        pPosition newPosition = malloc(sizeof(Position));
                        if (newPosition == NULL) {
                            perror("[ERRO] Erro ao alocar memória para a nova posição.\n");
                            free(newUser);
                            free(newUserInfo);
                            continue;
                        }
                        newUser->ptrUserInfo->position = newPosition;
                        pthread_mutex_lock(&tData->ptrGameSetup->mutexJogadores);
                        newUser->ptrUserInfo->identificador = toupper(newUser->username[0]);
                        newUser->ptrUserInfo->position->x = 0;
                        newUser->ptrUserInfo->position->y = 0;
                        newUser->ptrUserInfo->position->next = NULL;
                        newUser->ptrUserInfo->next = NULL;
                        newUser->next = NULL;
                        (thisUser == NULL) ? (tData->ptrGameSetup->ptrUsersEsperaHeader = newUser)
                                           : (thisUser->next = newUser);
                        tData->ptrGameSetup->usersEspera++;
                        pthread_mutex_unlock(&tData->ptrGameSetup->mutexJogadores);
                        int pipeJogador = open(msgFrontEnd.informacao.inscricao.username, O_WRONLY);
                        if (pipeJogador == -1) {
                            perror("[ERRO] Erro ao abrir o pipe do jogador.\n");
                            continue;
                        }
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
                        printf("\n[INFO] %s em lista de espera.\n", msgFrontEnd.informacao.inscricao.username);
                        fflush(stdout);
                    } else { // não há espaço em nenhuma lista
                        int pipeJogador = open(msgFrontEnd.informacao.inscricao.username, O_WRONLY);
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
                        printf("\n[INFO] %s tentou inscrever-se mas as listas estão cheias.\n",
                               msgFrontEnd.informacao.inscricao.username);
                        fflush(stdout);
                    }
                }

                break;
            case tipo_movimento:
                break;
            case tipo_informacao:
                // le pedido do cliente e processa-o
                if (true) {

                    int pipeJogador = open(msgFrontEnd.informacao.informacao.username, O_WRONLY);
                    if (pipeJogador == -1) {
                        perror("[ERRO] Erro ao abrir o pipe do jogador.\n");
                        continue;
                    }
                    // get lista de users ativos
                    // meter isso no array no tipoMensagem
                    // enviar para o jogador

                    MsgBackEnd msgBackEnd;
                    msgBackEnd.tipoMensagem = tipo_retorno_players;
                    strcpy(msgBackEnd.informacao.retornoPlayers.origem, "Servidor");
                    strcpy(msgBackEnd.informacao.retornoPlayers.mensagem, "Lista de jogadores ativos: ");

                    pUser ptrUser = tData->ptrGameSetup->ptrUsersAtivosHeader;
                    // enche o array com os nomes
                    for (int i = 0; i < tData->ptrGameSetup->usersAtivos; ++i) {
                        strcpy(msgBackEnd.informacao.retornoPlayers.listaJogadores[i], ptrUser->username);
                        ptrUser = ptrUser->next;
                    }

                    // envia a mensagem para o jogador
                    if (write(pipeJogador, &msgBackEnd, sizeof(msgBackEnd)) == -1) {
                        perror("[ERRO] Erro ao escrever no pipe do jogador.\n");
                        close(pipeJogador);
                        continue;
                    }
                    close(pipeJogador);
                }

                break;
            case tipo_mensagem:
                if (true) {
                    pUser ptrUser = tData->ptrGameSetup->ptrUsersAtivosHeader;
                    while (ptrUser != NULL) {
                        if (strcmp(ptrUser->username,
                                   msgFrontEnd.informacao.mensagem.usernameDestino) == 0) {
                            checkMsg = true;
                        }
                        ptrUser = ptrUser->next;
                    }
                    if (checkMsg) {
                        int pipeJogadorOrigem = open(msgFrontEnd.informacao.mensagem.username, O_WRONLY);
                        if (pipeJogadorOrigem == -1) {
                            perror("[ERRO] Erro ao abrir o pipe do jogador.\n");
                            continue;
                        }
                        int pipeJogadorDestino = open(msgFrontEnd.informacao.mensagem.usernameDestino, O_WRONLY);
                        if (pipeJogadorDestino == -1) {
                            perror("[ERRO] Erro ao abrir o pipe do jogador.\n");
                            continue;
                        }
                        MsgBackEnd msgBackEnd;
                        msgBackEnd.tipoMensagem = tipo_retorno_chat;
                        strcpy(msgBackEnd.informacao.retornoChat.origem, msgFrontEnd.informacao.mensagem.username);
                        strcpy(msgBackEnd.informacao.retornoChat.mensagem, msgFrontEnd.informacao.mensagem.mensagem);
                        if (write(pipeJogadorOrigem, &msgBackEnd, sizeof(msgBackEnd)) == -1) {
                            perror("[ERRO] Erro ao escrever no pipe do jogador.\n");
                            close(pipeJogadorOrigem);
                            continue;
                        }
                        close(pipeJogadorOrigem);
                        if (write(pipeJogadorDestino, &msgBackEnd, sizeof(msgBackEnd)) == -1) {
                            perror("[ERRO] Erro ao escrever no pipe do jogador.\n");
                            close(pipeJogadorDestino);
                            continue;
                        }
                        close(pipeJogadorDestino);

                    } else {
                        int pipeJogador = open(msgFrontEnd.informacao.mensagem.username, O_WRONLY);
                        if (pipeJogador == -1) {
                            perror("[ERRO] Erro ao abrir o pipe do jogador.\n");
                            continue;
                        }
                        MsgBackEnd msgBackEnd;
                        msgBackEnd.tipoMensagem = tipo_retorno_chat;
                        strcpy(msgBackEnd.informacao.retornoChat.origem, "Servidor");
                        strcpy(msgBackEnd.informacao.retornoChat.mensagem,
                               "O utilizador que tentou contactar nao existe.");
                        if (write(pipeJogador, &msgBackEnd, sizeof(msgBackEnd)) == -1) {
                            perror("[ERRO] Erro ao escrever no pipe do jogador.\n");
                            close(pipeJogador);
                            continue;
                        }
                        close(pipeJogador);
                    }
                    checkMsg = false;
                }
                break;
            case tipo_terminar_programa:
                if (true) {
                    // codigo para o tirar da lista
                    pUser ptrUser = tData->ptrGameSetup->ptrUsersAtivosHeader;
                    pUser ptrUserAnterior = NULL;
                    while (ptrUser != NULL) {
                        if (strcmp(ptrUser->username, msgFrontEnd.informacao.terminarPrograma.username) == 0) {
                            if (ptrUserAnterior == NULL) {
                                tData->ptrGameSetup->ptrUsersAtivosHeader = ptrUser->next;
                            } else {
                                ptrUserAnterior->next = ptrUser->next;
                            }
                            free(ptrUser->ptrUserInfo->position);
                            free(ptrUser->ptrUserInfo);
                            free(ptrUser);
                            tData->ptrGameSetup->usersAtivos--;
                            break;
                        }
                        ptrUserAnterior = ptrUser;
                        ptrUser = ptrUser->next;
                    }

                    MsgBackEnd msgBackEnd;
                    msgBackEnd.tipoMensagem = tipo_retorno_logout;
                    strcpy(msgBackEnd.informacao.retornoLogout.username,
                           msgFrontEnd.informacao.terminarPrograma.username);

                    // avisar todos os outros jogadores de que este jogador saiu
                    ptrUser = tData->ptrGameSetup->ptrUsersAtivosHeader;
                    while (ptrUser != NULL) {
                        int pipeJogador = open(ptrUser->username, O_WRONLY);
                        if (pipeJogador == -1) {
                            perror("[ERRO] Erro ao abrir o pipe do jogador.\n");
                            continue;
                        }

                        if (write(pipeJogador, &msgBackEnd, sizeof(msgBackEnd)) == -1) {
                            perror("[ERRO] Erro ao escrever no pipe do jogador.\n");
                            close(pipeJogador);
                            continue;
                        }
                        close(pipeJogador);
                        ptrUser = ptrUser->next;
                    }
                }
                break;
            case tipo_terminar:
                break;
        }

    } while (tData->continua == false);
    close(serverPipe);
    pthread_exit(NULL);
}



// TODO: uma função para enviar uma mensagem a todos os clientes com as informações do jogo

// TODO: uma função para enviar uma mendagem a todos os clientes com o mapa atualizado

// TODO: 3 Threads
//  1 - thread para lidar com coisas temporais (tempo de jogo, tempo de inscrição, tempo de decremento, etc)
//  2 - thread para lidar com os comandos dos clientes (jogadores): quando um user faz um movimento, a posição do user é atualizada no mapa e é enviado a todos os clientes
//  3 - thread para lidar com os comandos do servidor (administração do jogo)