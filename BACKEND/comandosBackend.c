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
    return 0;
}

int comandoBots(GameSetup *gameSetup) {
    pBot ptrBot;
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
    return 0;
}

int comandoBmov(GameSetup *gameSetup) {
    pBlock ptrBlocks;
    // adicionar um Block ao mapa
    ptrBlocks = gameSetup->ptrMapa->ptrBlocksHeader;
    int nBlocks = 1;
    while (ptrBlocks != NULL && ptrBlocks->next != NULL) {
        ptrBlocks = ptrBlocks->next;
        nBlocks++;
    }
    if (nBlocks >= MAX_BLOCKS) {
        printf("[INFO] Não é possível adicionar mais Blocks.\n");
        fflush(stdout);
        return 0;
    }
    pBlock newBlock = malloc(sizeof(Block));
    if (newBlock == NULL) {
        perror("[ERRO] Erro ao alocar memória para o Block.\n");
        return 6;
    }
    pPosition ptrPosition = malloc(sizeof(Position));
    if (ptrPosition == NULL) {
        perror("[ERRO] Erro ao alocar memória para o Position.\n");
        free(newBlock);
        return 6;
    }
    ptrPosition->next = NULL;
    newBlock->position = ptrPosition;
    setRandomPosition(gameSetup->ptrMapa, newBlock->position);
    newBlock->identificador = 'B';
    newBlock->next = NULL;
    if (ptrBlocks == NULL) {
        pthread_mutex_lock(&gameSetup->mutexBots);
        gameSetup->ptrMapa->ptrBlocksHeader = newBlock;
        pthread_mutex_unlock(&gameSetup->mutexBots);
    } else {
        pthread_mutex_lock(&gameSetup->mutexBots);
        ptrBlocks->next = newBlock;
        pthread_mutex_unlock(&gameSetup->mutexBots);
    }

    MsgBackEnd msgBlock = {
            .tipoMensagem = tipo_block,
    };
    strcpy(msgBlock.informacao.block.origem, "motor");
    msgBlock.informacao.block.x = newBlock->position->x;
    msgBlock.informacao.block.y = newBlock->position->y;

    pUser users = gameSetup->ptrUsersAtivosHeader;
    while (users != NULL) {
        int msgTodos = open(users->username, O_WRONLY);
        if (msgTodos == -1) {
            perror("[ERRO] Erro ao abrir o pipe do jogador.\n");
            close(msgTodos);
            free(newBlock);
            free(ptrPosition);
            return 6;
        }
        if (write(msgTodos, &msgBlock, sizeof(msgBlock)) == -1) {
            perror("[ERRO] Erro ao escrever no pipe do jogador.\n");
            close(msgTodos);
            free(newBlock);
            free(ptrPosition);
            return 6;
        }
        close(msgTodos);
        users = users->next;
    }
    printf("[INFO] Block adicionado na posição (x: %d, y: %d)\n", newBlock->position->x, newBlock->position->y);
    fflush(stdout);
    return 0;
}

int comandoRbm() {
    return 0;
}

int comandoBegin() {
    return 0;
}

int comandoEnd() {
    return 0;
}

int comandoKick(GameSetup *gameSetup, char *username) {
    // TODO: enviar mensagem ao cliente a dizer que foi kickado
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
                // mutex lock

                (ptrUserAnterior == NULL)
                ? (gameSetup->ptrUsersAtivosHeader = ptrUser->next)
                : (ptrUserAnterior->next = ptrUser->next);
                gameSetup->usersAtivos--;
                printf("[INFO] Jogador kickado %d: %s - '%c'\n", ptrUser->pid, ptrUser->username,
                       ptrUser->ptrUserInfo->identificador);
                fflush(stdout);
                free(ptrUser->ptrUserInfo->position);
                free(ptrUser->ptrUserInfo);
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

