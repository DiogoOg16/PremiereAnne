#include "delayer.h"

#include <stdlib.h>
#include <stdio.h>
//#include <pthread.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>

void sig_term_handler(int sig) {
	close_socket();
	PRINT("Exiting...");
}

int main() {
	
	if(!start_socket()) {
		PRINT("Error on socket condition...");
		return 1;
	}

	if(signal(SIGINT, sig_term_handler) == SIG_ERR) {
		PRINT("Error on sig term handler...");
		return 1;
	}

	while(1) {
		
		struct _ProcessArg * p_a = malloc(sizeof(struct _ProcessArg));

		PRINT("Waiting for message...");

		if(!wait_for_message(p_a))
			PRINT("Failed waiting for message...");

		PRINT("Message received.");
		
		struct _DelayerPacket r_p = p_a->_packet;
		struct sockaddr_in client = p_a->_client;
		struct _DelayerPacket s_p;
	
		print_packet(r_p);
		enum _Identification id;

		switch(r_p._op) {
			case CONNECT:
				memcpy(&id, &r_p._data, sizeof(enum _Identification));
				memset(&s_p, 0, sizeof(struct _DelayerPacket));
				if(conn(client, id)) {
					s_p._op = CONNECTED;
				} else {
					s_p._op = ERROR;
				}
				// Acknowlodge	
				ack(s_p, client);
				break;

			case FOWARD:
				s_p._op = SEND_DATA;
				memcpy(s_p._data, r_p._data, DELAYER_BUFFER_LEN);
			
				if(foward(r_p, s_p)) {
					memset(&s_p, 0, sizeof(struct _DelayerPacket));
					s_p._op = FOWARDED;
				} else {
					memset(&s_p, 0, sizeof(struct _DelayerPacket));
					s_p._op = ERROR;
				}	
			
				break;

			default:
				
				break;
		}
	
		free(p_a);
	}


	return 0;
}

