#include "../BACKEND/constantes.h"
#include "jogoUI.h"


int main(int argc, char *argv[]) {

    int controlo = 0;
    char comando[TAMANHO_NAMES] = {'\0'};

    do {
        printf("Digite um comando: ");
        fgets(comando, sizeof(comando), stdin);
        controlo = verificaComandoUI(comando);
    } while (controlo == 0);
    return 0;
};
