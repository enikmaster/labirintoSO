#include "constantes.h"
#include "jogoUI.h"


int main(int argc, char *argv[]) {
//  gcc -o ncurses-exemplo-1 ncurses-exemplo-1.c -lncurses
//  gcc -o ncurses-exemplo-2 ncurses-exemplo-2.c -lncurses

    int controlo = 0;
    char comando[TAMANHO_NAMES] = {'\0'};

    initscr();
    //start_color();
    raw();
    noecho();
    keypad(stdscr, TRUE);
    attrset(A_DIM);
    //cbreak();
    //init_pair(1, COLOR_RED, COLOR_BLACK);
    //init_pair(2, COLOR_GREEN, COLOR_BLACK);
    //init_pair(3, COLOR_BLUE, COLOR_BLACK);
    //init_pair(4, COLOR_YELLOW, COLOR_BLUE);
    //attron(COLOR_PAIR(3));   // associa uma cor ao ecrã para desenhar uma moldura
    //desenhaMoldura(10,20,6,15);
    //wbkgd(janelaTopo, COLOR_PAIR(4));     // define backgound dos espaço vazio





    mvprintw(1, 10, "[ Digite um comando ]");  // mensagem fora da janela, na linha 1, coluna 10 do ecrã
    mvprintw(2, 10, "[ space - muda para o foco da janela de baixo / q - sair ]"); // mensagem fora da janela, na linha 2, coluna 10 do ecrã
    WINDOW *janelaTopo = newwin(22, 82, 3, 1);  // Criar janela para a matriz de jogo, tendo os parametro numero de linhas,numero de colunas, posição onde começa a janela  e posição onde termina
    WINDOW *janelaBaixo = newwin(15, 82, 26, 1);
    desenhaMapa(janelaTopo, 2);  // função exemplo que desenha o janela no ecrã
    desenhaMapa(janelaBaixo, 1);  // função exemplo que desenha o janela no ecrã

    if (argc != 2) {
        mvprintw(1, 10, "Insira o numero correto de parametros.\n");
        wrefresh(janelaTopo);
        fflush(stdin);
        endwin();
        return 0;
    } else {
        trataTeclado(janelaTopo, janelaBaixo); // função exemplo que trata o teclado
        wclear(janelaTopo); // função que limpa o ecrã
        wrefresh(janelaTopo);  // função que faz atualização o ecrã com as operações realizadas anteriormente
        delwin(janelaTopo);  // apaga a janela.
        wclear(janelaBaixo); // função que limpa o ecrã
        wrefresh(janelaBaixo); // função que faz atualiza o ecrã com as operações realizadas anteriormente
        delwin(janelaBaixo);  // apaga a janela.
        endwin();  // encerra a utilização do ncurses. Muito importante senão o terminal fica inconsistente (idem se sair por outras vias)

    }
};
