#include "../constantes.h"
#include "motor.h"
#include <pthread.h>

void *threadTimers(void *arg) {
    ThreadData *TData = (ThreadData *) arg;
    do {
        //decrementaUmSegundo();
        sleep(1);
    } while (TData->continua);
    pthread_exit(NULL);
}



// TODO: uma função para enviar uma mensagem a todos os clientes com as informações do jogo

// TODO: uma função para enviar uma mendagem a todos os clientes com o mapa atualizado

// TODO: 3 Threads
//  1 - thread para lidar com coisas temporais (tempo de jogo, tempo de inscrição, tempo de decremento, etc)
//  2 - thread para lidar com os comandos dos clientes (jogadores) e bots
//      quando um user faz um movimento, a posição do user é atualizada no mapa e é enviado a todos os clientes
//  3 - thread para lidar com os comandos do servidor (administração do jogo