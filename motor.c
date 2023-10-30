#include "constantes.h"
#include "motor.h"


int main(int argc, char *argv[]) {
//    initscr();
//    printw("Hello World !!!");
//    refresh();
    //Setup setup;
    //User UsersAtivos;
    //User UsersEspera;
    Mapa mapa;
    mapa.ptrMeta = NULL;
    mapa.ptrInicioHeader = NULL;
    //mapa.ptrUsersAtivosHeader = NULL;
    mapa.ptrRocksHeader = NULL;
    mapa.ptrBlocksHeader = NULL;
    mapa.ptrWallsHeader = NULL;
    loadMapa(&mapa);
    //GameSetup gameSetup;
    //gameSetup.ptrSetup = &setup;
    //gameSetup.ptrUsersAtivosHeader = &UsersAtivos;
    //gameSetup.ptrUsersEsperaHeader = &UsersEspera;
    //gameSetup.ptrMapa = &mapa;
    //setGameSetup(&gameSetup);

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