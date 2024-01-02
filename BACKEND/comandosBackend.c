#include "../constantes.h"
#include "motor.h"

void setRandomPosition(Map *ptrMapa, Position *ptrPosition) {
    int x, y;
    do {
        x = rand() % MAPA_COLUNAS;
        y = rand() % MAPA_LINHAS;
    } while (ptrMapa->mapa[y][x] != ' ');
    ptrPosition->x = x;
    ptrPosition->y = y;
}

int comandoUsers(GameSetup *gameSetup) {
    pUser ptrUser;
    pthread_mutex_lock(&gameSetup->mutexJogadores);
    if (gameSetup->usersAtivos == 0) {
        printf("[INFO] Não existem jogadores ativos.\n");
        fflush(stdout);
    } else {
        ptrUser = gameSetup->ptrUsersAtivosHeader;
        while (ptrUser != NULL) {
            printf("[INFO] Jogador ativo %d: %s - '%c'\n", ptrUser->pid, ptrUser->username,
                   ptrUser->ptrUserInfo->identificador);
            fflush(stdout);
            ptrUser = ptrUser->next;
        }
    }
    if (gameSetup->usersEspera == 0) {
        printf("[INFO] Não existem jogadores em espera.\n");
        fflush(stdout);
    } else {
        ptrUser = gameSetup->ptrUsersEsperaHeader;
        while (ptrUser != NULL) {
            printf("[INFO] Jogador em espera %d: %s - '%c'\n", ptrUser->pid, ptrUser->username,
                   ptrUser->ptrUserInfo->identificador);
            fflush(stdout);
            ptrUser = ptrUser->next;
        }
    }
    pthread_mutex_unlock(&gameSetup->mutexJogadores);
    return 0;
}

int comandoBots(GameSetup *gameSetup) {
    pBot ptrBot;
    pthread_mutex_lock(&gameSetup->mutexBots);
    if (gameSetup->ptrBotsHeader == NULL) {
        printf("[INFO] Não existem bots ativos.\n");
        fflush(stdout);
    } else {
        ptrBot = gameSetup->ptrBotsHeader;
        while (ptrBot != NULL) {
            printf("[INFO] Bot ativo: pid - %d | duração - %ds\n", ptrBot->pid, ptrBot->duracao);
            fflush(stdout);
            ptrBot = ptrBot->next;
        }
    }
    pthread_mutex_unlock(&gameSetup->mutexBots);
    return 0;
}

int comandoBmov(GameSetup *gameSetup) {
    pBlock ptrBlocks;
    // adicionar um Block ao mapa
    int nBlocks = 1;
    pthread_mutex_lock(&gameSetup->mutexMapa);
    ptrBlocks = gameSetup->ptrMapa->ptrBlocksHeader;
    while (ptrBlocks != NULL && ptrBlocks->next != NULL) {
        ptrBlocks = ptrBlocks->next;
        nBlocks++;
    }
    pthread_mutex_unlock(&gameSetup->mutexMapa);
    if (nBlocks >= MAX_BLOCKS) {
        printf("[INFO] Não é possível adicionar mais Blocks.\n");
        fflush(stdout);
        return 0;
    }
    pBlock newBlock = malloc(sizeof(Block));
    if (newBlock == NULL) {
        perror("[ERRO] Erro ao alocar memória para o Block.\n");
        return 0;
    }
    pPosition ptrPosition = malloc(sizeof(Position));
    if (ptrPosition == NULL) {
        perror("[ERRO] Erro ao alocar memória para o Position.\n");
        free(newBlock);
        return 0;
    }
    ptrPosition->next = NULL;
    newBlock->position = ptrPosition;
    setRandomPosition(gameSetup->ptrMapa, newBlock->position);
    newBlock->identificador = 'B';
    newBlock->next = NULL;
    pthread_mutex_lock(&gameSetup->mutexMapa);
    if (ptrBlocks == NULL)
        gameSetup->ptrMapa->ptrBlocksHeader = newBlock;
    else
        ptrBlocks->next = newBlock;
    pthread_mutex_unlock(&gameSetup->mutexMapa);

    MsgBackEnd msgBlock = {
            .tipoMensagem = tipo_block,
    };
    strcpy(msgBlock.informacao.block.origem, "motor");
    msgBlock.informacao.block.x = newBlock->position->x;
    msgBlock.informacao.block.y = newBlock->position->y;

    pthread_mutex_lock(&gameSetup->mutexMapa);
    pUser users = gameSetup->ptrUsersAtivosHeader;
    while (users != NULL) {
        int msgTodos = open(users->username, O_WRONLY);
        if (msgTodos == -1) {
            perror("[ERRO] Erro ao abrir o pipe do jogador.\n");
            close(msgTodos);
            free(newBlock);
            free(ptrPosition);
            pthread_mutex_unlock(&gameSetup->mutexMapa);
            return 0;
        }
        if (write(msgTodos, &msgBlock, sizeof(msgBlock)) == -1) {
            perror("[ERRO] Erro ao escrever no pipe do jogador.\n");
            close(msgTodos);
            free(newBlock);
            free(ptrPosition);
            pthread_mutex_unlock(&gameSetup->mutexMapa);
            return 0;
        }
        close(msgTodos);
        users = users->next;
    }
    pthread_mutex_unlock(&gameSetup->mutexMapa);
    printf("[INFO] Block adicionado na posição (x: %d, y: %d)\n", newBlock->position->x, newBlock->position->y);
    fflush(stdout);
    return 0;
}

int comandoRbm(GameSetup *gameSetup) {
    pBlock ptrBlock;
    pthread_mutex_lock(&gameSetup->mutexMapa);
    ptrBlock = gameSetup->ptrMapa->ptrBlocksHeader;
    pthread_mutex_unlock(&gameSetup->mutexMapa);
    // verificar se há blocks
    if (ptrBlock == NULL) {
        printf("[INFO] Não existem Blocks no mapa.\n");
        fflush(stdout);
        return 0;
    }
    // remover um Block do mapa
    pthread_mutex_lock(&gameSetup->mutexMapa);
    gameSetup->ptrMapa->ptrBlocksHeader = gameSetup->ptrMapa->ptrBlocksHeader->next;
    pthread_mutex_unlock(&gameSetup->mutexMapa);
    free(ptrBlock->position);
    free(ptrBlock);
    // avisar todos os jogadores
    if (gameSetup->usersAtivos == 0) return 0;
    MsgBackEnd msgBlock = {
            .tipoMensagem = tipo_remove_block,
    };
    strcpy(msgBlock.informacao.removeBlock.origem, "motor");
    pthread_mutex_lock(&gameSetup->mutexMapa);
    pUser users = gameSetup->ptrUsersAtivosHeader;
    while (users != NULL) {
        int msgTodos = open(users->username, O_WRONLY);
        if (msgTodos == -1) {
            perror("[ERRO] Erro ao abrir o pipe do jogador.\n");
            close(msgTodos);
            pthread_mutex_unlock(&gameSetup->mutexMapa);
            return 0;
        }
        if (write(msgTodos, &msgBlock, sizeof(msgBlock)) == -1) {
            perror("[ERRO] Erro ao escrever no pipe do jogador.\n");
            close(msgTodos);
            pthread_mutex_unlock(&gameSetup->mutexMapa);
            return 0;
        }
        close(msgTodos);
        users = users->next;
    }
    pthread_mutex_unlock(&gameSetup->mutexMapa);
    printf("[INFO] Block removido.\n");
    fflush(stdout);
    return 0;
}

int comandoBegin(GameSetup *gameSetup) {
    gameSetup->jogoAtivo = true;
    // begin();
    return 0;
}

int comandoEnd(GameSetup *gameSetup) {
    pUser ptrUser;
    ptrUser = gameSetup->ptrUsersAtivosHeader;
    MsgBackEnd msgBackEnd;
    msgBackEnd.tipoMensagem = tipo_retorno_kick;

    while (ptrUser != NULL) {
        int pipeJogador = open(ptrUser->username, O_WRONLY);
        if (pipeJogador == -1) {
            perror("[ERRO] Erro ao abrir o pipe do jogador.\n");
            continue;
        }

        strcpy(msgBackEnd.informacao.retornoKick.username, ptrUser->username);
        if (write(pipeJogador, &msgBackEnd, sizeof(msgBackEnd)) == -1) {
            perror("[ERRO] Erro ao escrever no pipe do jogador.\n");
            close(pipeJogador);
            continue;
        }
        close(pipeJogador);
        ptrUser = ptrUser->next;
    }

    return 6;
}

int comandoKick(GameSetup *gameSetup, char *username) {
    pUser ptrUser, ptrUserAnterior;
    if (gameSetup->usersAtivos == 0) {
        printf("[INFO] Não existem jogadores ativos.\n");
        fflush(stdout);
        return 0;
    } else {
        ptrUser = gameSetup->ptrUsersAtivosHeader;
        ptrUserAnterior = NULL;
        username[strlen(username)] = '\0';
        for (int i = 0; i < strlen(username); ++i)
            username[i] = tolower(username[i]);
        char usernameNomalized[strlen(username)];

        while (ptrUser != NULL) {
            strcpy(usernameNomalized, gameSetup->ptrUsersAtivosHeader->username);
            for (int i = 0; i < strlen(usernameNomalized); ++i)
                usernameNomalized[i] = tolower(usernameNomalized[i]);

            // enviar mensagem ao cliente que foi kickado
            MsgBackEnd msgBackEnd;
            msgBackEnd.tipoMensagem = tipo_retorno_kick;
            strcpy(msgBackEnd.informacao.retornoKick.username, username);

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

            if (strcmp(usernameNomalized, username) == 0) {
                // TODO: mutexes
                // mutex lock

                (ptrUserAnterior == NULL)
                ? (gameSetup->ptrUsersAtivosHeader = ptrUser->next)
                : (ptrUserAnterior->next = ptrUser->next);
                gameSetup->usersAtivos--;
                printf("[INFO] Jogador kickado %d: %s - '%c'\n", ptrUser->pid, ptrUser->username,
                       ptrUser->ptrUserInfo->identificador);
                fflush(stdout);
                free(ptrUser->ptrUserInfo->position);
                ptrUser->ptrUserInfo->position = NULL;
                free(ptrUser->ptrUserInfo);
                ptrUser->ptrUserInfo = NULL;
                free(ptrUser);
                // mutex unlock

                break;
            }
            ptrUserAnterior = ptrUser;
            ptrUser = ptrUser->next;
        }

        if (ptrUser == NULL) {
            printf("[INFO] Não existe nenhum jogador com o username %s.\n", username);
            fflush(stdout);
            return 0;
        }

        MsgBackEnd msgBackEnd;
        msgBackEnd.tipoMensagem = tipo_retorno_logout;
        strcpy(msgBackEnd.informacao.retornoLogout.username, username);

        // avisar todos os outros jogadores de que este jogador foi kickado
        ptrUser = gameSetup->ptrUsersAtivosHeader;
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
    return 0;
}
