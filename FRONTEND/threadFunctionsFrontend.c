#include "../constantes.h"
#include "jogoUI.h"

// função para gerir a comunicaçõa com o backend
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
                wprintw(tData->janelaLogs, "%s", msgBackEnd.informacao.retornoInscricao.mensagem);
                wrefresh(tData->janelaLogs);
                break;
            case tipo_retorno_players:
                break;
            case tipo_retorno_chat:
                break;
            case tipo_terminar_programa:
                break;
        }

    } while (tData->continua);
}