
CC = g++
CFLAGS = -Wall -g
SOURCES = server.cpp module.cpp eventmgr.cpp main.cpp threadpool.cpp
OBJECTS = $(SOURCES:.c=.o)
MODULES = processes.so time.so diskinfo.so

.PHONY: all clean

all: server $(MODULES)

clean:
	rm -f $(OBJECTS) server

obj: $(SOURCES)
	$(CC) $(CFLAGS) -cpp $@ $^
server: $(OBJECTS)
	$(CC) $(CFLAGS) -Wl,-export-dynamic -o $@ $^ -ldl -lpthread

$(MODULES):
%.so: %.cpp server.h defines.h
	$(CC) $(CFLAGS) -fPIC -shared -o $@ $<