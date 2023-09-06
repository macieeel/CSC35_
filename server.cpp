#include <sys/types.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/fcntl.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include "utils.c"

#define SERVER_PORT 8080  /* arbitrary, but client & server must agree*/
#define BUF_SIZE 4096  /* block transfer size */
char buf[BUF_SIZE];  /* buffer for outgoing file */


int receive_message_from_client(int bytes, int sa) {
      // Receive information message
      bytes = read(sa, buf, BUF_SIZE); 
      if (bytes <= 0) {
         // Handle error or end of connection
         close(sa);
         return 0;
      }
      buf[bytes] = '\0'; // Null-terminate the received data
      printf("\nReceived from client: %s\n", buf);
      return 1;
}

int main(int argc, char *argv[])
{ 
   const char *askIP = "Envie sua identificação.";
   const char *return_to_base_message = "Missão cancelada. Retorne para a base.";
   const char *ask_position = "Envie sua posição atualizada.";
   const char *base_location = "Meu IP é: 128.0.1. Minha posição atualizada é: (0,0,0)";
   
   int s, b, l, fd, sa, bytes, on = 1;
   struct sockaddr_in channel;  /* holds IP address */
   /* Build address structure to bind to socket. */
   memset(&channel, 0, sizeof(channel));
   /* zero channel */
   channel.sin_family = AF_INET;
   channel.sin_addr.s_addr = htonl(INADDR_ANY);
   channel.sin_port = htons(SERVER_PORT);
   /* Passive open. Wait for connection. */
   s = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP); /* create socket */
   if (s < 0) {printf("socket call failed"); exit(-1);}
   setsockopt(s, SOL_SOCKET, SO_REUSEADDR, (char *) &on, sizeof(on));
   
   b = bind(s, (struct sockaddr *) &channel, sizeof(channel));
   if (b < 0) {printf("bind failed"); exit(-1);}
   
   l = listen(s, 1); /* specify queue size */
   if (l < 0) {printf("listen failed"); exit(-1);}

   /* Socket is now set up and bound. Wait for connection and process it. */
   while (true) {
      
      sa = accept(s, 0, 0); /* block for connection request */
      if (sa < 0) {printf("accept failed"); exit(-1);}
      printf("Conexão estabelecida\n");

      // Send the first message to client
      write(sa, askIP, strlen(askIP));
      printf("Sent fist message\n");
      int last_sent = 1;
      int flag = 0;
      while(flag == 0){

         if(receive_message_from_client(bytes, sa) == 0) break;
         if(last_sent == 1){
            if(checkMessage(askIP, buf, sa)){
                  // Send the return to base message to client
                  write(sa, return_to_base_message, strlen(return_to_base_message));
                  printf("Sent return to base message\n");
                  last_sent = 2;
            }
            else break;
            continue;
         }

         if(last_sent == 2){
            if(checkMessage(return_to_base_message, buf, sa)){
               // Wait for 10 seconds
               sleep(5);

               // Ask for position
               write(sa, ask_position, strlen(ask_position));
               printf("Sent position request\n");
               last_sent = 3;

               int iteration = 0;
               // Start asking for position every 3 seconds
               while (iteration < 10) {
               
                  // Receive position
                  if(receive_message_from_client(bytes, sa) == 0){
                     flag = 1;
                     break;
                  } 
                  if(checkMessage(ask_position, buf, sa)){
                     // Check if the client reached the base location
                     if (strcmp(buf, base_location) == 0 ) {
                        printf("Client has reached the base location.\n");
                        flag = 1;
                        break; // Exit the loop if the client reached the base location
                     }
                     else{
                        // Wait for 3 seconds before sending another request
                        sleep(3);

                        // Ask for position
                        write(sa, ask_position, strlen(ask_position));
                        printf("Sent position request\n");
                     }
                     iteration++;
                  }
                  else continue;
               }
            }
            continue;
         }

      }

      // ------------------------------------------------------------------
      close(sa); /* close connection */
   }   
}

