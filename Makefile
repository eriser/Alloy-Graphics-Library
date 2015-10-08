
SRC := $(wildcard src/core/*.cpp)
OBJS := $(patsubst %.cpp,%.o,$(wildcard src/core/*.cpp))
DS := $(patsubst %.cpp,%.d,$(wildcard src/core/*.cpp src/example/*.cpp))
EXOBJS := $(patsubst %.cpp,%.o,$(wildcard src/example/*.cpp))
CXX = g++
CC = gcc
INC = -Iinclude/core/ -Iinclude/example/
CXXFLAGS:= -std=c++0x -DGL_GLEXT_PROTOTYPES=1 -O3 -Wall -c -fmessage-length=0 -MMD -MP -fopenmp $(INC)
override CXXFLAGS:= -DEXAMPLE_NAME=$(EXAMPLE) $(CXXFLAGS)
LDLIBS =-L./ -L/usr/lib/ -L/usr/local/lib/ -L/usr/lib/x86_64-linux-gnu/
LIBS =-lglfw -lgomp -lGL -lXext -lGLU -lGLEW -lXi -lXrandr -lX11 -lXxf86vm -lXinerama -lXcursor -lXdamage -ldrm -lpthread -lfreetype -lm -lrt -ldl -lstdc++
RM=rm -f

alloy : $(OBJS)
	ar ru liballoy.a $(OBJS)
	ranlib liballoy.a
example : $(EXOBJS)
	$(CXX) $(LDLIBS) -o $(EXAMPLE) $(EXOBJS) $(OBJS) $(LIBS)
clean :
	clear
	$(RM) $(OBJS) $(EXOBJS) $(DS)
	$(RM) liballoy.a
	$(RM) UnitsEx CompositeEx EventsEx TweenEx ImageEx DragEx
	
all : 
	clear
	make -j8 alloy
	$(RM) src/example/main.o
	make -j8 EXAMPLE=UnitsEx example
	$(RM) src/example/main.o
	make -j8 EXAMPLE=EventsEx example
	$(RM) src/example/main.o 
	make -j8 EXAMPLE=CompositeEx example 	
	$(RM) src/example/main.o 
	make -j8 EXAMPLE=TweenEx example 	
	$(RM) src/example/main.o 
	make -j8 EXAMPLE=ImageEx example 	
	$(RM) src/example/main.o 
	make -j8 EXAMPLE=DragEx example 	
default : all