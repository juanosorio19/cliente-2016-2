# export LD_LIBRARY_PATH=$LD.LIBRARY_PATH:/usr/local/lib/

CC=g++ -std=c++11 





all: client server

client: client.cc
	
	$(CC) -o client client.cc -lzmq -lzmqpp

server: server.cc
	
	$(CC) -o server server.cc -lzmq -lzmqpp


