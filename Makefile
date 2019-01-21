CC = g++
CFLAGS = -std=c++17

.PHONY: clean all
all: shake128

shake.o: shake.cpp shake.h
	$(CC) $(CFLAGS) -c shake.cpp

sponge.o: sponge.cpp sponge.h
	$(CC) $(CFLAGS) -c sponge.cpp

main.o: main.cpp shake.h
	$(CC) $(CFLAGS) -c main.cpp

shake128: main.o shake.o sponge.o
	$(CC) $(CFLAGS) -o $@ $^

clean:
	rm -f shake.o sponge.o main.o shake128
