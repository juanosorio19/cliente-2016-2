# export LD_LIBRARY_PATH=$LD.LIBRARY_PATH:/usr/local/lib/

CC=g++ -std=c++11 





all: client server

client: client.cpp
	
	$(CC) -o client client.cpp -lzmq -lzmqpp

server: server.cpp
	
	$(CC) -o server server.cpp -lzmq -lzmqpp


