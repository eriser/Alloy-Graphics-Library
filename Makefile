
SRC := $(wildcard src/core/*.cpp src/example/*.cpp)
OBJS := $(patsubst %.cpp,%.o,$(wildcard src/core/*.cpp))
EXOBJS := $(patsubst %.cpp,%.o,$(wildcard src/example/*.cpp))
CXX = g++
CC = gcc
INC = -Iinclude/core/
CXXFLAGS = -std=c++0x -DGL_GLEXT_PROTOTYPES=1 -O3 -w -c -fmessage-length=0 -MMD -MP -fopenmp $(INC)
LDLIBS =-L/usr/lib/ -L/usr/local/lib/ -L/usr/lib/x86_64-linux-gnu/
LIBS =-lalloy -lglfw -lgomp -lGL -lXext -lGLU -lGLEW -lXi -lXrandr -lX11 -lXxf86vm -lXinerama -lXcursor -lXdamage -ldrm -lpthread -lfreetype -lm -lrt -ldl -lstdc++
RM=rm -f

default : liballoy.a

example : $(EXOBJS)
	$(CXX) $(LDLIBS) -o "example" $(EXOBJS) $(LIBS) 
	
liballoy.a : $(OBJS)
	ar ru $@ $^
	ranlib $@
	
clean :
	clear
	$(RM) $(OBJS) $(EXOBJS)
	
all : clean liballoy.a example