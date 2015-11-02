
SRC := $(wildcard src/core/*.cpp,src/core/*.c)
OBJS := $(patsubst %.cpp,%.o,$(wildcard src/core/*.cpp)) $(patsubst %.c,%.o,$(wildcard src/core/*.c))
DS := $(patsubst %.cpp,%.d,$(wildcard src/core/*.cpp src/example/*.cpp))
EXOBJS := $(patsubst %.cpp,%.o,$(wildcard src/example/*.cpp))
#Must use at least gcc/g++ 4.8 for C++11 support. Some systems call it gcc48 or gcc-48
#Standard compiler version on MacOS is 4.2.1 (no good)
CXX = g++
CC = gcc
INC = -Iinclude/core/ -Iinclude/example/ -I/usr/local/include/ -I/usr/include/
CXXFLAGS:= -std=c++0x -DGL_GLEXT_PROTOTYPES=1 -O3 -Wall -c -fmessage-length=0 -MMD -MP -fopenmp $(INC)
CFLAGS:= -DGL_GLEXT_PROTOTYPES=1 -O3 -Wall -c -fmessage-length=0 -MMD -MP -fopenmp $(INC)
override CXXFLAGS:= -DEXAMPLE_NAME=$(EXAMPLE) $(CXXFLAGS)
LDLIBS =-L./ -L/usr/lib/ -L/usr/local/lib/ -L/usr/lib/x86_64-linux-gnu/
#Pacakge dependencies are glfw 3.x, and glew, which are avaiable through apt-get and brew.
#Ubuntu may also require GLU, GLX, Xrandr, Xxf86vm, Xinerama, and Xi to build glfw from source.
#Other dependencies are standard on Ubuntu and MacOS.
#Some systems will use -lglfw3 
LIBS =-lalloy -lglfw -lgomp -lGL -lXext -lGLU -lGLEW -lXi -lXrandr -lX11 -lXxf86vm -lXinerama -lXcursor -lXdamage -lpthread -lm -ldl -lstdc++
RM=rm -f

EXAMPLES:=UnitsEx CompositeEx EventsEx TweenEx ImageEx DragEx ControlsEx \
 DialogsEx ExpandEx MeshMatcapEx MeshWireframeEx MeshTextureEx MeshVertexColorEx \
 MeshParticleEx MeshDepthEx MeshPhongEx LaplaceFillEx PoissonBlendEx \
 PoissonInpaintEx MeshSubdivideEx ImageProcessingEx MeshPickerEx KdTreeEx MeshSmoothEx \
 ColorSpaceEx MeshPrimitivesEx MenuEx LocatorEx GraphEx WindowsEx

alloy : $(OBJS)
	ar ru liballoy.a $(OBJS)
	ranlib liballoy.a

example : $(EXOBJS)
	$(CXX) $(LDLIBS) -o $(EXAMPLE) $(EXOBJS) $(LIBS)

examples : $(EXOBJS)
	$(CXX) $(LDLIBS) -o AlloyExamples $(EXOBJS) $(LIBS)
		
clean :
	clear
	$(RM) $(OBJS) $(EXOBJS) $(DS)
	$(RM) liballoy.a
	$(RM) $(EXAMPLES) AlloyExamples
	
all : 
	clear
	make -j8 alloy
	make -j8 examples
default : all