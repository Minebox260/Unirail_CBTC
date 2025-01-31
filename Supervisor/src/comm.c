#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include "comm.h"

#define CHECK_ERROR(val1,val2,msg)   if (val1==val2) \
                                    { perror(msg); \
                                        exit(EXIT_FAILURE); }

void send_data(const int sd, const struct sockaddr_in * send_adr, const message_t message) {
    char buff_send[MAXOCTETS + 1];
    char data_str[MAXOCTETS - 50] = ""; 
	int nbcar;
	socklen_t send_adr_len = sizeof(*send_adr);

    for (int i = 0; message.data[i] != NULL; i++) {
        if (strlen(data_str) + strlen(message.data[i]) + 1 < sizeof(data_str)) {
            if (i > 0) strcat(data_str, ":");
            strcat(data_str, message.data[i]);
        } else break;
    }

    sprintf(buff_send, "%d:%d:%s", message.id, message.code, data_str);
    
    nbcar=sendto(sd, buff_send, strlen(buff_send) + 1, 0, (const struct sockaddr *)send_adr, send_adr_len);
	CHECK_ERROR(nbcar,0,"\nErreur lors de l'émission des données");
}

void receive_data(const int sd, struct sockaddr_in * recv_adr, message_t *message) {
	int nbcar;
	char * ptr;
	socklen_t recv_adr_len = sizeof(*recv_adr);
	char buff_recv[MAXOCTETS + 1];
	
	nbcar = recvfrom(sd, buff_recv, MAXOCTETS + 1, 0, (struct sockaddr *)recv_adr, &recv_adr_len);
	CHECK_ERROR(nbcar,0,"\nErreur lors de la réception des données");
	buff_recv[nbcar] = '\0';

	ptr = strtok(buff_recv, ":");
	if (ptr == NULL) {
		message->id = -1;
		message->code = 500;
		printf("Erreur: message malformé\n");
		return;
	}
	message->id = atoi(ptr);

	ptr = strtok(NULL, ":");
	if (ptr == NULL) {
		message->code = 500;
		printf("Erreur: message malformé\n");
		return;
	}
	message->code = atoi(ptr);

	ptr = strtok(NULL, ":");
	for (int i = 0; ptr != NULL && i < MAXDATA; i++) {
		message->data[i] = ptr;
		ptr = strtok(NULL, ":");
	}
}