#include "delayer.h"

#pragma comment(lib, "ws2_32.lib")
#include <winsock2.h>
#include <fcntl.h>

WSADATA _ext_wsa_data;
identification_t _instance_id;
long int _init_time, _final_time;

DelayerSocket CreateDelayerSocket(int Port) {

	if(WSAStartup(MAKEWORD(2, 0), &_ext_wsa_data) != 0)
		return 0;

	DelayerSocket s;

	SOCKADDR_IN target;
	target.sin_family = AF_INET;
	target.sin_port = htons(Port);
	target.sin_addr.s_addr = inet_addr(SERVER_ADDR);

	if((s = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) < 0)
		return 0;

	if(bind(s, (SOCKADDR *) &target, sizeof(target)) < 0)
		return 0; 

	_init_time = _final_time = timeGetTime();

	return s;
}

int ConnectToDelayer(DelayerSocket SocketDescriptor, identification_t Identification) {

	_instance_id = Identification;

	struct _DelayerPacket p;
	memset(&p, 0, sizeof(struct _DelayerPacket));
	p._id = _instance_id;
	p._op = CONNECT;
	memcpy(&p._data, &Identification, sizeof(identification_t));

	SOCKADDR_IN server;
	server.sin_family = AF_INET;
	server.sin_port = htons(SERVER_PORT);
	server.sin_addr.s_addr = inet_addr(SERVER_ADDR);

	int server_len = sizeof(struct sockaddr_in);

	if(sendto(SocketDescriptor, (char *) &p, sizeof(struct _DelayerPacket), 0, (struct sockaddr *) &server, sizeof(server)) < 0)
		return 0; 
	
	memset(&p, 0, sizeof(struct _DelayerPacket));
	memset(&server, 0, sizeof(struct sockaddr_in));	

	if(recvfrom(SocketDescriptor, (char *) &p, sizeof(struct _DelayerPacket), 0, (struct sockaddr *) &server, &server_len) < 0)
		return 0;

	if(p._op == CONNECTED) {
		/*
		u_long non_blocking = 1;
		ioctlsocket(SocketDescriptor, FIONBIO, &non_blocking);
		*/
		return 1;
	}

	return 1;
}

int SetTimeout(DelayerSocket SocketDescriptor, unsigned long int TimeoutMs) {
	DWORD timeout = (DWORD) TimeoutMs;
	if(setsockopt(SocketDescriptor, SOL_SOCKET, SO_RCVTIMEO, (char *) &timeout, sizeof(timeout)) < 0) {
		return 0;
	}	
}

int SendTo(DelayerSocket SocketDescriptor, uint8_t * Buffer, uint32_t BufferLen) {

	struct _DelayerPacket p;
	memset(&p, 0, sizeof(struct _DelayerPacket));
	p._id = _instance_id; 
	p._op = FOWARD;
	memset(p._data, 0, DELAYER_BUFFER_LEN);
	memcpy(p._data, Buffer, DELAYER_BUFFER_LEN);

	SOCKADDR_IN server;
	server.sin_family = AF_INET;
	server.sin_port = htons(SERVER_PORT);
	server.sin_addr.s_addr = inet_addr(SERVER_ADDR);

	int server_len = sizeof(struct sockaddr_in);
		
	if(sendto(SocketDescriptor, (char *) &p, sizeof(struct _DelayerPacket), 0, (struct sockaddr *) &server, sizeof(server)) < 0)
		return 0; 
		
	_init_time = timeGetTime();

	return 1;
}

int RecvFrom(DelayerSocket SocketDescriptor, uint8_t * Buffer, uint32_t BufferLen) {
	
	struct _DelayerPacket p;
	memset(&p, 0, sizeof(struct _DelayerPacket));

	SOCKADDR_IN source;
	int source_len = sizeof(source); 

	if(recvfrom(SocketDescriptor, (char *) &p, sizeof(struct _DelayerPacket), 0, (struct sockaddr *) &source, &source_len) < 0)
		return 0;

	if(p._op == SEND_DATA) {
		memcpy(Buffer, p._data, DELAYER_BUFFER_LEN); 
	} else {
		return 0;
	}

	_final_time = timeGetTime();

	return 1;
}

long int GetDelay() {
	return abs(_final_time - _init_time); 
}

int CloseDelayerSocket(DelayerSocket s) {
	closesocket(s);
	WSACleanup();
}
