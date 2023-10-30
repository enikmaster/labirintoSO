#include <string.h>
#include "constantes.h"
#include "motor.h"


int main(int argc, char *argv[]) {
//    initscr();
//    printw("Hello World !!!");
//    refresh();

    pathParaVariaveisAmbiente();
    GameSetup gameSetup;
    gameSetup.usersAtivos = 0;
    gameSetup.usersEspera = 0;
    gameSetup.tempoJogo = 0;
    gameSetup.nivel = 1;

    Mapa mapa;
    mapa.next = NULL;
    loadMapa(&mapa, gameSetup.nivel);
    gameSetup.ptrMapa = &mapa;
    int controlo = 0;
    char comando[TAMANHO_COMANDO];
    do {
        printf("Digite um comando: ");
        fgets(comando, sizeof(comando), stdin);
        controlo = verificaComando(comando);
        // switch(controlo) {
        //     case 1:
        //         controlo = comandoUsers();
        //         break;
        // }
    } while (controlo == 0);
//    endwin();
    return 0;
}