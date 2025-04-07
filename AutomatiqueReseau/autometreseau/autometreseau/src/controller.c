#include "delayer.h"

#include <stdio.h>
#include <errno.h>

#define CONTROLLER_PORT 6556

uint8_t buffer[DELAYER_BUFFER_LEN] = { 0 };

int main() {
	
	DelayerSocket s = CreateDelayerSocket(CONTROLLER_PORT);
	
	if(ConnectToDelayer(s, CONTROLLER))
		printf("Connected.\n");
	else {
		printf("Failed to connect.\n");
		goto end;	
	}

	// Configure Instance
	delayer_config_t cfg = {
		.DelayWhenSending = DISABLE,
		.DelayWhenReceiving = DISABLE
	};
	if(SetConfig(s, cfg))
		printf("Instance Configured.\n");
	else {
		printf("Instance not configured.\n");
		goto end;
	}

	memcpy(buffer, "from 2 to 1", strlen("from 2 to 1"));
	
	if(SendTo(s, buffer, DELAYER_BUFFER_LEN))
		printf("Message sent successfully\n");
	else {	
		printf("Message was not sent\n"); 
		printf(strerror(errno));
		goto end;
	}

	memset(buffer, 0, DELAYER_BUFFER_LEN);

	if(RecvFrom(s, buffer, DELAYER_BUFFER_LEN)) {
		printf("Response received.\n");
		printf("Response: %s", buffer);
		printf("\n");
	} else {
		printf("No message received\n");
		printf(strerror(errno));
		goto end;
	};
	
	end: close(s);

	return 0;
}


