#include "constantes.h"
#include "jogoUI.h"



int verificaComandoUI(char *comando) {
    const char listaComandos[][TAMANHO_COMANDO] =
            {"players", "msg", "exit"};
    comando[strlen(comando) - 1] = '\0';
    for (int i = 0; i < strlen(comando); ++i) {
        comando[i] = tolower(comando[i]);
    }
    // se for só uma palavra
    if (strchr(comando, ' ') == NULL) {
        for (int i = 0; i <= strlen((const char *) listaComandos); ++i) {
            if (strcmp(comando, listaComandos[i]) == 0) {
                //switch (i) {
                //    case 0:
                //        printf("Comando %s válido\n", comando);
                //        fflush(stdin);
                //        return 1;
                //    case 2:
                //        printf("Comando %s válido\n", comando);
                //        fflush(stdin);
                //        return 0;
                //}
                printf("Comando %s válido\n", comando);
                fflush(stdin);
                return (strcmp(comando, "end") == 0) ? 1 : 0;
            }
        }
        printf("Comando %s inválido\n", comando);
        fflush(stdin);
        return 0;
    }
    // se quer mandar mensagem
    if (strchr(comando, ' ') != NULL) {
        char comandoTemp[TAMANHO_COMANDO] = {'\0'};
        char userToMessage[TAMANHO_COMANDO] = {'\0'};
        char msgToSend[TAMANHO_COMANDO] = {'\0'};
        char argAux[TAMANHO_COMANDO] = {'\0'};
        int nArgumentos = sscanf(comando, "%s %s %s %s", comandoTemp, userToMessage, msgToSend, argAux);
        if (nArgumentos > 3) {
            printf("Comando %s inválido\n", comando);
            fflush(stdin);
            return 0;
        }
        if (strcmp(comandoTemp, "msg") == 0) {
            if (checkIfUserAtivo(userToMessage)) {}
            printf("Comando %s válido\n", comando);
            fflush(stdin);
            return 0;
        }
        printf("Comando %s inválido\n", comando);
    }
    fflush(stdin);
    return 0;
}


bool checkIfUserAtivo(char *userToMessage) { return false;}