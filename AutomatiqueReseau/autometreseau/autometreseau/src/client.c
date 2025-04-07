#include "delayer.h"
#include "server.h"
#include "math.h"

#include <stdio.h>
#include <errno.h>

#define CONTROLLER_PORT 6556
#define T_FINAL 5

uint8_t buffer[DELAYER_BUFFER_LEN] = { 0 };

int main() {
	
	DelayerSocket s = CreateDelayerSocket(CONTROLLER_PORT);
	
	float w = 2*M_PI/2.5;
	float q0m = 10;
	float q1m = 0;
	float q2m = 1.5;
	float q3m = 0;
	float q4m = 0;
	float q5m = 0;
	float t = 0;
	float dt = 0.01;	

	struct ControllerPacket p;

	if(ConnectToDelayer(s, CONTROLLER))
		printf("Connected.\n");
	else {
		printf("Failed to connect.\n");
		goto end;	
	}

	// Configure Instance
	delayer_config_t cfg = {
		.DelayWhenSending 	= RANDOM,
		.DelayWhenReceiving = DISABLE
	};
	if(SetConfig(s, cfg))
		printf("Instance Configured.\n");
	else {
		printf("Instance not configured.\n");
		goto end;
	}

	memset(&p, 0, sizeof(struct ControllerPacket));

	while (t < T_FINAL) {
		printf("Current time: %6.4f\n", t);	

		p.q[0] = q0m*sin(w*t);
		p.q[1] = -q1m*sin(w*t);
		p.q[2] = q2m;
		p.q[3] = -q3m*sin(w*t);
		p.q[4] = q4m*sin(w*t);
		p.q[5] = -q5m*sin(w*t);
		
		memcpy(buffer, (uint8_t * ) &p, sizeof(struct ControllerPacket));
	
		if(SendTo(s, buffer, DELAYER_BUFFER_LEN))
			printf("Message sent.\n");
		
		t += dt;

		usleep(dt * 1000 * 1000);
	}

	end: close(s);

	return 0;
}


