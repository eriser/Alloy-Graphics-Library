
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
LIBS =-lalloy -lglfw -lgomp -lGL -lXext -lGLU -lGLEW -lXi -lXrandr -lX11 -lXxf86vm -lXinerama -lXcursor -lXdamage -ldrm -lpthread -lfreetype -lm -lrt -ldl -lstdc++
RM=rm -f

EXAMPLES:=UnitsEx CompositeEx EventsEx TweenEx ImageEx DragEx ControlsEx DialogsEx ExpandEx MeshMatcapEx MeshWireframeEx MeshTextureEx MeshVertexColorEx MeshParticleEx MeshDepthEx MeshPhongEx LaplaceFillEx

alloy : $(OBJS)
	ar ru liballoy.a $(OBJS)
	ranlib liballoy.a
example : $(EXOBJS)
	$(CXX) $(LDLIBS) -o $(EXAMPLE) $(EXOBJS) $(LIBS)
clean :
	clear
	$(RM) $(OBJS) $(EXOBJS) $(DS)
	$(RM) liballoy.a
	$(RM) $(EXAMPLES)
	
all : 
	clear
	make -j8 alloy
	for ex in $(EXAMPLES); do \
	$(RM) src/example/main.o; \
	make -j8 EXAMPLE=$$ex example; \
	done
default : all