#include "delayer.h"

#pragma comment(lib, "ws2_32.lib")
#include <winsock2.h>

struct _DelayerInstance _controller_instance = { 0 };
struct _DelayerInstance _robot_instance = { 0 };
WSADATA _wsa_data;
SOCKET _socket_desc;
SOCKADDR_IN _server_addr, _client_addr;
int _client_addr_len = sizeof(_client_addr);
long int _delay;

int start_socket() {
	
	HANDLE delayer_thread;

	if(WSAStartup(MAKEWORD(2, 0), &_wsa_data) != 0)
		return 0;

	if((_socket_desc = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
		return 0;
	
	_server_addr.sin_family = AF_INET;
	_server_addr.sin_port = htons(SERVER_PORT);
	_server_addr.sin_addr.s_addr = inet_addr(SERVER_ADDR);

	if(bind(_socket_desc, (SOCKADDR *) &_server_addr, sizeof(_server_addr)) < 0)
		return 0;
	
	if((delayer_thread = CreateThread(NULL, 0, _delayer_func, NULL, 0, NULL)) == NULL)
		return 0;

	return 1;
}

int conn(struct sockaddr_in client, enum _Identification id) {
	int connected = 0;

	if(id == CONTROLLER) {
		_controller_instance._id = id;	
		_controller_instance._socket = client;
	
		connected = 1;
	} else if(id == ROBOT) {
		_robot_instance._id = id;	
		_robot_instance._socket = client;

		connected = 1;
	}

	return connected;
}

int close_socket() {
	
	closesocket(_socket_desc);
	WSACleanup();

	return 1;
}

int wait_for_message(struct _ProcessArg * p_a) {

	if(recvfrom(_socket_desc, (char *) &(p_a->_packet), sizeof(struct _DelayerPacket), 0, (struct sockaddr *) &(p_a->_client), &_client_addr_len) < 0) {
		return 0;
	}

	return 1;
}

int ack(struct _DelayerPacket p, struct sockaddr_in t) {

	if(sendto(_socket_desc, (char *) &p, sizeof(struct _DelayerPacket), 0, (struct sockaddr *) &t, sizeof(t)) < 0)
		return 0;

	return 1;
}

int foward(struct _DelayerPacket source, struct _DelayerPacket dest) {
	
	if(source._id == CONTROLLER) {
		if((rand() % 100) < PACKET_LOSS) {
			printf("(PACKET LOST) CONTROLLER -> ROBOT");
			return 0;
		} else {
			printf("CONTROLLER -> ROBOT (Delay: %d ms)\n", _delay);

			Sleep(_delay);

			if(sendto(_socket_desc, (char *) &dest, sizeof(struct _DelayerPacket), 0, (struct sockaddr *) &(_robot_instance._socket), sizeof(_robot_instance._socket)) < 0)
				return 0;
		}
			
	}
	else if(source._id == ROBOT) {
		if((rand() % 100) < PACKET_LOSS) {
			printf("(PACKET LOST) ROBOT -> CONTROLLER");
			return 0;
		} else {	
			printf("ROBOT -> CONTROLLER (Delay: %d ms)\n", _delay);
			
			Sleep(_delay);

			if(sendto(_socket_desc, (char *) &dest, sizeof(struct _DelayerPacket), 0, (struct sockaddr *) &(_controller_instance._socket), sizeof(_controller_instance._socket)) < 0)
				return 0;

		}
	}

	return 1;
}

void print_packet(struct _DelayerPacket p) {
	printf("--- Packet ----\n");
	printf("Id: %d\n", p._id);
	printf("Operation: %d\n", p._op);
	if(p._op == FOWARD)
		printf("Data %s\n", p._data);
	else
		printf("Data %x\n", p._data);
		
	printf("\n");
}

#if defined(DELAYER_MODE_RANDOM)
#pragma message("The delayer mode RANDOM has been chosen")
DWORD WINAPI _delayer_func() {
	_delay = 0;
	srand(0);
	while(1) {
		_delay = rand() % 1000;
		Sleep(10);
	}
}

#elif defined(DELAYER_MODE_VARCONST)
#pragma message("The delayer mode VARCONST has been chosen")
DWORD WINAPI _delayer_func() {
	_delay = 20;
	while(1) {
		_delay += 100;		
		Sleep(2500);
	}	
}
#elif defined(DELAYER_MODE_CONST)
#pragma message("The delayer mode CONST has been chosen")
DWORD WINAPI _delayer_func() {
	_delay = CONST_MODE_DELAY;	
}
#elif defined(DELAYER_MODE_NONE)
#pragma message("The delayer mode NONE has been chosen")
DWORD WINAPI _delayer_func() {
	_delay = 0;	
}
#else
#pragma message("A delayer mode must be chosen")
#endif
