EXE = grass
CPP = g++
CPPFLAGS = -w -O3 -g

# Linux (default)
LDFLAGS = -lglut -lGLU -lGL -lGLEW

# OS X
ifeq "$(shell uname)" "Darwin"
LDFLAGS = -framework Cocoa -framework OpenGL -framework GLUT -DGL_GLEXT_PROTOTYPES
endif

CPPLIBS = $(LDFLAGS) -lncurses

$(EXE): main.o Scene.o Shader.o GLSL_helper.o glutMaster.o glutWindow.o cameraWindow.o Bitmap.o
	$(CPP) $(CPPFLAGS) $^ -o $@ $(CPPLIBS)

main.o: main.cpp cameraWindow.h GLSL_helper.h Shader.h Scene.h glutMaster/glutMaster.h Bitmap.h
	$(CPP) $(CPPFLAGS) -c main.cpp

Shader.o: Shader.h Shader.cc GLSL_helper.h glutMaster/glutMaster.h Bitmap.h
	$(CPP) $(CPPFLAGS) -c Shader.cc

GLSL_helper.o: GLSL_helper.h GLSL_helper.cpp
	$(CPP) $(CPPFLAGS) -c GLSL_helper.cpp

Scene.o: Scene.cc Scene.h glutMaster/glutMaster.h
	$(CPP) $(CPPFLAGS) -c Scene.cc

glutMaster.o: glutMaster/glutMaster.cc glutMaster/glutMaster.h
	$(CPP) $(CPPFLAGS) -c glutMaster/glutMaster.cc

glutWindow.o: glutMaster/glutWindow.cc glutMaster/glutWindow.h
	$(CPP) $(CPPFLAGS) -c glutMaster/glutWindow.cc

cameraWindow.o: cameraWindow.h cameraWindow.cc glutMaster/glutMaster.h Shader.h Scene.h GLSL_helper.h Bitmap.h
	$(CPP) $(CPPFLAGS) -c cameraWindow.cc

Bitmap.o: Bitmap.cpp Bitmap.h
	$(CPP) $(CPPFLAGS) -c Bitmap.cpp

clean : 
	rm *.o 
