#include "constantes.h"
#include "jogoUI.h"


int main(int argc, char *argv[]) {

    int controlo = 0;
    char comando[TAMANHO_NAMES] = {'\0'};

    if (argc != 2)  {
        printf("Insira o numero correto de parametros.\n");
        fflush(stdin);
        return 0;
    } else {
    do {
        printf("Digite um comando: ");
        fgets(comando, sizeof(comando), stdin);
        controlo = verificaComandoUI(comando);
    } while (controlo == 0);
    return 0;

    }
};
