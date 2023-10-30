#include "constantes.h"
#include "jogoUI.h"



int verificaComandoUI(char *comando) {
    const char listaComandos[][TAMANHO_COMANDO] =
            {"players", "msg", "exit"};
    comando[strlen(comando) - 1] = '\0';
    for (int i = 0; i < strlen(comando); ++i) {
        comando[i] = tolower(comando[i]);
    }
    // se for só uma palavra
    if (strchr(comando, ' ') == NULL) {
        for (int i = 0; i <= strlen((const char *) listaComandos); ++i) {
            if (strcmp(comando, listaComandos[i]) == 0) {
                /*switch (i) {
                    case 0:
                        printf("Comando %s válido\n", comando);
                        fflush(stdin);
                        return 1;
                    case 2:
                        printf("Comando %s válido\n", comando);
                        fflush(stdin);
                        return 0;
                }*/
                if (strcmp(comando, "msg")==0) {
                    printf("Comando %s inválido\n", comando);
                    fflush(stdin);
                    return 0;
                }
                printf("Comando %s válido\n", comando);
                fflush(stdin);
                return (strcmp(comando, "exit") == 0) ? 1 : 0;
            }
        }
        printf("Comando %s inválido\n", comando);
        fflush(stdin);
        return 0;
    }
    // se quer mandar mensagem
    if (strchr(comando, ' ') != NULL) {
        char comandoTemp[TAMANHO_COMANDO] = {'\0'};
        char userToMessage[TAMANHO_COMANDO] = {'\0'};
        char msgToSend[TAMANHO_COMANDO] = {'\0'};
        char argAux[TAMANHO_COMANDO] = {'\0'};
        int nArgumentos = sscanf(comando, "%s %s %s %s", comandoTemp, userToMessage, msgToSend, argAux);
        if (nArgumentos > 3) {
            printf("Comando %s inválido\n", comando);
            fflush(stdin);
            return 0;
        }
        if (strcmp(comandoTemp, "msg") == 0 && nArgumentos == 3) {
            if (checkIfUserAtivo(userToMessage)) {}
            printf("Comando %s válido\n", comando);
            fflush(stdin);
            return 0;
        }
        printf("Comando %s inválido\n", comando);
    }
    fflush(stdin);
    return 0;
}


bool checkIfUserAtivo(char *userToMessage) { return false;}

void desenhaMoldura(int alt, int comp, int linha, int coluna) {
    --linha;
    --coluna;
    alt+=2;
    comp+=2;
    // desenha barras verticais laterais
    for (int l=linha; l<=linha+alt-1; ++l) {
        mvaddch(l, coluna, '|');            // MoVe para uma posição e ADDiciona um CHar lá
        mvaddch(l, coluna+comp-1, '|');
    }
    // desenha as barras horizontais
    for (int c=coluna; c<=coluna+comp-1; ++c) {
        mvaddch(linha, c, '-');
        mvaddch(linha+alt-1, c, '-');
    }
    // desenha os cantos
    mvaddch(linha, coluna, '+');
    mvaddch(linha, coluna+comp-1, '+');
    mvaddch(linha+alt-1, coluna, '+');
    mvaddch(linha+alt-1,coluna+comp-1,'+');
}

void desenhaMapa(WINDOW *janela, int tipo)
{

    // quando temos o scroll ativo, não deveremos ter a borda desenhada na janela para não termos o problema escrever em cima das bordas
    if (tipo == 1)
    {
        scrollok(janela, TRUE); // liga o scroll na "janela".
        wprintw(janela, "\n #> ");
    }
    else
    {
        keypad(janela, TRUE); // para ligar as teclas de direção (aplicar à janela)
        wclear(janela);// limpa a janela
        wborder(janela, '|', '|', '-', '-', '+', '+', '+', '+'); // Desenha uma borda. Nota importante: tudo o que escreverem, devem ter em conta a posição da borda
    }
    refresh(); // necessário para atualizar a janela
    wrefresh(janela); // necessário para atualizar a janela
}

void trataTeclado(WINDOW *janelaTopo, WINDOW *janelaBaixo)
{
    keypad(janelaTopo, TRUE);       // para ligar as teclas de direção (aplicar à janelaTopo)
    wmove(janelaTopo, 1, 1);        // posiciona o cursor,visualmente, na posicao 1,1 da janelaTopo
    //nota a posição é relativa à janelaTopo e não ao ecrã.
    int tecla = wgetch(janelaTopo); // MUITO importante: o input é feito sobre a janela em questão, neste caso na janelaTopo
    char comando[100];             // string que vai armazenar o comando

    while (tecla != 113) // trata as tecla até introduzirem a letra q. O código asci de q é 113
    {

        if (tecla == KEY_UP) // quando o utilizador introduz a seta para cima
        {
            desenhaMapa(janelaTopo, 2); // atualiza toda a janela
            mvwprintw(janelaTopo, 1, 1, "Estou a carregar na tecla UP na posição 1,1 "); // escreve a palavra UP na posição  1,1.
            //Nota: não escreve na posição 0,0 porque está lá a borda da janela que foi criada anteriormente
            wrefresh(janelaTopo);// // necessário para atualizar a janelaTopo. Nota: é apenas esta janela que pretendemos atualizar
        }
        else if (tecla == KEY_RIGHT)
        {
            desenhaMapa(janelaTopo, 2);
            mvwprintw(janelaTopo, 1, 1, "Estou a carregar na tecla RIGHT na posição 1,1");
            wrefresh(janelaTopo);
        }
        else if (tecla == KEY_LEFT)
        {
            desenhaMapa(janelaTopo, 2);
            mvwprintw(janelaTopo, 1, 1, "Estou a carregar na tecla LEFT na posição 1,1");
            wrefresh(janelaTopo);
        }
        else if (tecla == KEY_DOWN)
        {
            desenhaMapa(janelaTopo, 2);
            mvwprintw(janelaTopo, 1, 1, "Estou a carregar na tecla DOWN na posição 1,1");
            wrefresh(janelaTopo);
        }
        else if (tecla == ' ') // trata a tecla espaço
        {  // a tecla espaço ativa a janela inferior e tem o scroll ativo
            //  o wprintw e o wgetstr devem ser utilizados em janelas que tem o scroll ativo.
            wclear(janelaBaixo);
            echo(); // habilita a maneira como o utilizador visualiza o que escreve
            // ou seja volta a ligar o echo para se ver o que se está a escrever
            wprintw(janelaBaixo, "\n #> "); // utilizada para imprimir.
            //nota como a janelaBaixo tem o scroll ativo, ele vai imprimindo linha a linha
            wgetstr(janelaBaixo, comando);  // para receber do teclado uma string na "janelaBaixo" para a variavel comando
            //wprintw(janelaBaixo, "\n [%s] ", comando);

            if (verificaComandoUI(comando) == 1) return;

            noecho(); //voltar a desabilitar o que o utilizador escreve
            wrefresh(janelaBaixo); //sempre que se escreve numa janela, tem de se fazer refresh
        }
        wmove(janelaTopo, 1, 1); // posiciona o cursor (visualmente) na posicao 1,1 da janelaTopo
        tecla = wgetch(janelaTopo); //espera que o utilizador introduza um inteiro. Importante e como já referido anteriormente introduzir a janela onde queremos receber o input
    }
}