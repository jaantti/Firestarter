CFLAGS = `pkg-config --cflags opencv`
LIBS = `pkg-config --libs opencv` /usr/lib/i386-linux-gnu/libX11.so.6 /usr/lib/i386-linux-gnu/libXext.so.6
FILES = capture.cpp main.opencv.cpp rs232.c segmentation.cpp
OUT = bin/main

all: clean
	g++ $(CFLAGS) -o $(OUT) $(FILES) $(LIBS)

clean:
	cd bin && rm -rf *

run:
	./bin/main

