#######################
#Uncomment correct LIBX
#######################

#32-bit system xLib
#LIBX = /usr/lib/i386-linux-gnu/libX11.so.6 /usr/lib/i386-linux-gnu/libXext.so.6

#64-bit system xLib
LIBX = /usr/lib/x86_64-linux-gnu/libX11.so.6 /usr/lib/x86_64-linux-gnu/libXext.so.6

##############
#RakNet dir
##############

RAKNETDIR = /home/antti/Desktop/RakNet-master

##################
# Boost location #
##################

# Use the directory that *contains* the boost directory
BOOST_HEADERS = /usr/include/boost
BOOST_LIBRARIES = /usr/lib/x86_64-linux-gnu/
# The flavour is the suffix in the library name
# For example: if the library is libboost_filesystem-mt.a, then FLAVOUR is -mt
BOOST_FLAVOUR =


CXX = g++
CFLAGS = `pkg-config --cflags opencv` -Wall -std=c++11 -I$(RAKNETDIR)/Source -I$(BOOST_HEADERS) -Iinclude -g
LIBS = `pkg-config --libs opencv` $(LIBX) -L$(RAKNETDIR)/Lib/LibStatic -lRakNetLibStatic -L$(BOOST_LIBRARIES) -lboost_thread$(BOOST_FLAVOUR) -lboost_system
APP = bin/main
SRCDIR = src/
OBJDIR = obj/

SRC = $(wildcard $(SRCDIR)*.cpp)

OBJS = $(SRC:$(SRCDIR)%.cpp=$(OBJDIR)%.o)

build: $(OBJS) obj/rs232.o
	$(CXX) $(OBJS) obj/rs232.o -o $(APP) $(LIBS)

$(OBJDIR)%.o: $(SRCDIR)%.cpp
	$(CXX) $(CFLAGS) -c $(SRCDIR)$*.cpp -o $(OBJDIR)$*.o

obj/rs232.o: src/rs232.c
	$(CXX) $(CFLAGS) -c src/rs232.c -o obj/rs232.o

clean: cleandoc
	rm -fv $(OBJS)
	rm -fv obj/rs232.o
	rm -fv $(APP)
	
doc:
	doxygen

cleandoc:
	rm -rfv docs/*

