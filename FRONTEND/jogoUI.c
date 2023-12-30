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
            .ptrGameInfo = &gameInfoFrontend,
            .trinco = PTHREAD_MUTEX_INITIALIZER
    };

    // thread para lidar com a comunicacao com o backend
    pthread_t threadGerirBackendId;
    if (pthread_create(&threadGerirBackendId, NULL, threadGerirBackend, (void *) &tData) != 0) {
        perror("[ERRO] Erro ao criar a thread da comunicação do backend.\n");
        fecharCliente(&gameInfoFrontend);
        exit(-1);
    }

    initscr();
    //start_color();
    raw();
    noecho();
    keypad(stdscr, TRUE);
    attrset(A_DIM);
    //cbreak();
    //init_pair(1, COLOR_RED, COLOR_BLACK);
    //init_pair(2, COLOR_GREEN, COLOR_BLACK);
    //init_pair(3, COLOR_BLUE, COLOR_BLACK);
    //init_pair(4, COLOR_YELLOW, COLOR_BLUE);
    //attron(COLOR_PAIR(3));   // associa uma cor ao ecrã para desenhar uma moldura
    //desenhaMoldura(10,20,6,15);
    //wbkgd(janelaMapa, COLOR_PAIR(4));     // define backgound dos espaço vazio

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
    pthread_mutex_unlock(&tData.trinco);

    desenhaMapa(janelaMapa, 2);  // função exemplo que desenha o janela no ecrã
    desenhaMapa(janelaComandos, 1);  // função exemplo que desenha o janela no ecrã
    desenhaMapa(janelaChat, 3);  // função exemplo que desenha o janela no ecrã
    desenhaMapa(janelaLogs, 4);  // função exemplo que desenha o janela no ecrã
    trataTeclado(janelaMapa, janelaComandos); // função exemplo que trata o teclado
    delwin(janelaMapa);  // apaga a janela.
    delwin(janelaComandos);  // apaga a janela.
    delwin(janelaLogs);  // apaga a janela.
    delwin(janelaChat);  // apaga a janela.
    endwin();  // encerra a utilização do ncurses. Muito importante senão o terminal fica inconsistente (idem se sair por outras vias)

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
