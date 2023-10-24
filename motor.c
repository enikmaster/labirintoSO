#include <stdio.h>
#include <string.h>
#include "motor.h"

int main(int argc, char *argv[]) {
    char cmdTeste[TAMANHO_COMANDO];
    do {
        printf("Digite um comando: ");
        scanf("%s", cmdTeste);
        printf("Comando digitado: %s\n", cmdTeste);
        if (strcmp(cmdTeste, "users") == 0) {
            printf("Comando users válido\n");
        } else if (strcmp(cmdTeste, "bots") == 0) {
            printf("Comando bots válido\n");
        } else if (strcmp(cmdTeste, "bmov") == 0) {
            printf("Comando bmov válido\n");
        } else if (strcmp(cmdTeste, "rbm") == 0) {
            printf("Comando rbm válido\n");
        } else if (strcmp(cmdTeste, "begin") == 0) {
            printf("Comando begin válido\n");
        } else if (strcmp(cmdTeste, "end") == 0) {
            printf("Comando end válido\n");
        } else {
            printf("Comando inválido\n");
        }
    } while (strcmp(cmdTeste, "end") != 0);
    return 0;
}