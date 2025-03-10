#include "../constantes.h"
#include "jogoUI.h"

int main(int argc, char *argv[]) {
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
            .janelaTempoNivel = NULL,
            .ptrGameInfo = &gameInfoFrontend,
            .trinco = PTHREAD_MUTEX_INITIALIZER,
            .trincoMapa = PTHREAD_MUTEX_INITIALIZER
    };

    initscr();
    raw();
    noecho();
    keypad(stdscr, TRUE);
    attrset(A_DIM);
    cbreak();

    mvprintw(1, 10, "[ space - para inserir comandos ]");
    WINDOW *janelaMapa = newwin(MAPA_LINHAS + 2, MAPA_COLUNAS + 2, 3, 1);
    WINDOW *janelaTempoNivel = newwin(3, MAPA_COLUNAS + 2, MAPA_LINHAS + 4, 1);
    WINDOW *janelaComandos = newwin(15, MAPA_COLUNAS + 2, MAPA_LINHAS + 8, 1);
    WINDOW *janelaChat = newwin(MAPA_LINHAS + 2, 35, 3, MAPA_COLUNAS + 5);
    WINDOW *janelaLogs = newwin(15, 35, MAPA_LINHAS + 6, MAPA_COLUNAS + 5);

    pthread_mutex_lock(&tData.trinco);
    tData.janelaMapa = janelaMapa;
    tData.janelaTempoNivel = janelaTempoNivel;
    tData.janelaComandos = janelaComandos;
    tData.janelaChat = janelaChat;
    tData.janelaLogs = janelaLogs;
    pthread_mutex_unlock(&tData.trinco);

    desenhaMapa(janelaMapa, 2);
    desenhaMapa(janelaTempoNivel, 2);
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
    delwin(janelaTempoNivel);
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
        pthread_mutex_destroy(&tData.trinco);
        fecharCliente(&gameInfoFrontend);
        unlink(argv[1]);
        exit(-1);
    }
    MsgFrontEnd terminarPrograma = {
            .tipoMensagem = tipo_terminar_programa
    };
    strcpy(terminarPrograma.informacao.terminarPrograma.username, "motor");
    if (write(forceExit, &terminarPrograma, sizeof(terminarPrograma)) == -1) {
        perror("[ERRO] Erro ao escrever no pipe do servidor.\n");
        close(forceExit);
        pthread_mutex_destroy(&tData.trinco);
        fecharCliente(&gameInfoFrontend);
        unlink(argv[1]);
        exit(-1);
    }

    pthread_mutex_destroy(&tData.trinco);
    fecharCliente(&gameInfoFrontend);
    unlink(argv[1]);
    exit(0);
}
