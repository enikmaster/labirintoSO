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
    if (mkfifo(SRV_FIFO, 0640) == -1) {
        perror("[ERRO] Erro a abrir o pipe do jogador");
        exit(-1);
    }
    srand((unsigned int) time(NULL));
    pathParaVariaveisAmbiente();
    GameSetup gameSetup = {
            .mutexJogadores = PTHREAD_MUTEX_INITIALIZER,
            .mutexBots = PTHREAD_MUTEX_INITIALIZER,
            .mutexMapa = PTHREAD_MUTEX_INITIALIZER,
            .mutexGeral = PTHREAD_MUTEX_INITIALIZER,
    };
    setGameSetup(&gameSetup);
    loadMapa(&gameSetup, gameSetup.nivel);
    if (gameSetup.ptrMapa == NULL) {
        perror("[ERRO] Erro ao carregar o mapa\n");
        exit(-1);
    }
    ThreadData tData = {
            .continua = false,
            .ptrGameSetup = &gameSetup,
            .trinco = PTHREAD_MUTEX_INITIALIZER
    };
    // thread para lidar com a comunicacao com o frontend
    pthread_t threadGerirFrontendId;
    if (pthread_create(&threadGerirFrontendId, NULL, threadGerirFrontend, (void *) &tData) != 0) {
        perror("[ERRO] Erro ao criar a thread da comunicação do frontend.\n");
        exit(-1);
    }
    // thread para lidar com os timers do jogo, após os jogadores terem feito a inscrição
    /*pthread_t threadTimersId;
    if (pthread_create(&threadTimersId, NULL, threadTimers, (void *) &tData) != 0) {
        perror("[ERRO] Erro ao criar a thread dos timers.\n");
        exit(-1);
    }*/

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
                controlo = comandoBots(&gameSetup);
                break;
            case 3:
                controlo = comandoBmov(&gameSetup);
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
    tData.continua = true;
    int forceExit = open(SRV_FIFO, O_WRONLY);
    if (forceExit == -1) {
        perror("[ERRO] Erro ao abrir o pipe do servidor.\n");
        fecharJogo(&gameSetup);
        exit(-1);
    }
    MsgFrontEnd terminarPrograma = {
            .tipoMensagem = tipo_terminar_programa
    };
    strcpy(terminarPrograma.informacao.terminarPrograma.origem, "motor");
    strcpy(terminarPrograma.informacao.terminarPrograma.mensagem, "terminar");
    if (write(forceExit, &terminarPrograma, sizeof(terminarPrograma)) == -1) {
        perror("[ERRO] Erro ao escrever no pipe do servidor.\n");
        close(forceExit);
        fecharJogo(&gameSetup);
        exit(-1);
    }
    if (pthread_join(threadGerirFrontendId, NULL) != 0) {
        perror("[ERRO] Erro ao esperar pela thread da comunicação do frontend.\n");
        close(forceExit);
        fecharJogo(&gameSetup);
        exit(-1);
    }
    close(forceExit);
    /*tData.continua = true;
    forceExit = open(SRV_FIFO, O_WRONLY);
    if (forceExit == -1) {
        perror("[ERRO] Erro ao abrir o pipe do servidor.\n");
        fecharJogo(&gameSetup);
        exit(-1);
    }
    if (pthread_join(threadTimersId, NULL) != 0) {
        perror("[ERRO] Erro ao fechar a thread dos timers.\n");
        fecharJogo(&gameSetup);
        exit(-1);
    }
    close(forceExit);*/
    // fechar o pipe do servidor
    unlink(SRV_FIFO);
    fecharJogo(&gameSetup); // esta é a última coisa a fazer antes de sair
    exit(0);
}