#include "../constantes.h"
#include "jogoUI.h"
#include "../BACKEND/motor.h"

void setGameInfoFrontend(GameInfoFrontend *gameInfoFrontend) {
    for (int y = 0; y < MAPA_LINHAS; ++y)
        for (int x = 0; x < MAPA_COLUNAS; ++x)
            gameInfoFrontend->mapa[y][x] = ' ';
    gameInfoFrontend->ptrRocksHeader = NULL;
    gameInfoFrontend->ptrBlocksHeader = NULL;
    gameInfoFrontend->ptrThisUser = NULL;
    gameInfoFrontend->ptrOtherUsersHeader = NULL;
    gameInfoFrontend->tempoJogo = 0;
    gameInfoFrontend->nivel = 0;
}

int verificaComandoUI(char *comando, WINDOW *janelaBaixo) {
    comando[strlen(comando)] = '\0';
    for (int i = 0; i < strlen(comando); ++i) {
        comando[i] = tolower(comando[i]);
    }
    char comandoTemp[TAMANHO_COMANDO] = {'\0'};
    char userToMessage[TAMANHO_COMANDO] = {'\0'};
    char msgToSend[TAMANHO_COMANDO] = {'\0'};
    if (sscanf(comando, "%s %s", comandoTemp, userToMessage) == 2
        && strcmp(comandoTemp, "msg") == 0
        && checkIfUserAtivo(userToMessage)) {
        char *mensagem_ptr = strchr(strchr(comando, ' ') + 1, ' ');
        if (mensagem_ptr != NULL) {
            mensagem_ptr++;
            strcpy(msgToSend, mensagem_ptr);
            wprintw(janelaBaixo, "\n Comando %s válido ", comando);
            wprintw(janelaBaixo, "\n Mensagem: %s  ", msgToSend);
            fflush(stdin);
            return 0;
        }
    }

    if (strcmp(comando, "players") == 0) {
        wprintw(janelaBaixo, "\n Comando %s válido ", comando);
        fflush(stdin);
        /*
         * 1. Criar um obj MsgFrontEnd
         * 2. Preencher o obj com os dados necessários
         * 3. Abrir o named pipe com o meu nome para ouvir a mensagem de resposta do servido { fd_retorno }
         * 4. Abrir o named pipe do servidor para enviar a mensagem { fd }
         * 5. Enviar a mensagem para o servidor
        */

        MsgFrontEnd msgFrontEnd;
        msgFrontEnd.tipoMensagem = tipo_informacao;
        msgFrontEnd.informacao.informacao.pid = getpid();

        char nome[20] = {'\0'};
        int fd;
        sprintf(nome, "cli%d", msgFrontEnd.informacao.informacao.pid);

        fd = open(SRV_FIFO, O_WRONLY); // pipe do servidor
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
    } else if (strcmp(comando, "exit") == 0) {
        fflush(stdin);
        return 1;
    }

    wprintw(janelaBaixo, "\n Comando %s inválido ", comando);
    fflush(stdin);
    return 0;
}


bool checkIfUserAtivo(char *userToMessage) { return true; }

void desenhaMapa(WINDOW *janela, int tipo) {
    if (tipo == 1) {
        scrollok(janela, TRUE);
        wprintw(janela, "\nComando: ");
    } else {
        keypad(janela, TRUE);
        wclear(janela);
        wborder(janela, '|', '|', '-', '-', '+', '+', '+', '+');
    }
    refresh();
    wrefresh(janela);
}

void trataTeclado(WINDOW *janelaTopo, WINDOW *janelaBaixo) {
    keypad(janelaTopo, TRUE);
    wmove(janelaTopo, 1, 1);
    int tecla = wgetch(janelaTopo);
    char comando[100];

    while (tecla != 113) // trata as tecla até introduzirem a letra q. O código asci de q é 113
    {
        if (tecla == KEY_UP) {
            desenhaMapa(janelaTopo, 2);
            mvwprintw(janelaTopo, 1, 1, "Estou a carregar na tecla UP na posição 1,1 ");
            wrefresh(janelaTopo);
        } else if (tecla == KEY_RIGHT) {
            desenhaMapa(janelaTopo, 2);
            mvwprintw(janelaTopo, 1, 1, "Estou a carregar na tecla RIGHT na posição 1,1");
            wrefresh(janelaTopo);
        } else if (tecla == KEY_LEFT) {
            desenhaMapa(janelaTopo, 2);
            mvwprintw(janelaTopo, 1, 1, "Estou a carregar na tecla LEFT na posição 1,1");
            wrefresh(janelaTopo);
        } else if (tecla == KEY_DOWN) {
            desenhaMapa(janelaTopo, 2);
            mvwprintw(janelaTopo, 1, 1, "Estou a carregar na tecla DOWN na posição 1,1");
            wrefresh(janelaTopo);
        } else if (tecla == ' ') {
            wclear(janelaBaixo);
            echo();
            wprintw(janelaBaixo, "\nDigite um comando: ");
            wgetstr(janelaBaixo, comando);
            noecho();
            wrefresh(janelaBaixo);
            if (verificaComandoUI(comando, janelaBaixo) == 1) return;
            noecho();
            wrefresh(janelaBaixo);
        }
        wmove(janelaTopo, 1, 1);
        wrefresh(janelaTopo);
        tecla = wgetch(janelaTopo);
    }
}

void fecharRocks(pRock ptrRocksHeader) {
    pRock libertaRock;
    pPosition libertaPosicao;
    while (ptrRocksHeader != NULL) {
        libertaRock = ptrRocksHeader;
        libertaPosicao = libertaRock->position;
        ptrRocksHeader = ptrRocksHeader->next;
        free(libertaPosicao);
        free(libertaRock);
    }
}

void fecharBlocks(pBlock ptrBlocksHeader) {
    pBlock libertaBlock;
    pPosition libertaPosicao;
    while (ptrBlocksHeader != NULL) {
        libertaBlock = ptrBlocksHeader;
        libertaPosicao = libertaBlock->position;
        ptrBlocksHeader = ptrBlocksHeader->next;
        free(libertaPosicao);
        free(libertaBlock);
    }
}

void fecharThisUser(pUser *ptrThisUser) {
    free((*ptrThisUser)->ptrUserInfo->position);
    free((*ptrThisUser)->ptrUserInfo);
    free((*ptrThisUser));
    *ptrThisUser = NULL;
}

void fecharUsers(pUser ptrThisUser, pUserInfo ptrOtherUsersHeader) {
    if (ptrThisUser != NULL) fecharThisUser(&ptrThisUser);
    if (ptrOtherUsersHeader == NULL) return;
    pUserInfo libertaUserInfo;
    pPosition libertaPosicao;
    while (ptrOtherUsersHeader != NULL) {
        libertaUserInfo = ptrOtherUsersHeader;
        libertaPosicao = libertaUserInfo->position;
        ptrOtherUsersHeader = ptrOtherUsersHeader->next;
        free(libertaPosicao);
        free(libertaUserInfo);
    }
}

void fecharCliente(GameInfoFrontend *game) {
    // liberta a memória das pedras
    if (game->ptrRocksHeader != NULL) fecharRocks(game->ptrRocksHeader);
    // liberta a memória dos blocos
    if (game->ptrBlocksHeader != NULL) fecharBlocks(game->ptrBlocksHeader);
    // liberta a memória dos users
    fecharUsers(game->ptrThisUser, game->ptrOtherUsersHeader);
    // liberta a memória do jogo em si
    free(game);
}