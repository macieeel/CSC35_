#include <sys/types.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#define BUFSIZE 4096

char requests_responses[2][3][256]; // Assuming a maximum string length of 256 characters

// Initialize the array with sample data
void initializeArray(){
    strcpy(requests_responses[0][0], "Envie sua identificação.");
    strcpy(requests_responses[0][1], "Missão cancelada. Retorne para a base.");
    strcpy(requests_responses[0][2], "Envie sua posição atualizada.");

    strcpy(requests_responses[1][0], "Meu IP é: 128.0.1. Minha posição é: (x,y,z)");
    strcpy(requests_responses[1][1], "Missão atualizada. Retornando para a base.");
    strcpy(requests_responses[1][2], "Meu IP é: 128.0.1. Minha posição atualizada é: ");
}


int checkMessage(const char * request, char * response, int fd){
    initializeArray();
    for (int i = 0; i <= 2; i++){
        if (strcmp(requests_responses[0][i], request) == 0){
            switch (i){
                case 0:
                    if (strcmp(requests_responses[1][i], response) == 0)
                        return 1;
                    else{   
                        printf("Cliente não identificado. Encerrando conexão.\n");
                        close(fd);
                        return 0;
                    }
                    break;
                
                case 1:
                    if (strcmp(requests_responses[1][i], response) == 0)
                        return 1;
                    else{   
                        printf("\nFormato de resposta errado. Reenviando mensagem.\n");
                        sleep(5);
                        write(fd, request, strlen(request));
                        printf("Mensagem Reenviada.\n");
                        return 0;
                    }
                    break;

                case 2:
                    if (strncmp(requests_responses[1][i], response, 45) == 0)
                        return 1;
                    else{   
                        printf("Formato de resposta errado. Reenviando mensagem.\n");
                        sleep(5);
                        write(fd, request, strlen(request));
                        printf("Mensagem Reenviada.\n");
                        return 0;
                    }
                default:
                    break;
                }
        }
    }
    return 0;
}
