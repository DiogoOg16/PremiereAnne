#pragma comment(lib, "ws2_32.lib")
#pragma comment(lib, "Winmm.lib")

#include "delayer.h"
#include "server.h"
#include "math.h"

#include<stdio.h>
#include <errno.h>

#define CONTROLLER_PORT 6556
#define T_FINAL 5

uint8_t buffer[DELAYER_BUFFER_LEN] = { 0 };
DelayerSocket s;
int stop = 0;

float last_value_sent = 0.0;
DWORD start, end;
DWORD mesured_delay = 0;
float q0m = 1.0;
float ref_pos = M_PI/2;
float last_pos = 0.0;
float error = 0.0;

int main() {
	
	float K = 0.0;
	struct ControllerPacket * p = (struct ControllerPacket *) buffer;
	s = CreateDelayerSocket(CONTROLLER_PORT);

	if(ConnectToDelayer(s, CONTROLLER))
		printf("Connected.\n");
	else {
		printf("Failed to connect.\n");
		goto end;	
	}
	
	if(SendTo(s, buffer, DELAYER_BUFFER_LEN)) {
		p->q[0] = q0m;
		printf("Message sent: q[0] = %f\n", p->q[0]);	
		start = timeGetTime();
	}

	while(1) {
		if(RecvFrom(s, buffer, DELAYER_BUFFER_LEN)) {

			printf("Message received: q[0] = %f\n", p->q[0]);	
			last_pos = p->q[0];

			end = timeGetTime();
			mesured_delay = end - start;
			
			printf("mesured_delay = end - start, %lu = %lu - %lu\n", mesured_delay, end, start);

			error = ref_pos - last_pos;

			printf("(error = ref_pos - last_pos), %f = %f - %f\n", error, ref_pos, last_pos);

			K = (mesured_delay < 10) ? 12.5 : (332.31)*pow(mesured_delay, -1.1517);
			
			printf("Gain K: %f\n", K);
			q0m = K * error;	
			
			memset(p, 0, sizeof(struct ControllerPacket));

			p->q[0] = q0m;

			if(SendTo(s, buffer, DELAYER_BUFFER_LEN)) {
				printf("Message sent: q[0] = %f\n", p->q[0]);	
				start = timeGetTime();
			}

		}
		memset(buffer, 0, DELAYER_BUFFER_LEN);
	}

	end: CloseDelayerSocket(s);

	return 0;
}


