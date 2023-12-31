#include "jogoUI.h"
#include "../constantes.h"


int comandoPlayers(ThreadDataFrontend *tData) {

    /*
 * 1. Criar um obj MsgFrontEnd
 * 2. Preencher o obj com os dados necessários
 * 3. Abrir o named pipe com o meu nome para ouvir a mensagem de resposta do servido { fd_retorno }
 * 4. Abrir o named pipe do servidor para enviar a mensagem { fd }
 * 5. Enviar a mensagem para o servidor
    */

    MsgFrontEnd msgFrontEnd;
    msgFrontEnd.tipoMensagem = tipo_informacao;
    strcpy(msgFrontEnd.informacao.informacao.username, tData->ptrGameInfo->ptrThisUser->username);

    int fd = open(SRV_FIFO, O_WRONLY); // pipe do servidor
    if (fd == -1) {
        perror("[ERRO] Erro ao abrir o pipe do servidor.\n");
        exit(-1);
    }
    if (write(fd, &msgFrontEnd, sizeof(msgFrontEnd)) == -1) {
        perror("[ERRO] Erro ao escrever no pipe do servidor.\n");
        close(fd);
        exit(-1);
    }
    close(fd);
    return 0;
}

int comandoExit(ThreadDataFrontend *tData) {

    MsgFrontEnd msgFrontEnd;
    msgFrontEnd.tipoMensagem = tipo_terminar;
    strcpy(msgFrontEnd.informacao.terminar.username, tData->ptrGameInfo->ptrThisUser->username);

    int fd = open(SRV_FIFO, O_WRONLY); // pipe do servidor
    if (fd == -1) {
        perror("[ERRO] Erro ao abrir o pipe do servidor.\n");
        exit(-1);
    }
    if (write(fd, &msgFrontEnd, sizeof(msgFrontEnd)) == -1) {
        perror("[ERRO] Erro ao escrever no pipe do servidor.\n");
        close(fd);
        exit(-1);
    }
    close(fd);
    return 113;
}

int comandoMensagem(ThreadDataFrontend *tData, char *comando) {

    // dividir comando novamente
    // encher o obj msgFrontEnd
    // abrir os diversos pipes e enviar mensagem

    char comandoTemp[TAMANHO_COMANDO] = {'\0'};
    char userToMessage[TAMANHO_COMANDO] = {'\0'};
    char msgToSend[TAMANHO_COMANDO] = {'\0'};
    sscanf(comando, "%s %s", comandoTemp, userToMessage);
    char *mensagem_ptr = strchr(strchr(comando, ' ') + 1, ' ');
    if (mensagem_ptr != NULL) {
        mensagem_ptr++;
        strcpy(msgToSend, mensagem_ptr);
    }

    MsgFrontEnd msgFrontEnd;
    msgFrontEnd.tipoMensagem = tipo_mensagem;
    strcpy(msgFrontEnd.informacao.mensagem.username, tData->ptrGameInfo->ptrThisUser->username);
    strcpy(msgFrontEnd.informacao.mensagem.usernameDestino, userToMessage);
    strcpy(msgFrontEnd.informacao.mensagem.mensagem, msgToSend);

    int fd = open(SRV_FIFO, O_WRONLY); // pipe do servidor
    if (fd == -1) {
        perror("[ERRO] Erro ao abrir o pipe do servidor.\n");
        exit(-1);
    }
    if (write(fd, &msgFrontEnd, sizeof(msgFrontEnd)) == -1) {
        perror("[ERRO] Erro ao escrever no pipe do servidor.\n");
        close(fd);
        exit(-1);
    }
    close(fd);
    return 0;
}