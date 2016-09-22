CC=g++
VERSION=-std=c++14
FLAGS=-ggdb -Werror -Wall -Ofast
LIBS=-I../

all: exp-map

exp-map: testExpMap.cpp ../ExpiringMap.h
	$(CC) $(VERSION) $(FLAGS) $(LIBS) $< -o testExpMap

clean:
	rm -rf testExpMap
	rm -rf *.dSYM *.core
