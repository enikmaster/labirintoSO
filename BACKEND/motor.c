#include "constantes.h"
#include "motor.h"

pid_t pidBot;

int main(int argc, char *argv[]) {
    pathParaVariaveisAmbiente();
    GameSetup gameSetup;
    gameSetup.usersAtivos = 0;
    gameSetup.usersEspera = 0;
    gameSetup.tempoJogo = 0;
    gameSetup.nivel = 1;
    gameSetup.ptrMapa = NULL;
    gameSetup.ptrSetup = NULL;
    setGameSetup(&gameSetup);
    loadMapa(&gameSetup, gameSetup.nivel);
    if (gameSetup.ptrMapa == NULL) {
        perror("[ERRO] Erro ao carregar o mapa\n");
        exit(-1);
    }
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

        // estes dois comandos são apenas para a meta 1
        if (controlo == 8) {
            testarBot();
            controlo = 0;
        }
        if (controlo == 9) {
            desenhaMapa(gameSetup.ptrMapa->mapa);
            controlo = 0;
        }
    } while (controlo == 0);
    fecharJogo(&gameSetup);
    exit(0);
}