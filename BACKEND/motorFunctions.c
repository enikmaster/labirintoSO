#include "../constantes.h"
#include "motor.h"

char PathMapaUm[TAMANHO_PATH];
char PathMapaDois[TAMANHO_PATH];
char PathMapaTres[TAMANHO_PATH];
char Inscricao[TAMANHO_PATH];
char NPlayers[TAMANHO_PATH];
char Duracao[TAMANHO_PATH];
char Decremento[TAMANHO_PATH];

void sinalizaBot(int sig, siginfo_t *info, void *context) {
    union sigval value = {.sival_int = 0};
    if (sigqueue(pidBot, sig, value) == -1) {
        perror("[INFO] Não foi possivel enviar o sinal e tem o erro: ");
        return;
    }
    pidBot = -1;
}

void setGameSetup(GameSetup *gameSetup) {
    gameSetup->ptrSetup = NULL;
    gameSetup->ptrUsersAtivosHeader = NULL;
    gameSetup->ptrUsersEsperaHeader = NULL;
    gameSetup->ptrMapa = NULL;
    gameSetup->ptrBotsHeader = NULL;
    gameSetup->jogoAtivo = false;
    gameSetup->usersAtivos = 0;
    gameSetup->usersEspera = 0;
    gameSetup->nivel = 1;
    char *endptr;
    gameSetup->ptrSetup = malloc(sizeof(Setup));
    if (gameSetup->ptrSetup == NULL) {
        perror("[ERRO] Erro ao alocar memória para o setup\n");
        free(gameSetup->ptrSetup);
        exit(-1);
    }

    gameSetup->ptrSetup->inscricao = strtol(Inscricao, &endptr, 10);
    if (*endptr != '\n' && *endptr != '\0') {
        perror("[ERRO] Erro ao converter string para inteiro\n");
        exit(-1);
    }
    gameSetup->ptrSetup->duracao = strtol(Duracao, &endptr, 10);
    if (*endptr != '\n' && *endptr != '\0') {
        perror("[ERRO] Erro ao converter string para inteiro\n");
        exit(-1);
    }
    gameSetup->ptrSetup->decremento = strtol(Decremento, &endptr, 10);
    if (*endptr != '\n' && *endptr != '\0') {
        perror("[ERRO] Erro ao converter string para inteiro\n");
        exit(-1);
    }
    gameSetup->ptrSetup->minJogadores = strtol(NPlayers, &endptr, 10);
    if (*endptr != '\n' && *endptr != '\0') {
        perror("[ERRO] Erro ao converter string para inteiro\n");
        exit(-1);
    }
}

void preencheMapa(pMap novo, FILE *ficheiro) {
    pPosition thisInicioHeader = NULL;
    for (int y = 0; y < MAPA_LINHAS; ++y) {
        for (int x = 0; x < MAPA_COLUNAS; ++x) {
            char caracter = fgetc(ficheiro);
            // caracteres a encontrar: 'x', ' ', '\n'
            if (caracter == '\n') {
                --x;
            }
            if (caracter == 'x') {
                novo->mapa[y][x] = 'x';
            }
            if (caracter == ' ') {
                novo->mapa[y][x] = ' ';
                if (y == 0) {
                    pPosition novaMeta = (pPosition) malloc(sizeof(Position));
                    if (novaMeta == NULL) {
                        perror("[ERRO] Erro ao alocar memória para uma posição\n");
                        free(novaMeta);
                        fclose(ficheiro);
                        exit(-1);
                    }
                    novo->ptrMeta = novaMeta;
                    novo->ptrMeta->x = x;
                    novo->ptrMeta->y = y;
                    novo->ptrMeta->next = NULL;
                }
                if (y == MAPA_LINHAS - 1) {
                    pPosition novoInicio = (pPosition) malloc(sizeof(Position));
                    if (novoInicio == NULL) {
                        perror("[ERRO] Erro ao alocar memória para uma posição\n");
                        free(novoInicio);
                        fclose(ficheiro);
                        exit(-1);
                    }
                    novoInicio->next = NULL;
                    novoInicio->x = x;
                    novoInicio->y = y;
                    if (novo->ptrInicioHeader == NULL) {
                        novo->ptrInicioHeader = novoInicio;
                        thisInicioHeader = novo->ptrInicioHeader;
                    } else {
                        thisInicioHeader->next = novoInicio;
                        thisInicioHeader = novoInicio;
                    }
                }
            }
        }
    }
}

void loadMapa(GameSetup *gameSetup, int nivel) {
    FILE *ficheiro;
    pMap thisMapa = NULL;
    if (nivel < 1 || nivel > 3) {
        perror("[ERRO] Nível inválido\n");
        exit(-1);
    }

    for (int i = nivel; i <= MAX_LEVELS; ++i) {
        if (i == 1) {
            ficheiro = fopen(PathMapaUm, "r");
            // descomentar para debugging
            //ficheiro = fopen("BACKEND/MAPA_UM.txt", "r");
        }
        if (i == 2) {
            ficheiro = fopen(PathMapaDois, "r");
            // descomentar para debugging
            //ficheiro = fopen("BACKEND/MAPA_DOIS.txt", "r");
        }
        if (i == 3) {
            ficheiro = fopen(PathMapaTres, "r");
            // descomentar para debugging
            //ficheiro = fopen("BACKEND/MAPA_TRES.txt", "r");
        }
        if (ficheiro == NULL) {
            perror("[ERRO] Erro ao abrir o ficheiro.\n");
            fclose(ficheiro);
            exit(-1);
        }
        pMap novoMapa = (pMap) malloc(sizeof(Map));
        if (novoMapa == NULL) {
            perror("[ERRO] Erro ao alocar memória para um mapa\n");
            while (gameSetup->ptrMapa != NULL) {
                pMap temp = gameSetup->ptrMapa;
                gameSetup->ptrMapa = gameSetup->ptrMapa->next;
                free(temp);
            }
            fclose(ficheiro);
            exit(-1);
        }
        novoMapa->ptrMeta = NULL;
        novoMapa->ptrInicioHeader = NULL;
        novoMapa->ptrRocksHeader = NULL;
        novoMapa->ptrBlocksHeader = NULL;
        novoMapa->next = NULL;
        preencheMapa(novoMapa, ficheiro);
        if (thisMapa == NULL) {
            gameSetup->ptrMapa = novoMapa;
            thisMapa = gameSetup->ptrMapa;
        } else {
            thisMapa->next = novoMapa;
            thisMapa = novoMapa;
        }
        fclose(ficheiro);
    }
}

int verificaComando(char *comando) {
    const char listaComandos[][TAMANHO_COMANDO] = {"users", "bots", "bmov", "rbm", "begin", "end",
            //"test_bot",
            //"test_mapa"
    };
    comando[strlen(comando) - 1] = '\0';
    for (int i = 0; i < strlen(comando); ++i)
        comando[i] = tolower(comando[i]);

    // comando sem argumentos
    if (strchr(comando, ' ') == NULL) {
        for (int i = 0; i < sizeof(listaComandos) / sizeof(listaComandos[0]); ++i) {
            if (strcmp(comando, listaComandos[i]) == 0) {
                // para a meta 1
                // printf("Comando %s válido\n", comando);
                // fflush(stdin);
                //if (i == 6) return 8;
                //if (i == 7) return 9;
                //return (strcmp(comando, "end") == 0) ? 1 : 0;
                // para a meta 2
                return i + 1;
            }
        }
        // printf("Comando %s inválido\n", comando);
        // fflush(stdin);
        return 0;
    }
    // comando com 1 argumento
    if (strchr(comando, ' ') != NULL) {
        char comandoTemp[TAMANHO_COMANDO];
        char argumentoTemp[TAMANHO_COMANDO];
        char argumentoCtrl[TAMANHO_COMANDO];
        memset(comandoTemp, 0, sizeof(comandoTemp));
        memset(argumentoTemp, 0, sizeof(argumentoTemp));
        memset(argumentoCtrl, 0, sizeof(argumentoCtrl));
        int nArgumentos = sscanf(comando, "%s %s %s", comandoTemp, argumentoTemp, argumentoCtrl);
        if (nArgumentos > 2) {
            //printf("Comando %s inválido\n", comando);
            //fflush(stdin);
            return 0;
        }
        if (strcmp(comandoTemp, "kick") == 0) {
            //printf("Comando %s válido\n", comando);
            //fflush(stdin);
            //return 0;
            return 7;
        }
        //printf("Comando %s inválido\n", comando);
    }
    fflush(stdin);
    return 0;
}

void pathParaVariaveisAmbiente() {
    strcpy(Inscricao, getenv("INSCRICAO"));
    strcpy(NPlayers, getenv("NPLAYERS"));
    strcpy(Duracao, getenv("DURACAO"));
    strcpy(Decremento, getenv("DECREMENTO"));
    strcpy(PathMapaUm, getenv("PATH_MAPA_UM"));
    strcpy(PathMapaDois, getenv("PATH_MAPA_DOIS"));
    strcpy(PathMapaTres, getenv("PATH_MAPA_TRES"));
}

// libertar memória
void fecharSetup(Setup *setup) {
    free(setup);
    setup = NULL;
}

void fecharUsers(pUser users) {
    pUserInfo libertaUserInfo;
    pPosition libertaPosicao;
    while (users != NULL) {
        libertaUserInfo = users->ptrUserInfo;
        libertaPosicao = libertaUserInfo->position;
        users = users->next;
        free(libertaPosicao);
        free(libertaUserInfo);
    }
}

void fecharUsersAtivosEspera(pUser usersAtivos, pUser usersEspera) {
    if (usersAtivos != NULL) fecharUsers(usersAtivos);
    if (usersEspera != NULL) fecharUsers(usersEspera);
}

void fecharPositions(pPosition posicoes) {
    pPosition libertaPosicao;
    while (posicoes != NULL) {
        libertaPosicao = posicoes;
        posicoes = posicoes->next;
        free(libertaPosicao);
    }
}

void fecharRocks(pRock rocks) {
    pRock libertaRock;
    pPosition libertaPosicao;
    while (rocks != NULL) {
        libertaRock = rocks;
        libertaPosicao = libertaRock->position;
        rocks = rocks->next;
        free(libertaPosicao);
        free(libertaRock);
    }
}

void fecharBlocks(pBlock blocks) {
    pBlock libertaBlock;
    pPosition libertaPosicao;
    while (blocks != NULL) {
        libertaBlock = blocks;
        libertaPosicao = libertaBlock->position;
        blocks = blocks->next;
        free(libertaPosicao);
        free(libertaBlock);
    }
}

void fecharMapas(pMap mapa) {
    pMap libertaMapa;
    pPosition libertaPosicao = NULL;
    while (mapa != NULL) {
        libertaMapa = mapa;
        if (libertaMapa->ptrMeta != NULL) free(libertaMapa->ptrMeta);
        if (libertaMapa->ptrInicioHeader != NULL) fecharPositions(libertaMapa->ptrInicioHeader);
        if (libertaMapa->ptrRocksHeader != NULL) fecharRocks(libertaMapa->ptrRocksHeader);
        if (libertaMapa->ptrBlocksHeader != NULL) fecharBlocks(libertaMapa->ptrBlocksHeader);
        mapa = mapa->next;
        free(libertaMapa);
    }
}

void fecharBots(pBot bots) {
    pBot libertaBot;
    while (bots != NULL) {
        libertaBot = bots;
        bots = bots->next;
        free(libertaBot);
    }
}

void fecharJogo(GameSetup *gameSetup) {
    // libertar a memória do setup
    if (gameSetup->ptrSetup != NULL) fecharSetup(gameSetup->ptrSetup);
    gameSetup->ptrSetup = NULL;
    // libertar a memória dos users
    fecharUsersAtivosEspera(gameSetup->ptrUsersAtivosHeader, gameSetup->ptrUsersEsperaHeader);
    // libertar a memória dos mapas
    if (gameSetup->ptrMapa != NULL) fecharMapas(gameSetup->ptrMapa);
    // libertar a memória dos bots
    if (gameSetup->ptrBotsHeader != NULL) fecharBots(gameSetup->ptrBotsHeader);
    // destruir os mutexes
    pthread_mutex_destroy(&gameSetup->mutexJogadores);
    pthread_mutex_destroy(&gameSetup->mutexMapa);
    pthread_mutex_destroy(&gameSetup->mutexBots);
    pthread_mutex_destroy(&gameSetup->mutexGeral);
}

void sigHandler(int sig) {
    if (sig == SIGINT) {
        printf("\n[INFO] A fechar o jogo...\n");
        exit(0);
    }
}

// meta 1
void testarBot() {
    int fd[2], status;

    if (pipe(fd) == -1) {
        printf("[ERRO] Erro na tentativa de lançamento do bot\n");
        exit(-1);
    }
    pidBot = fork();
    if (pidBot == 0) {
        // ações do filho (bot)
        close(STDOUT_FILENO);
        dup(fd[1]);
        close(fd[1]);
        close(fd[0]);
        if (execl("bot", "bot", "5", "10", NULL) == -1) {
            perror("[ERRO] Erro a executar bot! E tem o erro: ");
            printf("\n");
            pidBot = -1;
            exit(-1);
        }
    } else if (pidBot > 0) {
        struct sigaction sac = {0};
        sac.sa_sigaction = sinalizaBot;
        if (sigaction(SIGINT, &sac, NULL) == -1) {
            perror("[ERRO] Erro no sigaction! Tem o erro: ");
            printf("\n");
        }
        while (pidBot != -1) {
            char dadosDoBot[50];
            setbuf(stdout, NULL);
            close(fd[1]);
            memset(dadosDoBot, 0, sizeof(dadosDoBot));
            if (read(fd[0], &dadosDoBot, sizeof(dadosDoBot)) == -1) {
                perror("[ERRO] Erro a ler com o erro: ");
                printf("\n");
            }
            if (strlen(dadosDoBot) > 5 && pidBot != -1)
                printf("Recebi: %s", dadosDoBot);
        }
        waitpid(pidBot, &status, 0);
        printf("\nEncerramos o bot!\n");
        close(fd[0]);
    }
}

void desenhaMapa(char mapa[MAPA_LINHAS][MAPA_COLUNAS]) {
    system("clear");
    //printf("\e[H\e[2J\e[3J");
    for (int y = 0; y < MAPA_LINHAS; ++y) {
        for (int x = 0; x < MAPA_COLUNAS; ++x) {
            printf("%c", mapa[y][x]);
        }
        printf("\n");
    }
}