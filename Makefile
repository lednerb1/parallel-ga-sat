all:
	rm -rf bin && mkdir bin
	mpic++ src/main.cpp -std=c++2a -O2 -fopenmp -o bin/ksat.out
debug:
	rm -rf debug && mkdir debug
	mpic++ src/main.cpp -std=c++2a -O2 -g -DDEBUG -o debug/ksat.out
clean:
	rm -rf bin debug
