#include <stdint.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <limits.h>
#include <stdlib.h>

#define SERVER_ADDR "127.0.0.1"
#define SERVER_PORT 6555
#define DISABLE 0
#define ENABLE 1
#define RANDOM UINT_MAX
#define LOG 1
#define DELAYER_BUFFER_LEN 256

#define PRINT(x) 						\
	do { 								\
		if (LOG == 1) { 				\
			printf("[LOG]: %s\n", x); 	\
			fflush(stdout); 			\
		} 								\
	} while(0)

/* Internal API */

enum _Operation {
	NOP 				= 0,
	
	CONNECT				= 1,
	CONFIGURE			= 2,
	FOWARD 				= 3,
	SEND_DATA 			= 4,
	
	CONNECTED			= 5,	
	CONFIGURED			= 6,
	FOWARDED 			= 7,
	SENT_DATA 			= 8,
	
	ERROR 				= 9
};

enum _Identification {
	CONTROLLER 	= 0x00,
	ROBOT 		= 0xFF,
};

struct _DelayerConfig {
	useconds_t DelayWhenSending;
	useconds_t DelayWhenReceiving;
};

struct _DelayerInstance {
	enum _Identification _id;
	struct sockaddr_in _socket;
	struct _DelayerConfig _config;	
};

struct _DelayerPacket {
	enum _Identification _id;
	enum _Operation _op;	
	uint8_t _data[DELAYER_BUFFER_LEN];
} __attribute__((packed));

struct _ProcessArg {
	struct _DelayerPacket _packet;
	struct sockaddr_in _client;
};

extern struct _DelayerInstance _controller_instance, _robot_instance;
extern int _socket_desc;
extern struct sockaddr_in _server_addr, _client_addr;
extern socklen_t _client_addr_len;

int start_socket();
int conn(struct sockaddr_in t, enum _Identification id);
int set_config(struct _DelayerPacket p);
int close_socket();
int wait_for_message(struct _ProcessArg * p);
int ack(struct _DelayerPacket p, struct sockaddr_in t);
int foward(struct _DelayerPacket source, struct _DelayerPacket dest);
void print_packet(struct _DelayerPacket p);

/* External API */

typedef struct _DelayerPacket delayer_packet_t;
typedef struct _DelayerConfig delayer_config_t;
typedef int DelayerSocket;
typedef enum _Operation operation_t;
typedef enum _Identification identification_t;
extern identification_t _instance_id;
extern time_t _init_time, _final_time;

int CreateDelayerSocket(int Port);
int ConnectToDelayer(DelayerSocket SocketDescriptor, identification_t Identification);
int SetConfig(DelayerSocket SocketDescriptor, delayer_config_t Configuration);
int SendTo(DelayerSocket SocketDescriptor, uint8_t * Buffer, uint32_t BufferLen);
int RecvFrom(DelayerSocket SocketDescriptor, uint8_t * Buffer, uint32_t BufferLen);
time_t GetDelay();
