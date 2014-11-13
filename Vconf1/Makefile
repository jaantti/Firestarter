CC = g++
FILES = main.cpp capture.cpp r_video.cpp

CFLAGS = `pkg-config gtk+-2.0 --cflags`
LDFLAGS = -lX11 -lXext `pkg-config gtk+-2.0 --libs`


vision_conf:
	$(CC)$  $(CFLAGS)$  $(FILES)$  -o vision_conf  $(LDFLAGS)

clean: 
	rm -f vision_conf
