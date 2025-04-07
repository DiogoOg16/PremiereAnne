#pragma comment(lib, "ws2_32.lib")
#pragma comment(lib, "Winmm.lib")
#include <winsock2.h>
#include <windows.h>
//#include <timeapi.h>
#include <stdint.h>
#include <stdio.h>

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
	FOWARD 				= 3,
	SEND_DATA 			= 4,
	CONNECTED			= 5,	
	CONFIGURED			= 6,
	FOWARDED 			= 7,
	SENT_DATA 			= 8,
	DELAYER_ERROR 			= 9
};

enum _Identification {
	CONTROLLER 	= 0x00,
	ROBOT 		= 0xFF,
};

struct _DelayerInstance {
	enum _Identification _id;
	struct sockaddr_in _socket;
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
extern WSADATA _int_wsa_data;
extern SOCKET _socket_desc;
extern SOCKADDR_IN _server_addr, _client_addr;
extern int _client_addr_len;
extern long int _delay;

int start_socket();
int conn(struct sockaddr_in t, enum _Identification id);
int close_socket();
int wait_for_message(struct _ProcessArg * p);
int ack(struct _DelayerPacket p, struct sockaddr_in t);
int foward(struct _DelayerPacket source, struct _DelayerPacket dest);
void print_packet(struct _DelayerPacket p);
DWORD WINAPI _delayer_func();

/* External API */

typedef struct _DelayerPacket delayer_packet_t;
typedef int DelayerSocket;
typedef enum _Operation operation_t;
typedef enum _Identification identification_t;
extern WSADATA _ext_wsa_data;
extern identification_t _instance_id;
extern long int _init_time, _final_time;

int CreateDelayerSocket(int Port);
int SetTimeout(DelayerSocket SocketDescriptor, unsigned long int TimeoutMs);
int ConnectToDelayer(DelayerSocket SocketDescriptor, identification_t Identification);
int SendTo(DelayerSocket SocketDescriptor, uint8_t * Buffer, uint32_t BufferLen);
int RecvFrom(DelayerSocket SocketDescriptor, uint8_t * Buffer, uint32_t BufferLen);
int CloseDelayerSocket(DelayerSocket s);
long int GetDelay();
