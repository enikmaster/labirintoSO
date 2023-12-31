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
                mvwprintw(tData->janelaLogs, 2, 2, "%s", msgBackEnd.informacao.retornoInscricao.mensagem);
                wrefresh(tData->janelaLogs);
                break;
            case tipo_retorno_players:
                for (int i = 0; i < 5; i++) {
                    if (strlen(msgBackEnd.informacao.retornoPlayers.listaJogadores[i]) > 0)
                        mvwprintw(tData->janelaLogs, 5 + i, 5 + i, "%s",
                                  msgBackEnd.informacao.retornoPlayers.listaJogadores[i]);
                }
                wrefresh(tData->janelaLogs);
                break;
            case tipo_retorno_chat:
                mvwprintw(tData->janelaLogs, 3, 3, "%s: %s", msgBackEnd.informacao.retornoChat.origem,
                          msgBackEnd.informacao.retornoChat.mensagem);
                break;
            case tipo_terminar_programa:
                mvwprintw(tData->janelaLogs, 4, 4, "%s", msgBackEnd.informacao.terminarPrograma.mensagem);
                break;
        }

    } while (tData->continua);
}