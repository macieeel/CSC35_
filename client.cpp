#include <sys/types.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>

#define SERVER_PORT 8080 /* arbitrary, but client & server must agree */
#define BUFSIZE 4096  /* block transfer size */

int main(int argc, char **argv)
{
  const char *information = "Meu IP é: 128.0.1. Minha posição é: (x,y,z)";
  const char *confirmation_message = "Missão atualizada. Retornando para a base.";
  const char *error_message = "Comando não identificado. Mande um novo comando.";
  int c, s, bytes;
  char buf[BUFSIZE];  /* buffer for incoming file */
  struct hostent *h;  /* info about server */
  struct sockaddr_in channel; /* holds IP address */

  h = gethostbyname(argv[1]); /* look up host’s IP address */
  if (!h) {printf("gethostbyname failed to locate %s", argv[1]); exit(-1);}
  
  s = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
  if (s <0) {printf("socket call failed"); exit(-1);}
  memset(&channel, 0, sizeof(channel));
  channel.sin_family= AF_INET;
  memcpy(&channel.sin_addr.s_addr, h->h_addr, h->h_length);
  channel.sin_port= htons(SERVER_PORT);
  c = connect(s, (struct sockaddr *) &channel, sizeof(channel));
  if (c < 0) {printf("connect failed"); exit(-1);} 
  /* Connection is now established. */
  
  while (1) {
  // -----------------------------------------------------------------
    // Receive message1
    bytes = read(s, buf, BUFSIZE); /* read from socket */
    if (bytes <= 0) exit(0); /* check for end of file */
    buf[bytes] = '\0'; // Null-terminate the received data
    printf("\nReceived from server: %s\n", buf);
    

    if (strcmp(buf, "Envie sua identificação.") == 0) {
      // Send information to Server
      write(s, information, strlen(information));
      printf("Sent information\n");
      continue;
    }

    if (strcmp(buf, "Missão cancelada. Retorne para a base.") == 0) {
      // Send confirmation to Server
      write(s, confirmation_message, strlen(confirmation_message));
      printf("Sent confirmation\n");

      continue;
    }

    // Check for position request from server
    if (strcmp(buf, "Envie sua posição atualizada.") == 0) {
      char userInput[20];
      printf("Escreva sua posição: ");
      fgets(userInput, sizeof(userInput), stdin);
      userInput[strlen(userInput) - 1] = '\0';

      char location_message[256]; 
      sprintf(location_message, "Meu IP é: 128.0.1. Minha posição atualizada é: (%s)", userInput);
      write(s, location_message, strlen(location_message));
      printf("Sent location update\n");
      continue;
    }

    printf("Comando não identificado. Pedir reenvio.\n");
    write(s, error_message, strlen(error_message));
    printf("Pedido de reenvio enviado.\n");
// -----------------------------------------------------------------
  }
}

