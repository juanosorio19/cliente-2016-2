# export LD_LIBRARY_PATH=$LD.LIBRARY_PATH:/usr/local/lib/

CC=g++ -std=c++11 -O0 -ggdb
#CC=g++ -O3 -std=c++11 





all: compressed

Disperse: compressed.cc	
	$(CC) -o compressed compressed.cc -pthread
