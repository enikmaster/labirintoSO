#include "constantes.h"
#include "motor.h"

char PathGameSetup[TAMANHO_PATH];
char PathMapaUm[TAMANHO_PATH];
char PathMapaDois[TAMANHO_PATH];
char PathMapaTres[TAMANHO_PATH];

void sinalizaBot(int sig, siginfo_t *info, void *context) {
    union sigval value;
    if (sigqueue(pidBot, sig, value) == -1) {
        perror("Não foi possivel enviar o sinal e tem o erro: ");
        return;
    }
    pidBot = -1;
}

void setGameSetup(GameSetup *gameSetup) {
    FILE *ficheiro;
    ficheiro = fopen(PathGameSetup, "r");
    // descomentar para debugging
    //ficheiro = fopen("BACKEND/GAME_SETUP", "r");
    if (ficheiro == NULL) {
        perror("[ERRO] Erro ao abrir o ficheiro %s\n");
        exit(-1);
    } else {
        char conteudo[TAMANHO_CONTEUDO];
        char *endptr;
        int contador = 0;
        long int value;
        gameSetup->ptrSetup = malloc(sizeof(Setup));
        while (fgets(conteudo, sizeof(conteudo), ficheiro) != NULL) {
            value = strtol(conteudo, &endptr, 10);
            if (endptr == conteudo || *endptr != '\n' && *endptr != '\0') {
                contador = 0;
                perror("[ERRO] Erro ao converter string para inteiro\n");
                exit(-1);
            } else {
                switch (++contador) {
                    case 1:
                        gameSetup->ptrSetup->inscricao = (int) value;
                        break;
                    case 2:
                        gameSetup->ptrSetup->duracao = (int) value;
                        break;
                    case 3:
                        gameSetup->ptrSetup->decremento = (int) value;
                        break;
                    case 4:
                        gameSetup->ptrSetup->minJogadores = (int) value;
                        break;
                    default:
                        contador = 0;
                        perror("[ERRO] Erro ao converter string para inteiro\n");
                        exit(-1);
                }

            }
        }
    }
    fclose(ficheiro);
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
            free(novoMapa);
            fclose(ficheiro);
            exit(-1);
        }
        novoMapa->ptrMeta = NULL;
        novoMapa->ptrInicioHeader = NULL;
        novoMapa->next = NULL;
        preencheMapa(novoMapa, ficheiro);
        if (gameSetup->ptrMapa == NULL) {
            gameSetup->ptrMapa = novoMapa;
            thisMapa = gameSetup->ptrMapa;
        } else {
            thisMapa->next = novoMapa;
            thisMapa = novoMapa;
        }
        fclose(ficheiro);
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

int verificaComando(char *comando) {
    const char listaComandos[][TAMANHO_COMANDO] = {"users", "bots", "bmov", "rbm", "begin", "end", "test_bot",
                                                   "test_mapa"};
    comando[strlen(comando) - 1] = '\0';
    for (int i = 0; i < strlen(comando); ++i) {
        comando[i] = tolower(comando[i]);
    }
    // comando sem argumentos
    if (strchr(comando, ' ') == NULL) {
        for (int i = 0; i < sizeof(listaComandos) / sizeof(listaComandos[0]); ++i) {
            if (strcmp(comando, listaComandos[i]) == 0) {
                /*switch (i) {
                    case 0:
                        printf("Comando %s válido\n", comando);
                        fflush(stdin);
                        return 1;
                    case 1:
                        printf("Comando %s válido\n", comando);
                        fflush(stdin);
                        return 2;
                    case 2:
                        printf("Comando %s válido\n", comando);
                        fflush(stdin);
                        return 3;
                    case 3:
                        printf("Comando %s válido\n", comando);
                        fflush(stdin);
                        return 4;
                    case 4:
                        printf("Comando %s válido\n", comando);
                        fflush(stdin);
                        return 5;
                    case 5:
                        printf("Comando %s válido\n", comando);
                        fflush(stdin);
                        return 6;
                    default:
                        return 0;
                }*/
                printf("Comando %s válido\n", comando);
                fflush(stdin);
                if (i == 6) return 8;
                if (i == 7) return 9;
                return (strcmp(comando, "end") == 0) ? 1 : 0;
            }
        }
        printf("Comando %s inválido\n", comando);
        fflush(stdin);
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
            printf("Comando %s inválido\n", comando);
            fflush(stdin);
            return 0;
        }
        if (strcmp(comandoTemp, "kick") == 0) {
            printf("Comando %s válido\n", comando);
            fflush(stdin);
            return 0;
            //return 7;
        }
        printf("Comando %s inválido\n", comando);
    }
    fflush(stdin);
    return 0;
}

void pathParaVariaveisAmbiente() {
    strcpy(PathGameSetup, getenv("GAME_SETUP"));
    strcpy(PathMapaUm, getenv("PATH_MAPA_UM"));
    strcpy(PathMapaDois, getenv("PATH_MAPA_DOIS"));
    strcpy(PathMapaTres, getenv("PATH_MAPA_TRES"));
}

void fecharJogo(GameSetup *gameSetup) {
    pMap libertaMapa;
    while (gameSetup->ptrMapa != NULL) {
        libertaMapa = gameSetup->ptrMapa;
        pPosition libertaPosicao;
        while (gameSetup->ptrMapa->ptrInicioHeader != NULL) {
            libertaPosicao = gameSetup->ptrMapa->ptrInicioHeader;
            gameSetup->ptrMapa->ptrInicioHeader = gameSetup->ptrMapa->ptrInicioHeader->next;
            free(libertaPosicao);
        }
        free(gameSetup->ptrMapa->ptrMeta);
        gameSetup->ptrMapa = gameSetup->ptrMapa->next;
        free(libertaMapa);
    }
    free(gameSetup->ptrSetup);
}

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
            perror("Erro no sigaction! Tem o erro: ");
            printf("\n");
        }
        while (pidBot != -1) {
            char dadosDoBot[50];
            setbuf(stdout, NULL);
            close(fd[1]);
            memset(dadosDoBot, 0, sizeof(dadosDoBot));
            if (read(fd[0], &dadosDoBot, sizeof(dadosDoBot)) == -1) {
                perror("Erro a ler com o erro: ");
                printf("\n");
            }
            if (strlen(dadosDoBot) > 5 && pidBot != -1)
                printf("Recebi: %s", dadosDoBot);

        }
        // TODO: fazer o encerramento do bot
        waitpid(pidBot, &status, 0);
        printf("\nEncerramos o bot!\n");
        close(fd[0]);
    }
}