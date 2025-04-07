#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <unistd.h>
#include "server.h"

#define HANDLES 6
#define COPPELIA_SIM_PORT 5555
#define TIMEOUT_MS 5000
#define T_FINAL 5
#define COMM_THREAD_CYCLE_MS 5
#define ROBOT_PORT 6557

using namespace std;

extern "C" {
    #include "extApi.h"
	#include "delayer.h"
}

int handles[HANDLES] = { 0 };

int GetHandles(int clientID){
	
	simxChar objectName[100];
	simxInt handle;

	int n = 0;

	for (int i = 0; i < HANDLES; i++) {
		memset(objectName, 0, 100);	
		
		sprintf(objectName, "joint%d", i+1);
		
		if (simxGetObjectHandle(clientID, objectName, &handle, simx_opmode_oneshot_wait) == simx_return_ok) {
			handles[i] = handle;
		    n++;
		}
	}
	
	return n;
}

int SetJointPos(int clientID,  float *q)
{
	int n = 0;

	for (int i = 0; i < HANDLES; i++)
    	if(simxSetJointTargetPosition(clientID, handles[i], q[i], simx_opmode_oneshot) != -1)
			n++;
	
	return n;
}

int main(int argc,char* argv[])
{

	/* Variables */
	int err, clientID;
	float q[6] = { 0 };
	float qr[6] = { 0 };
	float t = 0.0;
	float dt = 0.01;
	float q0m = 0.5;
	float w = 2*M_PI/2.5;

	DelayerSocket s;
	delayer_config_t cfg;
	uint8_t buffer[DELAYER_BUFFER_LEN] = { 0 };
	struct ControllerPacket * packet;

	/* Connection to Delayer */
	// Delayer Socket
	s = CreateDelayerSocket(ROBOT_PORT);

	// Delayer Connection
	if(ConnectToDelayer(s, ROBOT))
		printf("Connected.\n");
	else
		printf("Failed to connect.\n");

	// Delayer Configuration
	cfg.DelayWhenSending = DISABLE;
	cfg.DelayWhenReceiving = DISABLE;
	
	if(SetConfig(s, cfg))
		printf("Instance Configured.\n");
	else
		printf("Instance not configured.\n");

	/* Connection to Coppelia */
	if((clientID = simxStart((simxChar*) "127.0.0.1", COPPELIA_SIM_PORT, true, true, TIMEOUT_MS, COMM_THREAD_CYCLE_MS)) == -1) {
		printf("Connection to the server not possible\n");
		close(s);
		return 1;
	}

	GetHandles(clientID);
	simxSynchronous(clientID, true);
	simxStartSimulation(clientID, simx_opmode_oneshot);

	while (1 == 1) {	
		
		RecvFrom(s, buffer, DELAYER_BUFFER_LEN);

		packet = (struct ControllerPacket *) buffer;
		
		printf("Packet received: q[0] = %f\n", packet->q[0]);

		SetJointPos(clientID, packet->q);
   
		t+=dt;
   
		usleep(100*1000); // 100 ms
	}

	simxStopSimulation(clientID, simx_opmode_oneshot);

	simxFinish(clientID);

    return 0;
}
