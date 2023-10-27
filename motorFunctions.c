#include <string.h>
#include <ctype.h>

#include "constantes.h"
#include "motor.h"

char pathDuracao[TAMANHO_PATH];
char pathMapa1[TAMANHO_PATH];
char pathMapa2[TAMANHO_PATH];
char pathMapa3[TAMANHO_PATH];

void definirTempoInicial(Tempo *tempoJogo) {
    FILE *ficheiro;
    ficheiro = fopen(pathDuracao, "r");
    if (ficheiro == NULL) {
        perror("Erro ao abrir o ficheiro %s\n");
        exit(1);
    }
    fscanf(ficheiro, "%d %d %d", &tempoJogo->inscricao, &tempoJogo->duracao, &tempoJogo->decremento);
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
    strcpy(pathDuracao, getenv("TEMPO_CONFIG"));
    strcpy(pathMapa1, getenv("MAPA_NIVEL_1.txt"));
    strcpy(pathMapa2, getenv("MAPA_NIVEL_2.txt"));
    strcpy(pathMapa3, getenv("MAPA_NIVEL_3.txt"));
    // guarda numa variável o path para o ficheiro
    // depois é preciso abrir o ficheiro e ler o seu conteúdo
    // e guardar na estrutura de dados Tempo
}