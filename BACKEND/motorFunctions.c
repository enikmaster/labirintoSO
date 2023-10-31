#include <stdio.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <stdlib.h>
#include <time.h>
#include <ctype.h>
#include <errno.h>
#include <signal.h>

#include "constantes.h"
#include "motor.h"

char PathGameSetup[TAMANHO_PATH];
char PathMapaUm[TAMANHO_PATH];
char PathMapaDois[TAMANHO_PATH];
char PathMapaTres[TAMANHO_PATH];


void setGameSetup(GameSetup *gameSetup) {
    FILE *ficheiro;
    ficheiro = fopen(PathGameSetup, "r");
    if (ficheiro == NULL) {
        perror("Erro ao abrir o ficheiro %s\n");
        exit(1);
    } else {
        char conteudo[TAMANHO_CONTEUDO];
        char *endptr;
        int contador = 0;
        long int value;
        while (fgets(conteudo, sizeof(conteudo), ficheiro) != NULL) {
            value = strtol(conteudo, &endptr, 10);
            if (endptr == conteudo || *endptr != '\n' && *endptr != '\0') {
                contador = 0;
                perror("Erro ao converter string para inteiro\n");
                exit(1);
            } else {
                switch (++contador) {
                    case 1:
                        //gameSetup->ptrSetup->inscricao = (int) value;
                        break;
                    case 2:
                        //gameSetup->ptrSetup->duracao = (int) value;
                        break;
                    case 3:
                        //gameSetup->ptrSetup->decremento = (int) value;
                        break;
                    case 4:
                        //gameSetup->ptrSetup->minJogadores = (int) value;
                        break;
                    default:
                        contador = 0;
                        perror("Erro ao converter string para inteiro\n");
                        exit(1);
                }

            }
        }
    }
    fclose(ficheiro);
}

void preencheMapa(pMapa novo, FILE *ficheiro) {
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
                    pPosicao novaMeta = malloc(sizeof(Posicao));
                    if (novaMeta == NULL) {
                        perror("Erro ao alocar memória para uma posição\n");
                        free(novaMeta);
                        fclose(ficheiro);
                        exit(1);
                    }
                    novo->ptrMeta = novaMeta;
                    novo->ptrMeta->x = x;
                    novo->ptrMeta->y = y;
                    novo->ptrMeta->next = NULL;
                }
                if (y == MAPA_LINHAS - 1) {
                    pPosicao thisInicioHeader = novo->ptrInicioHeader;
                    while (thisInicioHeader != NULL) {
                        // garante que vai para o fim da lista
                        thisInicioHeader = thisInicioHeader->next;
                    }
                    pPosicao novoInicio = malloc(sizeof(Posicao));
                    if (novoInicio == NULL) {
                        perror("Erro ao alocar memória para uma posição\n");
                        free(novoInicio);
                        fclose(ficheiro);
                        exit(1);
                    }
                    thisInicioHeader = novoInicio;
                    thisInicioHeader->x = x;
                    thisInicioHeader->y = y;
                    thisInicioHeader->next = NULL;
                }
            }
        }
    }
}

void loadMapa(pMapa mapa, int nivel) {
    FILE *ficheiro;
    pMapa thisMapa = mapa;
    if (nivel < 1 || nivel > 3) {
        perror("Nível inválido\n");
        exit(1);
    }

    for (int i = nivel; i <= MAX_LEVELS; ++i) {
        while (thisMapa != NULL) {
            // garante que vai para o fim da lista
            thisMapa = thisMapa->next;
        }

        if (i == 1)
            ficheiro = fopen(PathMapaUm, "r");
        if (i == 2)
            ficheiro = fopen(PathMapaDois, "r");
        if (i == 3)
            ficheiro = fopen(PathMapaTres, "r");
        if (ficheiro == NULL) {
            perror("Erro ao abrir o ficheiro.\n");
            fclose(ficheiro);
            exit(1);
        }
        pMapa novo;
        novo = malloc(sizeof(Mapa));
        if (novo == NULL) {
            perror("Erro ao alocar memória para um mapa\n");
            free(novo);
            fclose(ficheiro);
            exit(1);
        }
        thisMapa = novo;
        novo->ptrMeta = NULL;
        novo->ptrInicioHeader = NULL;
        preencheMapa(novo, ficheiro);
        novo->next = NULL;
        thisMapa->next = NULL;
        fclose(ficheiro);
    }
}

void desenhaMapa(pMapa mapa) {
    pMapa thisMapa = mapa;
    while (thisMapa != NULL) {
        for (int y = 0; y < MAPA_LINHAS; ++y) {
            for (int x = 0; x < MAPA_COLUNAS; ++x) {
                printw("%c", thisMapa->mapa[y][x]);
            }
            printw("\n");
        }
        thisMapa = thisMapa->next;
    }
    refresh();
}

int verificaComando(char *comando) {
    const char listaComandos[][TAMANHO_COMANDO] = {"users", "bots", "bmov", "rbm", "begin", "end", "text-bots"};
    comando[strlen(comando) - 1] = '\0';
    for (int i = 0; i < strlen(comando); ++i) {
        comando[i] = tolower(comando[i]);
    }
    // comando sem argumentos
    if (strchr(comando, ' ') == NULL) {
        for (int i = 0; i <= strlen((const char *) listaComandos); ++i) {
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
                if (i == 6) return 6;
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
    while (gameSetup->ptrMapa->next != NULL) {
        free(gameSetup->ptrMapa->ptrMeta);

        pMapa nextMapa = gameSetup->ptrMapa->next;

        while (gameSetup->ptrMapa->ptrInicioHeader != NULL) {
            pPosicao nextPosicao = gameSetup->ptrMapa->ptrInicioHeader->next;

            free(gameSetup->ptrMapa->ptrInicioHeader);

            gameSetup->ptrMapa->ptrInicioHeader = nextPosicao;
        }
        free(gameSetup->ptrMapa);

        gameSetup->ptrMapa = nextMapa;
    }
    free(gameSetup);
}