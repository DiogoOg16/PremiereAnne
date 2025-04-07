#include "delayer.h"

#include <fcntl.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <time.h>

identification_t _instance_id;
time_t _init_time, _final_time;

DelayerSocket CreateDelayerSocket(int Port) {

	DelayerSocket s;	
	
	struct sockaddr_in target;
	target.sin_family = AF_INET;
	target.sin_port = htons(Port);
	target.sin_addr.s_addr = inet_addr(SERVER_ADDR);

	struct timeval timeout;
	timeout.tv_sec = 5;
	timeout.tv_usec = 0;

	
	// int flags;

	if((s = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) < 0)
		return 0;

	if(bind(s, (struct sockaddr *) &target, sizeof(target)) < 0)
		return 0; 

	/*
 	if((flags = fcntl(s, F_GETFL, 0)) == -1)
		return 0;
	
	if(fcntl(s, F_SETFL, flags | O_NONBLOCK) < 0)
		return 0;
	*/

	_init_time = _final_time = 0;

	return s;
}

int ConnectToDelayer(DelayerSocket SocketDescriptor, identification_t Identification) {

	_instance_id = Identification;

	struct _DelayerPacket p;
	memset(&p, 0, sizeof(struct _DelayerPacket));
	p._id = _instance_id;
	p._op = CONNECT;
	memcpy(&p._data, &Identification, sizeof(identification_t));

	struct sockaddr_in server;
	server.sin_family = AF_INET;
	server.sin_port = htons(SERVER_PORT);
	server.sin_addr.s_addr = inet_addr(SERVER_ADDR);

	socklen_t server_len = sizeof(struct sockaddr_in);

	if(sendto(SocketDescriptor, &p, sizeof(struct _DelayerPacket), 0, (struct sockaddr *) &server, sizeof(server)) < 0)
		return 0; 
	
	memset(&p, 0, sizeof(struct _DelayerPacket));
	memset(&server, 0, sizeof(struct sockaddr_in));	

	// Acknowlodge
	if(recvfrom(SocketDescriptor, &p, sizeof(struct _DelayerPacket), 0, (struct sockaddr *) &server, &server_len) < 0)
		return 0;

	if(p._op == CONNECTED)
		return 1;

}

int SetConfig(DelayerSocket SocketDescriptor, delayer_config_t Configuration) {

	struct _DelayerPacket p;
	memset(&p, 0, sizeof(struct _DelayerPacket));
	p._id = _instance_id;
	p._op = CONFIGURE;
	memcpy(p._data, &Configuration, sizeof(struct _DelayerConfig));

	struct sockaddr_in server;
	server.sin_family = AF_INET;
	server.sin_port = htons(SERVER_PORT);
	server.sin_addr.s_addr = inet_addr(SERVER_ADDR);

	socklen_t server_len = sizeof(struct sockaddr_in);
	
	if(sendto(SocketDescriptor, &p, sizeof(struct _DelayerPacket), 0, (struct sockaddr *) &server, sizeof(server)) < 0)
		return 0; 

	memset(&p, 0, sizeof(struct _DelayerPacket));
	memset(&server, 0, sizeof(struct sockaddr_in));	
	
	if(recvfrom(SocketDescriptor, &p, sizeof(struct _DelayerPacket), 0, (struct sockaddr *) &server, &server_len) < 0)
		return 0;

	if(p._op == CONFIGURED)
		return 1;

	return 0;
}

int SendTo(DelayerSocket SocketDescriptor, uint8_t * Buffer, uint32_t BufferLen) {

	struct _DelayerPacket p;
	memset(&p, 0, sizeof(struct _DelayerPacket));
	p._id = _instance_id; 
	p._op = FOWARD;
	memset(p._data, 0, DELAYER_BUFFER_LEN);
	memcpy(p._data, Buffer, DELAYER_BUFFER_LEN);

	struct sockaddr_in server;
	server.sin_family = AF_INET;
	server.sin_port = htons(SERVER_PORT);
	server.sin_addr.s_addr = inet_addr(SERVER_ADDR);

	socklen_t server_len = sizeof(struct sockaddr_in);
		
	if(sendto(SocketDescriptor, &p, sizeof(struct _DelayerPacket), 0, (struct sockaddr *) &server, sizeof(server)) < 0)
		return 0; 
	
	memset(&p, 0, sizeof(struct _DelayerPacket));
	memset(&server, 0, sizeof(struct sockaddr_in));	
	
	if(recvfrom(SocketDescriptor, &p, sizeof(struct _DelayerPacket), 0, (struct sockaddr *) &server, &server_len) < 0)
		return 0;

	_init_time = time(0);

	if(p._op == FOWARDED)
		return 1;

	return 0;
}

int RecvFrom(DelayerSocket SocketDescriptor, uint8_t * Buffer, uint32_t BufferLen) {
	
	struct _DelayerPacket p;
	memset(&p, 0, sizeof(struct _DelayerPacket));

	struct sockaddr_in source;
	socklen_t source_len = sizeof(source); 

	if(recvfrom(SocketDescriptor, &p, sizeof(struct _DelayerPacket), 0, (struct sockaddr *) &source, &source_len) < 0)
		return 0;

	if(p._op == SEND_DATA) {
		memcpy(Buffer, p._data, DELAYER_BUFFER_LEN); 
	} else {
		return 0;
	}

	_final_time = time(0);

	return 1;
}

time_t GetDelay() {
	return (_final_time - _init_time); 
}
