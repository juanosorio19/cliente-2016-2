# export LD_LIBRARY_PATH=$LD.LIBRARY_PATH:/usr/local/lib/

CC=g++ -std=c++11 -O3




all: compressed

compressed: compressed.cc
	$(CC) -o compressed compressed.cc -pthread

clean:
		rm compressed
