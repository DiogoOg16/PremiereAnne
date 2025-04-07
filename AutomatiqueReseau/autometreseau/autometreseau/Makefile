all: lib delayer server client

ROOT = .
COPPELIA = /home/auto/Downloads/CoppeliaSim_Edu_V4_9_0_rev2_Ubuntu22_04

APP = $(ROOT)/app
LIB = $(ROOT)/lib
INC = $(ROOT)/inc
COPPELIA_INC1 = $(COPPELIA)/programming/include
COPPELIA_INC2 = $(COPPELIA)/programming/legacyRemoteApi/remoteApi
SRC = $(ROOT)/src
OBJ = $(ROOT)/obj
USR_LIB = /usr/lib

delayer: $(APP)/delayer
controller: $(APP)/controller
robot: $(APP)/robot
server: $(APP)/server
client: $(APP)/client

DEFINES = -DNON_MATLAB_PARSING -DMAX_EXT_API_CONNECTIONS=255 -DDO_NOT_USE_SHARED_MEMORY

# Apps

$(APP)/delayer: $(LIB)/libdelayer.so
	gcc $(SRC)/delayer.c -I$(INC) -ldelayer -o $(APP)/delayer

$(APP)/controller: $(LIB)/libdelayer.so 
	gcc $(SRC)/controller.c -I$(INC) -ldelayer -o $(APP)/controller

$(APP)/robot: $(LIB)/libdelayer.so
	gcc $(SRC)/robot.c -I$(INC) -ldelayer -o $(APP)/robot

$(APP)/server: $(OBJ)/extApi.o $(OBJ)/extApiPlatform.o $(OBJ)/server.o $(LIB)/libdelayer.so
	g++ $? -L$(LIB) -ldelayer -lrt -o $(APP)/server

$(APP)/client: $(LIB)/libdelayer.so
	gcc $(SRC)/client.c -I$(INC) -L$(LIB) -ldelayer -lm -o $(APP)/client

# Libraries

$(LIB)/libdelayer.so: $(OBJ)/delayer_int_api.o $(OBJ)/delayer_ext_api.o 
	gcc -shared $(OBJ)/delayer_int_api.o $(OBJ)/delayer_ext_api.o -o $(LIB)/libdelayer.so
	sudo cp $(LIB)/libdelayer.so $(USR_LIB)

# Objects

$(OBJ)/delayer_int_api.o: $(SRC)/delayer_int_api.c
	gcc -fPIC -c $(SRC)/delayer_int_api.c -I$(INC) -o $(OBJ)/delayer_int_api.o

$(OBJ)/delayer_ext_api.o: $(SRC)/delayer_int_api.c
	gcc -fPIC -c $(SRC)/delayer_ext_api.c -I$(INC) -o $(OBJ)/delayer_ext_api.o

$(OBJ)/extApi.o: $(COPPELIA_INC2)/extApi.c
	gcc -c -I$(COPPELIA_INC1) -I$(COPPELIA_INC2) $(DEFINES) $(COPPELIA_INC2)/extApi.c -o $(OBJ)/extApi.o

$(OBJ)/extApiPlatform.o: $(COPPELIA_INC2)/extApiPlatform.c
	gcc -c -I$(COPPELIA_INC1) -I$(COPPELIA_INC2) $(DEFINES) $(COPPELIA_INC2)/extApiPlatform.c -o $(OBJ)/extApiPlatform.o

$(OBJ)/server.o: $(SRC)/server.cpp
	gcc -c -I$(INC) -I$(COPPELIA_INC1) -I$(COPPELIA_INC2) $(DEFINES) $(SRC)/server.cpp -o $(OBJ)/server.o  

clean:
	rm -f $(OBJ)/* $(APP)/delayer $(APP)/controller $(APP)/robot $(APP)/server 
	rm -f $(APP)/client $(LIB)/libdelayer.so
	sudo rm -f $(USR_LIB)/libdelayer.so
