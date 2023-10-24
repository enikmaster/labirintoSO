#include <string.h>
#include <stdio.h>

#include "motor.h"

int verificaComando(char *cmd) {
    if (strcmp(cmd, "users") == 0) {
        printf("Comando users válido\n");
    } else if (strcmp(cmd, "bots") == 0) {
        printf("Comando bots válido\n");
    } else if (strcmp(cmd, "bmov") == 0) {
        printf("Comando bmov válido\n");
    } else if (strcmp(cmd, "rbm") == 0) {
        printf("Comando rbm válido\n");
    } else if (strcmp(cmd, "begin") == 0) {
        printf("Comando begin válido\n");
    } else if (strcmp(cmd, "end") == 0) {
        printf("Comando end válido\n");
        return 1;
    } else {
        printf("Comando inválido\n");
    }
    return 0;
}
