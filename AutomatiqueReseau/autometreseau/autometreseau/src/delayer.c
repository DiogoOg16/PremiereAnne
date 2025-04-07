#include "delayer.h"

#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
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

	if(signal(SIGINT, sig_term_handler) == SIG_ERR)
		return 1;

	while(1) {
		
		struct _ProcessArg * p_a = malloc(sizeof(struct _ProcessArg));

		PRINT("Waiting for message...");

		if(!wait_for_message(p_a))
			break;		
		
		PRINT("Message received.");
		
		struct _DelayerPacket r_p = p_a->_packet;
		struct sockaddr_in client = p_a->_client;
		struct _DelayerPacket s_p;
	
		print_packet(r_p);

		switch(r_p._op) {
			case NOP:
				s_p._op = NOP;
			
				break;

			case CONNECT:
				enum _Identification id;
				memcpy(&id, &r_p._data, sizeof(enum _Identification));
				if(conn(client, id)) {
					s_p._op = CONNECTED;
				} else {
					s_p._op = ERROR	;
				}
				break;

			case CONFIGURE:
				if(set_config(r_p)) { 
					s_p._op = CONFIGURED;
				} else { 
					s_p._op = ERROR;
				}

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

		// Acknowlodge	
		ack(s_p, client);
	
		free(p_a);
	}


	return 0;
}

