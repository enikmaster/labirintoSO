#include <unistd.h>
#include "constantes.h"
#include "motor.h"


int main(int argc, char *argv[]) {
    int res = 0;
    int canal[2];
    //char pai[20];
    pathParaVariaveisAmbiente();
    GameSetup gameSetup;
    gameSetup.usersAtivos = 0;
    gameSetup.usersEspera = 0;
    gameSetup.tempoJogo = 0;
    gameSetup.nivel = 1;
    gameSetup.ptrMapa = NULL;
    loadMapa(&gameSetup, gameSetup.nivel);
    if (gameSetup.ptrMapa == NULL) {
        perror("Erro ao carregar o mapa\n");
        exit(1);
    }
    int controlo = 0;
    char comando[TAMANHO_COMANDO];
    do {
        printf("Digite um comando: ");
        fgets(comando, sizeof(comando), stdin);
        controlo = verificaComando(comando);
        // switch(controlo) {
        //     case 1:
        //         controlo = comandoUsers();
        //         break;
        // }

        if (controlo == 8) {
            /*pipe(canal);
            res = fork();
            if (res == 0) {
                printf("\n[PAI] PID: %d...\n", getpid());
                printf("\n[FILHO] PID: %d...\n", res);
                printf("\n[FILHO] vou lançar o 'bot'...\n");
                close(canal[0]);
                close(1);
                dup(canal[1]);
                close(canal[1]);
                execl("bot", "bot", NULL);
                exit(0);
            }*/
            //testaBots();
            controlo = 0;
        }
        if (controlo == 9) {
            desenhaMapa(gameSetup.ptrMapa->mapa);
            controlo = 0;
        }
    } while (controlo == 0);
    // lançar o bot
    // ver o ficheiro executa.c da aula04
    // 1 - criar um processo filho com um fork
    // não esquecer de guardar o pid do processo filho na resposta do fork
    // 2 - no processo filho, executar o bot
    // 3 - no processo pai, continuar a execução do jogo
    // 4 - quando o bot terminar, o processo filho termina
    /*do {
        pipe(canal);
        res = fork();
        if (res == 0) {
            // processo filho
            close(canal[0]);
            close(1);
            dup(canal[1]);
            close(canal[1]);
            execl("bot", "bot", pai, NULL);
            exit(0);
        }

    } while (res == 0);*/

    // matar o processo:
    // kill(pid, SIGKILL);
    // ou
    // kill -s SIGINT pid
    // ou
    // kill -9 pid
    // ou
    // kill -2 pid

    // Sinais muito importantes
    // sigaction(); - para tratar sinais, o que fazer quando receber um sinal
    // sigqueue(); - para enviar sinais, enviar sinais para outros processos
    // alarm(); - para enviar sinais ao fim de um determinado tempo
    // pause(); - para esperar por sinais

    // Sinais importante
    // SIGINT (2) - CTRL+C - termina a aplicação
    // SIGKILL (9) - kill -s SIGKILL pid - termina a aplicação
    // SIGUSR1 (10) - termina a aplicação
    // SIGUSR2 (12) - termina a aplicação
    // SIGALRM (14) - termina a aplicação
    // SIGCHLD (17) - ignora
    // SIGCONT (18) - fg/bg - continua
    // SIGSTOP (19) - CTRL+Z - stop

    // $ man 7 signal

    // o sigqueue é para enviar sinais com informação para o bot

    fecharJogo(&gameSetup);
    return 0;
}