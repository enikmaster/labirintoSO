#include "../constantes.h"
#include "motor.h"

int comandoUsers(GameSetup *gameSetup) {
    pUser ptrUser;
    if (gameSetup->usersAtivos == 0) {
        printf("[INFO] Não existem jogadores ativos.\n");
        fflush(stdout);
    } else {
        ptrUser = gameSetup->ptrUsersAtivosHeader;
        while (ptrUser != NULL) {
            printf("[INFO] Jogador ativo %d: %s - '%c'\n", ptrUser->pid, ptrUser->username, ptrUser->identificador);
            fflush(stdout);
            ptrUser = ptrUser->next;
        }
    }
    if (gameSetup->usersEspera == 0) {
        printf("[INFO] Não existem jogadores em espera.\n");
        fflush(stdout);
    } else {
        ptrUser = gameSetup->ptrUsersEsperaHeader;
        while (ptrUser != NULL) {
            printf("[INFO] Jogador em espera %d: %s - '%c'\n", ptrUser->pid, ptrUser->username, ptrUser->identificador);
            fflush(stdout);
            ptrUser = ptrUser->next;
        }
    }
    return 0;
}

int comandoBots() {
    // TODO: implementar porque não sei bem o que fazer aqui
    return 0;
}

int comandoBmov() {
    return 0;
}

int comandoRbm() {
    return 0;
}

int comandoBegin() {
    return 0;
}

int comandoEnd() {
    return 0;
}

int comandoKick(GameSetup *gameSetup, char *username) {
    // TODO: enviar mensagem ao cliente a dizer que foi kickado
    pUser ptrUser, ptrUserAnterior;
    if (gameSetup->usersAtivos == 0) {
        printf("[INFO] Não existem jogadores ativos.\n");
        fflush(stdout);
    } else {
        ptrUser = gameSetup->ptrUsersAtivosHeader;
        ptrUserAnterior = NULL;
        username[strlen(username) - 1] = '\0';
        for (int i = 0; i < strlen(username); ++i)
            username[i] = tolower(username[i]);
        char usernameNomalized[strlen(username)];

        while (ptrUser != NULL) {
            strcpy(usernameNomalized, gameSetup->ptrUsersAtivosHeader->username);
            for (int i = 0; i < strlen(usernameNomalized); ++i)
                usernameNomalized[i] = tolower(usernameNomalized[i]);
            if (strcmp(usernameNomalized, username) == 0) {
                // TODO: avisar o jogador que vai ser desligado
                (ptrUserAnterior == NULL)
                ? (gameSetup->ptrUsersAtivosHeader = ptrUser->next)
                : (ptrUserAnterior->next = ptrUser->next);
                gameSetup->usersAtivos--;
                printf("[INFO] Jogador kickado %d: %s - '%c'\n", ptrUser->pid, ptrUser->username,
                       ptrUser->identificador);
                fflush(stdout);
                free(ptrUser->position);
                free(ptrUser);
                return 0;
            }
            ptrUserAnterior = ptrUser;
            ptrUser = ptrUser->next;
        }
        printf("[INFO] Não foi encontrado nenhum jogador com o username '%s\n", username);
        fflush(stdout);
    }
    return 0;
}

