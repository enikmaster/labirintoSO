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

    // abrir para leitura
    int pipeJogador = open(tData->ptrGameInfo->ptrThisUser->username, O_RDONLY);
    if (pipeJogador == -1) {
        perror("[ERRO] Erro ao abrir o pipe do jogador.\n");
        exit(-1);
    }

    MsgBackEnd msgBackEnd;
    do {
        memset(&msgBackEnd, 0, sizeof(msgBackEnd));
        ssize_t bytesRead = read(pipeJogador, &msgBackEnd, sizeof(msgBackEnd));
        if (bytesRead == 0) continue;
        if (bytesRead == -1) {
            perror("[ERRO] Erro ao ler do pipe do servidor.\n");
            exit(-1);
        }
        switch (msgBackEnd.tipoMensagem) {
            case tipo_retorno_inscricao:
                mvwprintw(tData->janelaLogs, 1, 1, "%s", msgBackEnd.informacao.retornoInscricao.mensagem);
                wrefresh(tData->janelaLogs);
                break;
            case tipo_retorno_players:
                break;
            case tipo_retorno_chat:
                break;
            case tipo_block:
                adicionaBlock(tData, msgBackEnd.informacao.block.x, msgBackEnd.informacao.block.y);
                desenhaBlock(tData, msgBackEnd.informacao.block.x, msgBackEnd.informacao.block.y);
                wrefresh(tData->janelaMapa);
                break;
        }

    } while (tData->continua);
}