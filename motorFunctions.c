#include <string.h>
#include <ctype.h>

#include "constantes.h"
#include "motor.h"

int verificaComando(char *cmd) {
    const char listaComandos[][TAMANHO_COMANDO] = {"users", "bots", "bmov", "rbm", "begin", "end"};
    cmd[strlen(cmd) - 1] = '\0';
    for (int i = 0; i < strlen(cmd); ++i) {
        cmd[i] = tolower(cmd[i]);
    }
    if (strchr(cmd, ' ') == NULL) {
        for (int i = 0; i <= strlen((const char *) listaComandos); ++i) {
            if (strcmp(cmd, listaComandos[i]) == 0) {
                /*switch (i) {
                    case 0:
                        printf("Comando %s válido\n", cmd);
                        fflush(stdin);
                        return 0;
                    case 1:
                        printf("Comando %s válido\n", cmd);
                        fflush(stdin);
                        return 0;
                    case 2:
                        printf("Comando %s válido\n", cmd);
                        fflush(stdin);
                        return 0;
                    case 3:
                        printf("Comando %s válido\n", cmd);
                        fflush(stdin);
                        return 0;
                    case 4:
                        printf("Comando %s válido\n", cmd);
                        fflush(stdin);
                        return 0;
                    case 5:
                        printf("Comando %s válido\n", cmd);
                        fflush(stdin);
                        return 1;
                    default:
                        break;
                }*/
                printf("Comando %s válido\n", cmd);
                fflush(stdin);
                return (strcmp(cmd, "end") == 0) ? 1 : 0;
            }
        }
        printf("Comando %s inválido\n", cmd);
        fflush(stdin);
        return 0;
    }
    if (strchr(cmd, ' ') != NULL) {
        char cmdAux[TAMANHO_COMANDO];
        char argAux[TAMANHO_COMANDO];
        char argControl[TAMANHO_COMANDO];
        memset(cmdAux, 0, sizeof(cmdAux));
        memset(argAux, 0, sizeof(argAux));
        memset(argControl, 0, sizeof(argControl));
        int nArgumentos = sscanf(cmd, "%s %s %s", cmdAux, argAux, argControl);
        if (nArgumentos > 2) {
            printf("Comando %s inválido\n", cmd);
            fflush(stdin);
            return 0;
        }
        if (strcmp(cmdAux, "kick") == 0) {
            printf("Comando %s válido\n", cmd);
            fflush(stdin);
            return 0;
        }
        printf("Comando %s inválido\n", cmd);
    }
    fflush(stdin);
    return 0;
}
