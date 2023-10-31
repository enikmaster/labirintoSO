#include "constantes.h"
#include "motor.h"


int main(int argc, char *argv[]) {
    pathParaVariaveisAmbiente();
    GameSetup gameSetup;
    gameSetup.usersAtivos = 0;
    gameSetup.usersEspera = 0;
    gameSetup.tempoJogo = 0;
    gameSetup.nivel = 1;
    gameSetup.ptrMapa = NULL;
    loadMapa(gameSetup.ptrMapa, gameSetup.nivel);
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
//        if (controlo == 6) {
//            desenhaMapa(gameSetup.ptrMapa);
//            controlo = 0;
//        }
        if (controlo == 6) {
            testaBots();
            controlo = 0;
        }
    } while (controlo == 0);

    
    fecharJogo(&gameSetup);
    return 0;
}