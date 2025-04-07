#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "server.h"

#define HANDLES 6
#define COPPELIA_SIM_PORT 5555
#define TIMEOUT_MS 5000
#define T_FINAL 5
#define COMM_THREAD_CYCLE_MS 5
#define ROBOT_PORT 6557

extern "C" {
    #include "extApi.h"
    #include "delayer.h"
}


DelayerSocket s;
int clientID;
int handles[HANDLES] = { 0 };
uint8_t buffer[DELAYER_BUFFER_LEN] = { 0 };
DWORD start, end;
DWORD mesured_delay = 0;
int GetHandles(){
	
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

int SetJointPos(float * q)
{
	int n = 0;

	for (int i = 0; i < HANDLES; i++) {
    		if(simxSetJointTargetPosition(clientID, handles[i], q[i], simx_opmode_oneshot) != -1)
			n++;
	}
	
	return n;
}

int SetJointVel(float * q) {

	int n = 0;

	for (int i = 0; i < HANDLES; i++) {
		if(simxSetJointTargetVelocity(clientID, handles[i], q[i], simx_opmode_oneshot) != -1)
			n++;
	}

	return n;
}

int GetJointPos(float * q)
{
	int n = 0;

	for (int i = 0; i < HANDLES; i++) {
		if(simxGetJointPosition(clientID, handles[i], &q[i], simx_opmode_oneshot) != -1)
			n++;
	}

	return n;
}

int main(int argc,char* argv[])
{

	HANDLE threads[2];
	struct ControllerPacket * packet = (struct ControllerPacket *) buffer;

	s = CreateDelayerSocket(ROBOT_PORT);

	if(ConnectToDelayer(s, ROBOT)) {
		printf("Connected.\n");
	} else {
		printf("Failed to connect.\n");
		CloseDelayerSocket(s);
		return 1;
	}

	SetTimeout(s, 5000);
	
	if((clientID = simxStart((simxChar*) "127.0.0.1", COPPELIA_SIM_PORT, true, true, TIMEOUT_MS, COMM_THREAD_CYCLE_MS)) == -1) {
		printf("Connection to Coppelia server not possible.\n");
		CloseDelayerSocket(s);
		return 1;
	} else printf("Connection to Coppelia server succesfull.\n");

	GetHandles();
	simxSynchronous(clientID, true);
	simxStartSimulation(clientID, simx_opmode_oneshot);

	start = end = 0;
	
	while(1) {
		memset(packet, 0, DELAYER_BUFFER_LEN);
		if(RecvFrom(s, buffer, DELAYER_BUFFER_LEN)) {
			end = timeGetTime();

			mesured_delay = end - start;
			SetTimeout(s, 2*mesured_delay);

			start = end;

			printf("Packet received: q[0] = %f\n", packet->q[0]);

			SetJointVel(packet->q);
			
			memset(packet, 0, sizeof(struct ControllerPacket));

			GetJointPos(packet->q);	

			memcpy(buffer, packet, sizeof(struct ControllerPacket));

			SendTo(s, buffer, DELAYER_BUFFER_LEN);  
		} else {
			// Recovery Mode (after a packet loss)
			SetTimeout(s, 2000);
			int exit = 0;
			packet->q[0] = 0.0;
			SetJointVel(packet->q);
			for(int i = 0; i < 5; i++) {
				printf("Recovery Mode (%d)...\n", i);
				if(RecvFrom(s, buffer, DELAYER_BUFFER_LEN)) {
					start = timeGetTime();	
					printf("Packet received: q[0] = %f\n", packet->q[0]);
					SetJointVel(packet->q);
					
					memset(packet, 0, sizeof(struct ControllerPacket));

					GetJointPos(packet->q);	

					memcpy(buffer, packet, sizeof(struct ControllerPacket));

					SendTo(s, buffer, DELAYER_BUFFER_LEN);  
					break;
				} else {
					if(i == 4) { 
						exit = 1;
						break;
					} else {
						memset(packet, 0, sizeof(struct ControllerPacket));
						GetJointPos(packet->q);
						memcpy(buffer, packet, sizeof(struct ControllerPacket));
						SendTo(s, buffer, DELAYER_BUFFER_LEN);
					}
				}
			}
			if (exit) 
				break;
		}
	}
	
	simxStopSimulation(clientID, simx_opmode_oneshot);
	simxFinish(clientID);
	CloseDelayerSocket(s);

    return 0;
}
