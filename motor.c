#include <stdio.h>

#include "constantes.h"
#include "motor.h"

int main(int argc, char *argv[]) {
    char cmdTeste[TAMANHO_COMANDO];
    do {
        printf("Digite um comando: ");
        scanf("%s", cmdTeste);
        printf("Comando digitado: %s\n", cmdTeste);
    } while (verificaComando(cmdTeste) == 0);
    return 0;
}