#include "../BACKEND/constantes.h"
#include "jogoUI.h"

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