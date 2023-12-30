#include "../constantes.h"
#include "jogoUI.h"

int main(int argc, char *argv[]) {
    int controlo = 0;
    char comando[TAMANHO_NAMES] = {'\0'};

    if (access(SRV_FIFO, F_OK) != 0) {
        perror("[ERRO] O servidor não está a correr.\n");
        exit(-1);
    }

    if (argc != 2) {
        printf("[ERRO] Número de argumentos inválido.\n");
        exit(-1);
    }

    if (mkfifo(argv[1], 0640) == -1) {
        perror("[ERRO] Erro a abrir o pipe do jogador");
        exit(-1);
    }


    GameInfoFrontend gameInfoFrontend;
    pUser thisUser = malloc(sizeof(User));
    if (thisUser == NULL) {
        perror("[ERRO] Erro ao alocar memória para o utilizador.\n");
        fecharCliente(&gameInfoFrontend);
        exit(-1);
    }
    setThisUser(&thisUser, argv[1]);
    gameInfoFrontend.ptrThisUser = thisUser;
    setGameInfoFrontend(&gameInfoFrontend);

    ThreadDataFrontend tData = {
            .continua = false,
            .janelaMapa = NULL,
            .janelaChat = NULL,
            .janelaLogs = NULL,
            .janelaComandos = NULL,
            .ptrGameInfo = &gameInfoFrontend,
            .trinco = PTHREAD_MUTEX_INITIALIZER
    };

    initscr();
    raw();
    noecho();
    keypad(stdscr, TRUE);
    attrset(A_DIM);
    cbreak();

    mvprintw(1, 10, "[ Tempo de jogo: %d   |   Nível: %d ]", tData.ptrGameInfo->tempoJogo, tData.ptrGameInfo->nivel);
    mvprintw(2, 10, "[ space - muda para o foco da janela de baixo ]");
    WINDOW *janelaMapa = newwin(22, 82, 3, 1);
    WINDOW *janelaComandos = newwin(15, 82, 26, 1);
    WINDOW *janelaChat = newwin(22, 35, 3, 85);
    WINDOW *janelaLogs = newwin(15, 35, 26, 85);

    pthread_mutex_lock(&tData.trinco);
    tData.janelaMapa = janelaMapa;
    tData.janelaChat = janelaChat;
    tData.janelaLogs = janelaLogs;
    tData.janelaComandos = janelaComandos;
    pthread_mutex_unlock(&tData.trinco);

    desenhaMapa(janelaMapa, 2);
    desenhaMapa(janelaComandos, 1);
    desenhaMapa(janelaChat, 2);
    desenhaMapa(janelaLogs, 2);

    // thread para lidar com a comunicacao com o backend
    pthread_t threadGerirBackendId;
    if (pthread_create(&threadGerirBackendId, NULL, threadGerirBackend, (void *) &tData) != 0) {
        perror("[ERRO] Erro ao criar a thread da comunicação do backend.\n");
        fecharCliente(&gameInfoFrontend);
        exit(-1);
    }

    trataTeclado(&tData);
    delwin(janelaMapa);
    delwin(janelaComandos);
    delwin(janelaLogs);
    delwin(janelaChat);
    endwin();

    pthread_mutex_lock(&tData.trinco);
    tData.continua = true;
    pthread_mutex_unlock(&tData.trinco);

    int forceExit = open(SRV_FIFO, O_WRONLY);
    if (forceExit == -1) {
        perror("[ERRO] Erro ao abrir o pipe do servidor.\n");
        fecharCliente(&gameInfoFrontend);
        exit(-1);
    }
    MsgBackEnd terminarPrograma = {
            .tipoMensagem = tipo_terminar_programa
    };
    strcpy(terminarPrograma.informacao.terminarPrograma.origem, "motor");
    strcpy(terminarPrograma.informacao.terminarPrograma.mensagem, "terminar");
    if (write(forceExit, &terminarPrograma, sizeof(terminarPrograma)) == -1) {
        perror("[ERRO] Erro ao escrever no pipe do servidor.\n");
        close(forceExit);
        fecharCliente(&gameInfoFrontend);
        exit(-1);
    }
    if (pthread_join(threadGerirBackendId, NULL) != 0) {
        perror("[ERRO] Erro ao esperar pela thread da comunicação do backend.\n");
        fecharCliente(&gameInfoFrontend);
        exit(-1);
    }
    pthread_mutex_destroy(&tData.trinco);
    fecharCliente(&gameInfoFrontend);
    exit(0);
}
