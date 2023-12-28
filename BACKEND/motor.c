#include "../constantes.h"
#include "motor.h"

pid_t pidBot;

int main(int argc, char *argv[]) {
    if (argc != 1) {
        printf("[ERRO] Número de argumentos inválido.\n");
        exit(-1);
    }
    if (access(SRV_FIFO, F_OK) != -1) {
        printf("[ERRO] Já existe um motor a ser executado.\n");
        exit(-1);
    }
    mkfifo(SRV_FIFO, 0640);

    pathParaVariaveisAmbiente();
    GameSetup gameSetup;
    setGameSetup(&gameSetup);
    loadMapa(&gameSetup, gameSetup.nivel);
    if (gameSetup.ptrMapa == NULL) {
        perror("[ERRO] Erro ao carregar o mapa\n");
        exit(-1);
    }
    ThreadData tData = {
            .continua = false,
            .ptrGameSetup = &gameSetup
    };
    // thread para lidar com a comunicacao com o frontend
    pthread_t threadGerirFrontendId;
    if (pthread_create(&threadGerirFrontendId, NULL, threadGerirFrontend, (void *) &tData) != 0) {
        perror("[ERRO] Erro ao criar a thread dae comunicacao com o frontend.\n");
        exit(-1);
    }
    // thread para lidar com os timers do jogo, após os jogadores terem feito a inscrição
    pthread_t threadTimersId;
    if (pthread_create(&threadTimersId, NULL, threadTimers, (void *) &tData) != 0) {
        perror("[ERRO] Erro ao criar a thread dos timers.\n");
        exit(-1);
    }

    int controlo = 0;
    char comando[TAMANHO_COMANDO];
    char comandoTemp[TAMANHO_COMANDO];
    char username[TAMANHO_COMANDO];

    do {
        printf("Digite um comando: ");
        fgets(comando, sizeof(comando), stdin);
        controlo = verificaComando(comando);
        switch (controlo) {
            case 1:
                controlo = comandoUsers(&gameSetup);
                break;
            case 2:
                controlo = comandoBots();
                break;
            case 3:
                controlo = comandoBmov();
                break;
            case 4:
                controlo = comandoRbm();
                break;
            case 5:
                controlo = comandoBegin();
                break;
            case 6:
                controlo = comandoEnd(); // fecha a loja e manda todos para casa
                controlo = 6;
                break;
            case 7:
                memset(comandoTemp, 0, sizeof(comandoTemp));
                memset(username, 0, sizeof(username));
                sscanf(comando, "%s %s", comandoTemp, username);
                controlo = comandoKick(&gameSetup, username);
                break;
            default:
                printf("[INFO] Comando inválido.\n");
                break;
        }

        // estes dois comandos são apenas para a meta 1
        /*if (controlo == 8) {
            testarBot();
            controlo = 0;
        }
        if (controlo == 9) {
            desenhaMapa(gameSetup.ptrMapa->mapa);
            controlo = 0;
        }*/
    } while (controlo != 6);
    // fechar a thread
    tData.continua = 0;
    if (pthread_join(threadTimersId, NULL) != 0) {
        perror("[ERRO] Erro ao fechar a thread dos timers.\n");
        exit(-1);
    }
    // fechar o pipe do servidor
    //close(fd[0]);
    unlink(SRV_FIFO);
    fecharJogo(&gameSetup); // esta é a última coisa a fazer antes de sair
    exit(0);
}