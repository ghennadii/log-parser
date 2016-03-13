CC=g++
CFLAGS=-Wall -std=c++0x

all: aggregate generate

aggregate:
	$(CC) $(CFLAGS) aggregate.cpp -o aggregate

generate:
	$(CC) $(CFLAGS) generate.cpp -o generate

clean:
	rm -rf *.o aggregate generate
