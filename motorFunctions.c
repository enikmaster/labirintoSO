#include <string.h>
#include <ctype.h>

#include "constantes.h"
#include "motor.h"

char pathGameSetup[TAMANHO_PATH];
char pathMapa1[TAMANHO_PATH];
char pathMapa2[TAMANHO_PATH];
char pathMapa3[TAMANHO_PATH];

void setGameSetup(GameSetup *gameSetup) {
    FILE *ficheiro;
    ficheiro = fopen(pathGameSetup, "r");
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
                        perror("Erro ao converter string para inteiro\n");
                        exit(1);
                }

            }
        }
    }
    fclose(ficheiro);
}

int getNumeroLinhas(FILE *ficheiro) {
    int nLinhas = 0;
    char caracter;
    while ((caracter = fgetc(ficheiro)) != EOF)
        if (caracter == '\n')
            ++nLinhas;
    if (caracter != '\n' && ftell(ficheiro) > 0)
        ++nLinhas;
    return nLinhas;
}

void loadMapa(Mapa *mapa) {
    FILE *ficheiro;
    ficheiro = fopen(pathMapa1, "r");
    if (ficheiro == NULL) {
        perror("Erro ao abrir o ficheiro %s\n");
        exit(1);
    }
    int nLinhas = getNumeroLinhas(ficheiro);
    int x = 1;
    int y = 1;

    // correr o ficheiro e ler cada caracter
    // se o caracter for 'x' criar uma parede
    // se o caracter for ' ' ignorar
    pWall wallAtual = NULL;
    pWall wallPrev = NULL;
    while (y <= nLinhas) {
        // ler o caracter
        char caracter = fgetc(ficheiro);
        if (caracter == 'x') {
            // criar uma parede

            wallAtual = malloc(sizeof(Wall));
            if (wallAtual == NULL) {
                perror("Erro ao alocar memória para uma parede\n");
                free(wallAtual);
                fclose(ficheiro);
                exit(1);
            }
            wallAtual->identificador = 'x';
            wallAtual->posicao.x = x++;
            wallAtual->posicao.y = y;
            wallAtual->next = NULL;
            if (mapa->ptrWallsHeader == NULL) {
                mapa->ptrWallsHeader = wallAtual;
                wallPrev = mapa->ptrWallsHeader;
            } else {
                wallPrev->next = wallAtual;
                wallPrev = wallAtual;
            }
        }
        if (caracter == ' ') {
            if (y == 1) {
                // criar a posição de meta
                // TODO: criar a posição de meta
            }
            if (y == nLinhas) {
                // criar uma posição de inicio
                // adicionar a posição de inicio à lista de posições de inicio
                // TODO: criar uma posição de inicio
                // TODO: adicionar a posição de inicio à lista de posições de inicio
            }
            ++x;
        }
        if (caracter == '\n') {
            ++y;
            x = 1;
        }
    }
    fclose(ficheiro);
}

int verificaComando(char *comando) {
    const char listaComandos[][TAMANHO_COMANDO] = {"users", "bots", "bmov", "rbm", "begin", "end"};
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
    strcpy(pathGameSetup, getenv("GAME_SETUP"));
    strcpy(pathMapa1, getenv("MAPA_NIVEL_1.txt"));
    strcpy(pathMapa2, getenv("MAPA_NIVEL_2.txt"));
    strcpy(pathMapa3, getenv("MAPA_NIVEL_3.txt"));
    // guarda numa variável o path para o ficheiro
    // depois é preciso abrir o ficheiro e ler o seu conteúdo
    // e guardar na estrutura de dados Tempo
}