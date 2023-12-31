#include "../constantes.h"
#include "jogoUI.h"

void adicionaBlock(ThreadDataFrontend *tData, int x, int y) {
    pBlock blocks = tData->ptrGameInfo->ptrBlocksHeader;
    while (blocks != NULL && blocks->next != NULL) {
        blocks = blocks->next;
    }
    pBlock ptrBlock = malloc(sizeof(Block));
    if (ptrBlock == NULL) {
        perror("[ERRO] Erro ao alocar memória para o Block.\n");
        exit(-1);
    }
    ptrBlock->position = malloc(sizeof(Position));
    if (ptrBlock->position == NULL) {
        perror("[ERRO] Erro ao alocar memória para o Position.\n");
        free(ptrBlock);
        exit(-1);
    }
    ptrBlock->identificador = 'B';
    ptrBlock->position->x = x;
    ptrBlock->position->y = y;
    ptrBlock->next = NULL;
    if (blocks == NULL) {
        pthread_mutex_lock(&tData->trinco);
        tData->ptrGameInfo->ptrBlocksHeader = ptrBlock;
        pthread_mutex_unlock(&tData->trinco);
    } else {
        pthread_mutex_lock(&tData->trinco);
        blocks->next = ptrBlock;
        pthread_mutex_unlock(&tData->trinco);
    }
}

void desenhaBlock(ThreadDataFrontend *tData, int x, int y) {
    mvwaddch(tData->janelaMapa, y, x, tData->ptrGameInfo->ptrBlocksHeader->identificador);
}

void apagaUserDoMapa(ThreadDataFrontend *tData, char *username) {
    pUserInfo users = tData->ptrGameInfo->ptrOtherUsersHeader;
    while (users != NULL) {
        if (users->identificador == toupper(username[0])) break;
        users = users->next;
    }
    if (users == NULL) return;
    mvwaddch(tData->janelaMapa, users->position->y, users->position->x, ' ');
    wrefresh(tData->janelaMapa);
}

void removeUser(ThreadDataFrontend *tData, char *username) {
    pUserInfo users = tData->ptrGameInfo->ptrOtherUsersHeader;
    pUserInfo prev = NULL;
    while (users != NULL) {
        if (users->identificador == toupper(username[0])) break;
        prev = users;
        users = users->next;
    }
    if (users == NULL) return;
    if (prev == NULL) {
        pthread_mutex_lock(&tData->trinco);
        tData->ptrGameInfo->ptrOtherUsersHeader = users->next;
        pthread_mutex_unlock(&tData->trinco);
    } else {
        pthread_mutex_lock(&tData->trinco);
        prev->next = users->next;
        pthread_mutex_unlock(&tData->trinco);
    }
    free(users->position);
    free(users);
}

void informaUser(ThreadDataFrontend *tData, MsgBackEnd msgBackEnd) {
    switch (msgBackEnd.tipoMensagem) {
        case tipo_block:
            mvwaddstr(tData->janelaLogs, 1, 1, "Adicionado um bloco.");
            break;
        case tipo_retorno_chat:
            mvwprintw(tData->janelaChat, 1, 1, "%s: %s", msgBackEnd.informacao.retornoChat.origem,
                      msgBackEnd.informacao.retornoChat.mensagem);
            break;
        case tipo_retorno_logout:
            mvwprintw(tData->janelaLogs, 1, 1, "O utilizador %s saiu do jogo.",
                      msgBackEnd.informacao.retornoLogout.username);
            break;
        case tipo_retorno_players:
            for (int i = 0; i < 5; i++) {
                if (strlen(msgBackEnd.informacao.retornoPlayers.listaJogadores[i]) > 0)
                    mvwprintw(tData->janelaLogs, 1 + i, 1, "%s",
                              msgBackEnd.informacao.retornoPlayers.listaJogadores[i]);
            }
            break;
        case tipo_retorno_inscricao:
            mvwaddstr(tData->janelaLogs, 1, 1, msgBackEnd.informacao.retornoInscricao.mensagem);
            break;
    }
    wrefresh(tData->janelaLogs);
}

// função para gerir a comunicação com o backend
void *threadGerirBackend(void *arg) {
    ThreadDataFrontend *tData = (ThreadDataFrontend *) arg;

    // dizer ao servidor que o cliente quer inscrever-se
    int serverPipe = open(SRV_FIFO, O_WRONLY);// pipe do servidor
    if (serverPipe == -1) {
        perror("[ERRO] Erro ao abrir o pipe do servidor.\n");
        exit(-1);
    }
    MsgFrontEnd inscrever = {
            .tipoMensagem = tipo_inscricao,
            .informacao.inscricao.pid = getpid()
    };
    strcpy(inscrever.informacao.inscricao.username, tData->ptrGameInfo->ptrThisUser->username);

    if (write(serverPipe, &inscrever, sizeof(inscrever)) == -1) {
        perror("[ERRO] Erro ao escrever no pipe do servidor.\n");
        close(serverPipe);
        exit(-1);
    }
    close(serverPipe);

    MsgBackEnd msgBackEnd;
    do {
        // abrir para leitura
        int pipeJogador = open(tData->ptrGameInfo->ptrThisUser->username, O_RDONLY);
        if (pipeJogador == -1) {
            perror("[ERRO] Erro ao abrir o pipe do jogador.\n");
            exit(-1);
        }
        memset(&msgBackEnd, 0, sizeof(msgBackEnd));

        ssize_t bytesRead = read(pipeJogador, &msgBackEnd, sizeof(msgBackEnd));
        wrefresh(tData->janelaLogs);
        if (bytesRead == 0) continue;
        if (bytesRead == -1) {
            perror("[ERRO] Erro ao ler do pipe do servidor.\n");
            exit(-1);
        }
        switch (msgBackEnd.tipoMensagem) {
            case tipo_retorno_inscricao:
                informaUser(tData, msgBackEnd);
                wrefresh(tData->janelaLogs);
                break;
            case tipo_retorno_players:
                informaUser(tData, msgBackEnd);
                wrefresh(tData->janelaLogs);
                break;
            case tipo_retorno_chat:
                informaUser(tData, msgBackEnd);
                wrefresh(tData->janelaChat);
                break;
            case tipo_retorno_logout:
                informaUser(tData, msgBackEnd);
                apagaUserDoMapa(tData, msgBackEnd.informacao.retornoLogout.username);
                removeUser(tData, msgBackEnd.informacao.retornoLogout.username);
                wrefresh(tData->janelaLogs);
                break;
            case tipo_block:
                adicionaBlock(tData, msgBackEnd.informacao.block.x, msgBackEnd.informacao.block.y);
                informaUser(tData, msgBackEnd);
                desenhaBlock(tData, msgBackEnd.informacao.block.x, msgBackEnd.informacao.block.y);
                wrefresh(tData->janelaMapa);
                break;
            case tipo_atualizar:
                break;

        }
        close(pipeJogador);
    } while (tData->continua == false);
}