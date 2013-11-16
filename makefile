CFLAGS = `pkg-config --cflags opencv` -pthread `pkg-config gtk+-2.0 --cflags` -O3
LIBS = `pkg-config --libs opencv` /usr/lib/i386-linux-gnu/libX11.so.6 /usr/lib/i386-linux-gnu/libXext.so.6 -pthread -lX11 -lXext `pkg-config gtk+-2.0 --libs`
FILES = capture.cpp main.cpp rs232.c segmentation.cpp r_video.cpp
HEADERS = capture.h main.h rs232.h segmentation.h r_video.h
OUT = makebin/main
WORKINGDIR := $(shell pwd)
GITDIR = ~/Firestarter

all: clean
	g++ -std=c++0x $(CFLAGS) -o $(OUT) $(FILES) $(LIBS)

clean:
	rm -rf makebin/*
	rmdir makebin
	mkdir makebin
	cd makebin && rm -rf *

run:
	./makebin/main

yellow:
	sh makeconf/cam.sh
	cp makeconf/conf makebin/
	./makebin/main yellow
blue:
	sh makeconf/cam.sh
	cp makeconf/conf makebin/
	./makebin/main blue

pull:
	cd $(GITDIR); \
	git pull; \
	cd alternateCpp; \
	rsync -v --exclude=makefile * $(WORKINGDIR)/

push:
	cp -v $(FILES) $(HEADERS) $(GITDIR)/alternateCpp/
	cd $(GITDIR); \
	git add .; \
	git commit -m "Autocommit"; \
	git push origin master
	

