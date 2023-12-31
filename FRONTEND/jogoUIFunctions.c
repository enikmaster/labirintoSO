#include "../constantes.h"
#include "jogoUI.h"

void setThisUser(pUser *thisUser, char *username) {
    strcpy((*thisUser)->username, username);
    (*thisUser)->pid = getpid();
    pUserInfo userInfo = malloc(sizeof(UserInfo));
    if (userInfo == NULL) {
        perror("[ERRO] Erro ao alocar memória para o UserInfo.\n");
        free(*thisUser);
        exit(-1);
    }
    userInfo->identificador = toupper(username[0]);
    userInfo->position = malloc(sizeof(Position));
    if (userInfo->position == NULL) {
        perror("[ERRO] Erro ao alocar memória para o Position.\n");
        free(userInfo);
        free(*thisUser);
        exit(-1);
    }
    userInfo->position->x = 0;
    userInfo->position->y = 0;
    userInfo->next = NULL;
    (*thisUser)->ptrUserInfo = userInfo;
    (*thisUser)->next = NULL;
}

void setGameInfoFrontend(GameInfoFrontend *gameInfoFrontend) {
    for (int y = 0; y < MAPA_LINHAS; ++y)
        for (int x = 0; x < MAPA_COLUNAS; ++x)
            gameInfoFrontend->mapa[y][x] = ' ';
    gameInfoFrontend->ptrRocksHeader = NULL;
    gameInfoFrontend->ptrBlocksHeader = NULL;
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
            return 1;
        }
    }

    if (strcmp(comando, "players") == 0) {
        wprintw(janelaBaixo, "\n Comando %s válido ", comando);
        fflush(stdin);
        return 2;

    } else if (strcmp(comando, "exit") == 0) {
        fflush(stdin);
        return 3;
    }

    wprintw(janelaBaixo, "\n Comando %s inválido ", comando);
    fflush(stdin);
    return -1;
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

void trataTeclado(ThreadDataFrontend *tData) {
    keypad(tData->janelaMapa, TRUE);
    wmove(tData->janelaMapa, 1, 1);
    int tecla = wgetch(tData->janelaMapa);
    char comando[100];

    while (tecla != 113) // trata as tecla até introduzirem a letra q. O código asci de q é 113
    {
        if (tecla == KEY_UP) {
            desenhaMapa(tData->janelaMapa, 2);
            mvwprintw(tData->janelaMapa, 1, 1, "Estou a carregar na tecla UP na posição 1,1 ");
            wrefresh(tData->janelaMapa);
        } else if (tecla == KEY_RIGHT) {
            desenhaMapa(tData->janelaMapa, 2);
            mvwprintw(tData->janelaMapa, 1, 1, "Estou a carregar na tecla RIGHT na posição 1,1");
            wrefresh(tData->janelaMapa);
        } else if (tecla == KEY_LEFT) {
            desenhaMapa(tData->janelaMapa, 2);
            mvwprintw(tData->janelaMapa, 1, 1, "Estou a carregar na tecla LEFT na posição 1,1");
            wrefresh(tData->janelaMapa);
        } else if (tecla == KEY_DOWN) {
            desenhaMapa(tData->janelaMapa, 2);
            mvwprintw(tData->janelaMapa, 1, 1, "Estou a carregar na tecla DOWN na posição 1,1");
            wrefresh(tData->janelaMapa);
        } else if (tecla == ' ') {
            wclear(tData->janelaComandos);
            echo();
            wprintw(tData->janelaComandos, "\nComando: ");
            wgetstr(tData->janelaComandos, comando);
            noecho();
            wrefresh(tData->janelaComandos);


            switch (verificaComandoUI(comando, tData->janelaComandos)) {
                case 1: // comando msg
                    comandoMensagem(tData, comando);
                    break;
                case 2: // comando players
                    comandoPlayers(tData);
                    break;
                case 3: // comando exit
                    tecla = comandoExit(tData);
                    break;
                case -1:
                    break;
            }

            noecho();
            wrefresh(tData->janelaComandos);
        }
        wmove(tData->janelaMapa, 1, 1);
        wrefresh(tData->janelaMapa);
        tecla = wgetch(tData->janelaMapa);
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