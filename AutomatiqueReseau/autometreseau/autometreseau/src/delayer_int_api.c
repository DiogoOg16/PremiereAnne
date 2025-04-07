#include "delayer.h"

struct _DelayerInstance _controller_instance = { 0 };
struct _DelayerInstance _robot_instance = { 0 };
int _socket_desc;
struct sockaddr_in _server_addr, _client_addr;
socklen_t _client_addr_len = sizeof(_client_addr);

int start_socket() {

	_socket_desc = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	if(_socket_desc < 0)
		return 0; 
	
	_server_addr.sin_family = AF_INET;
	_server_addr.sin_port = htons(SERVER_PORT);
	_server_addr.sin_addr.s_addr = inet_addr(SERVER_ADDR);

	if(bind(_socket_desc, (struct sockaddr *) &_server_addr, sizeof(_server_addr)) < 0)
		return 0;
	
	return 1;
}

int conn(struct sockaddr_in client, enum _Identification id) {
	int registered = 0;

	if(id == CONTROLLER) {
		if(_controller_instance._id == 0) {
			_controller_instance._id = id;	
			_controller_instance._socket = client;
			registered = 1;
		}
		
	} else if(id == ROBOT) {
		if(_robot_instance._id == 0) {
			_robot_instance._id = id;	
			_robot_instance._socket = client;
			registered = 1;
		}
	}

	return registered;
}

int set_config(struct _DelayerPacket p) {

	int set = 0;

	struct _DelayerConfig c;
	memcpy(&c, (struct _DelayerConfig *) &(p._data), sizeof(struct _DelayerConfig));

	if(p._id == _robot_instance._id) {
		_robot_instance._config.DelayWhenSending = c.DelayWhenSending;
		_robot_instance._config.DelayWhenReceiving = c.DelayWhenReceiving;
	
		set = 1;
	}
	else if(p._id == _controller_instance._id) {
		_controller_instance._config.DelayWhenSending = c.DelayWhenSending;
		_controller_instance._config.DelayWhenReceiving = c.DelayWhenReceiving;
		set = 1;
	}
		
	return set;
}

int close_socket() {
	
	close(_socket_desc);

	return 1;
}

int wait_for_message(struct _ProcessArg * p_a) {

	if(recvfrom(_socket_desc, &(p_a->_packet), sizeof(struct _DelayerPacket), 0, (struct sockaddr *) &(p_a->_client), &_client_addr_len) < 0) {
		return 0;
	}

	return 1;
}

int ack(struct _DelayerPacket p, struct sockaddr_in t) {

	if(sendto(_socket_desc, &p, sizeof(struct _DelayerPacket), 0, (struct sockaddr *) &t, sizeof(t)) < 0)
		return 0;

	return 1;
}

int foward(struct _DelayerPacket source, struct _DelayerPacket dest) {

	if(source._id == CONTROLLER) {

		if(_controller_instance._config.DelayWhenSending == RANDOM) {
			int random_delay = (rand() % (500)) * 1000;
			printf("delay of %d microseconds added to transmission\n", random_delay);
			usleep(random_delay);
		} else {

			printf("fowarding from controller to robot with %d microseconds of delay\n", _controller_instance._config.DelayWhenSending + _robot_instance._config.DelayWhenReceiving);
			
			usleep(_controller_instance._config.DelayWhenSending + _robot_instance._config.DelayWhenReceiving);
		}

		if(sendto(_socket_desc, &dest, sizeof(struct _DelayerPacket), 0, (struct sockaddr *) &(_robot_instance._socket), sizeof(_robot_instance._socket)) < 0)
			return 0;
			
	}
	else if(source._id == ROBOT) {

		if(_robot_instance._config.DelayWhenReceiving == RANDOM) {
			int random_delay = (rand() % (30)) * 1000;
			printf("delay of %d microseconds added to transmission\n", random_delay);
			usleep(random_delay);
		} else {
			printf("fowarding from controller to robot with %d microseconds of delay\n", _controller_instance._config.DelayWhenSending + _robot_instance._config.DelayWhenReceiving);
			
			usleep(_robot_instance._config.DelayWhenSending + _controller_instance._config.DelayWhenReceiving);		
		}		

	if(sendto(_socket_desc, &dest, sizeof(struct _DelayerPacket), 0, (struct sockaddr *) &(_controller_instance._socket), sizeof(_controller_instance._socket)) < 0)
			return 0;

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
