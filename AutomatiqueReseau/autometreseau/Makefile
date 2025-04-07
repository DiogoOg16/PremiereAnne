all: delayer server client

ROOT = .
COPPELIA = ../../CoppeliaRobotics/CoppeliaSimEdu

APP = $(ROOT)/app
INC = $(ROOT)/inc
COPPELIA_INC1 = $(COPPELIA)/programming/include
COPPELIA_INC2 = $(COPPELIA)/programming/legacyRemoteApi/remoteApi
SRC = $(ROOT)/src
OBJ = $(ROOT)/obj
USR_LIB = /usr/lib

delayer: $(APP)/delayer.exe
server: $(APP)/server.exe
client: $(APP)/client.exe

DELAYER_MODE = CONST
CONST_MODE_DELAY = 300
PACKET_LOSS = 0 	# from 0% to 100%

DELAYER_DEFINES = -DDELAYER_MODE_$(DELAYER_MODE) -DCONST_MODE_DELAY=${CONST_MODE_DELAY} -DPACKET_LOSS=${PACKET_LOSS}
COPPELIA_DEFINES = -DNON_MATLAB_PARSING -DMAX_EXT_API_CONNECTIONS=255 -DDO_NOT_USE_SHARED_MEMORY

# Apps

$(APP)/delayer.exe:  $(OBJ)/delayer_int_api.o $(SRC)/delayer.c
	gcc $(SRC)/delayer.c $(OBJ)/delayer_int_api.o -g -I$(INC) -lws2_32 -o $(APP)/delayer

$(APP)/server.exe:  $(OBJ)/delayer_ext_api.o $(OBJ)/extApi.o $(OBJ)/extApiPlatform.o $(OBJ)/server.o
	g++ $(OBJ)/delayer_ext_api.o $(OBJ)/extApi.o $(OBJ)/extApiPlatform.o $(OBJ)/server.o -g -lws2_32 -lwinmm -o $(APP)/server

$(APP)/client.exe: $(OBJ)/delayer_ext_api.o $(SRC)/client.c
	gcc $(SRC)/client.c $(OBJ)/delayer_ext_api.o -g -I$(INC) ${CLIENT_DEFINES} -lws2_32 -lwinmm -o $(APP)/client

# Objects

$(OBJ)/delayer_int_api.o: $(SRC)/delayer_int_api.c
	gcc -c $(SRC)/delayer_int_api.c -I$(INC) $(DELAYER_DEFINES) -lws2_32 -o $(OBJ)/delayer_int_api.o

$(OBJ)/delayer_ext_api.o: $(SRC)/delayer_ext_api.c
	gcc -c $(SRC)/delayer_ext_api.c -I$(INC) -lws2_32 -o $(OBJ)/delayer_ext_api.o

$(OBJ)/extApi.o: $(COPPELIA_INC2)/extApi.c
	gcc -c -I$(COPPELIA_INC1) -I$(COPPELIA_INC2) $(COPPELIA_DEFINES) $(COPPELIA_INC2)/extApi.c -o $(OBJ)/extApi.o

$(OBJ)/extApiPlatform.o: $(COPPELIA_INC2)/extApiPlatform.c
	gcc -c -I$(COPPELIA_INC1) -I$(COPPELIA_INC2) $(COPPELIA_DEFINES) $(COPPELIA_INC2)/extApiPlatform.c -o $(OBJ)/extApiPlatform.o

$(OBJ)/server.o: $(SRC)/server.cpp
	gcc -c -I$(INC) -I$(COPPELIA_INC1) -I$(COPPELIA_INC2) $(COPPELIA_DEFINES) $(SERVER_DEFINES) $(SRC)/server.cpp -o $(OBJ)/server.o  

clean:
	rm -f $(OBJ)/* $(APP)/delayer $(APP)/client $(APP)/server
