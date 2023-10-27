#include "constantes.h"
#include "motor.h"


int main(int argc, char *argv[]) {
//    initscr();
//    printw("Hello World !!!");
//    refresh();
    Tempo tempoJogo;
    definirTempoInicial(&tempoJogo);
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