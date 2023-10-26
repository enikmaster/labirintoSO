#include "constantes.h"
#include "motor.h"

int main(int argc, char *argv[]) {
//    initscr();
//    printw("Hello World !!!");
//    refresh();
    char cmdTeste[TAMANHO_COMANDO];
    do {
        printf("Digite um comando: ");
        fgets(cmdTeste, sizeof(cmdTeste), stdin);
    } while (verificaComando(cmdTeste) == 0);
//    endwin();
    return 0;
}