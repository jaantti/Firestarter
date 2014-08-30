CFLAGS = `pkg-config --cflags opencv`
LIBS = `pkg-config --libs opencv` /usr/lib/x86_64-linux-gnu/libX11.so.6 /usr/lib/x86_64-linux-gnu/libXext.so.6
FILES = capture.cpp main.cpp rs232.c segmentation.cpp
OUT = bin/main
WORKINGDIR := $(shell pwd)
GITDIR = ~/Firestarter

all: clean
	g++ -std=c++0x $(CFLAGS) -o $(OUT) $(FILES) $(LIBS) -pthread

clean:
	mkdir bin
	cd bin && rm -rf *

run:
	./bin/main

pull:
	cd $(GITDIR); \
	git pull; \
	cd alternateCpp; \
	rsync -v --exclude=makefile * $(WORKINGDIR)/

