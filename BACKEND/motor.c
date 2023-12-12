#include "../constantes.h"
#include "motor.h"

pid_t pidBot;

int main(int argc, char *argv[]) {
    int fd[2], nBytes;
    int jogadoresAtivos[MAX_USERS] = {[0 ... MAX_USERS - 1] = -1};
    int jogadoresEspera[MAX_USERS] = {[0 ... MAX_USERS - 1] = -1};
    char nome[20];

    if (access(SRV_FIFO, F_OK) != -1) {
        printf("[ERRO] Já existe um motor a ser executado.\n");
        exit(-1);
    }
    mkfifo(SRV_FIFO, 0640);
    fd[0] = open(SRV_FIFO, O_RDWR);// pipe do servidor
    if (fd[0] == -1) {
        perror("[ERRO] Erro ao abrir o pipe do servidor.\n");
        exit(-1);
    }
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

    fecharJogo(&gameSetup); // esta é a última coisa a fazer antes de sair
    exit(0);
}