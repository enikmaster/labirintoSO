#include <string.h>
#include <ctype.h>


#include "constantes.h"
#include "motor.h"

int verificaComando(char *comando) {
    const char listaComandos[][TAMANHO_COMANDO] = {"users", "bots", "bmov", "rbm", "begin", "end"};
    comando[strlen(comando) - 1] = '\0';
    for (int i = 0; i < strlen(comando); ++i) {
        comando[i] = tolower(comando[i]);
    }
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
    //strcpy(DURACAO, getenv("DURACAO"));
    // guarda numa variável o path para o ficheiro
    // depois é preciso abrir o ficheiro e ler o seu conteúdo
    // e guardar na estrutura de dados Tempo
}